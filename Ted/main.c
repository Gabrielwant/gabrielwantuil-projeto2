#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "geometria.h"
#include "lista.h"
#include "arvore.h"
#include "visibilidade.h"
#include "svg.h"

typedef struct
{
  char *dirEntrada;
  char *arquivoGeo;
  char *dirSaida;
  char *arquivoQry;
  char tipoOrdenacao;
  int limiteInsertionSort;
} Parametros;

void inicializarParametros(Parametros *params)
{
  params->dirEntrada = ".";
  params->arquivoGeo = NULL;
  params->dirSaida = NULL;
  params->arquivoQry = NULL;
  params->tipoOrdenacao = 'q';
  params->limiteInsertionSort = 10;
}

void parseArgumentos(int argc, char *argv[], Parametros *params)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc)
    {
      params->dirEntrada = argv[++i];
    }
    else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
    {
      params->arquivoGeo = argv[++i];
    }
    else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
    {
      params->dirSaida = argv[++i];
    }
    else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc)
    {
      params->arquivoQry = argv[++i];
    }
    else if (strcmp(argv[i], "-to") == 0 && i + 1 < argc)
    {
      params->tipoOrdenacao = argv[++i][0];
    }
    else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
    {
      params->limiteInsertionSort = atoi(argv[++i]);
    }
  }
}

void construirCaminho(char *destino, const char *dir, const char *arquivo)
{
  snprintf(destino, 512, "%s/%s", dir, arquivo);
}

// Extrai apenas o nome base do arquivo sem diretórios e sem extensão
void extrairNomeBase(const char *caminho, char *nomeBase)
{
  // Copia o caminho
  strcpy(nomeBase, caminho);

  // Remove diretórios (pega após a última barra)
  char *ultimaBarra = strrchr(nomeBase, '/');
  if (ultimaBarra)
  {
    // CORREÇÃO: copiar corretamente até o final da string
    size_t tamanho = strlen(ultimaBarra + 1) + 1; // +1 para incluir o \0
    memmove(nomeBase, ultimaBarra + 1, tamanho);
  }

  // Remove extensão (remove após o último ponto)
  char *ponto = strrchr(nomeBase, '.');
  if (ponto)
  {
    *ponto = '\0';
  }
}

void processarArquivoGeo(const char *caminhoGeo, Lista *formas, EstiloTexto *estilo)
{
  FILE *arquivo = fopen(caminhoGeo, "r");
  if (!arquivo)
  {
    fprintf(stderr, "Erro ao abrir arquivo .geo: %s\n", caminhoGeo);
    exit(1);
  }

  char linha[1024];
  while (fgets(linha, sizeof(linha), arquivo))
  {
    char cmd;
    if (sscanf(linha, " %c", &cmd) != 1)
      continue;

    if (cmd == 'c')
    {
      int id;
      double x, y, r;
      char corb[64], corp[64];
      if (sscanf(linha, "c %d %lf %lf %lf %s %s", &id, &x, &y, &r, corb, corp) == 6)
      {
        Forma *forma = criarCirculo(id, x, y, r, corb, corp);
        inserirLista(formas, forma);
      }
    }
    else if (cmd == 'r')
    {
      int id;
      double x, y, w, h;
      char corb[64], corp[64];
      if (sscanf(linha, "r %d %lf %lf %lf %lf %s %s", &id, &x, &y, &w, &h, corb, corp) == 7)
      {
        Forma *forma = criarRetangulo(id, x, y, w, h, corb, corp);
        inserirLista(formas, forma);
      }
    }
    else if (cmd == 'l')
    {
      int id;
      double x1, y1, x2, y2;
      char cor[64];
      if (sscanf(linha, "l %d %lf %lf %lf %lf %s", &id, &x1, &y1, &x2, &y2, cor) == 6)
      {
        Forma *forma = criarLinha(id, x1, y1, x2, y2, cor);
        inserirLista(formas, forma);
      }
    }
    else if (cmd == 't')
    {
      int id;
      double x, y;
      char corb[64], corp[64], ancora;
      char texto[512];
      char *ptr = strchr(linha, ' ');
      if (ptr && sscanf(ptr, "%d %lf %lf %s %s %c ", &id, &x, &y, corb, corp, &ancora) == 6)
      {
        ptr = strchr(ptr + 1, ' ');
        ptr = strchr(ptr + 1, ' ');
        ptr = strchr(ptr + 1, ' ');
        ptr = strchr(ptr + 1, ' ');
        ptr = strchr(ptr + 1, ' ');
        ptr = strchr(ptr + 1, ' ');
        if (ptr)
        {
          ptr++;
          strcpy(texto, ptr);
          texto[strcspn(texto, "\n")] = 0;
          Forma *forma = criarTexto(id, x, y, corb, corp, ancora, texto, estilo);
          inserirLista(formas, forma);
        }
      }
    }
    else if (cmd == 't' && linha[1] == 's')
    {
      char family[32], weight[32];
      int size;
      if (sscanf(linha, "ts %s %s %d", family, weight, &size) == 3)
      {
        strcpy(estilo->family, family);
        strcpy(estilo->weight, weight);
        estilo->size = size;
      }
    }
  }

  fclose(arquivo);
}

void processarArquivoQry(const char *caminhoQry, const char *nomeBaseGeo,
                         const char *nomeBaseQry, Lista *formas,
                         const char *dirSaida, FILE *txtOut)
{
  FILE *arquivo = fopen(caminhoQry, "r");
  if (!arquivo)
  {
    fprintf(stderr, "Erro ao abrir arquivo .qry: %s\n", caminhoQry);
    return;
  }

  // VERIFICAÇÃO CRÍTICA: Se txtOut for NULL, não podemos continuar
  if (!txtOut)
  {
    fprintf(stderr, "ERRO: arquivo de saída TXT não foi aberto corretamente\n");
    fclose(arquivo);
    return;
  }

  char linha[1024];
  while (fgets(linha, sizeof(linha), arquivo))
  {
    char cmd[8];
    if (sscanf(linha, " %s", cmd) != 1)
      continue;

    if (strcmp(cmd, "a") == 0)
    {
      int i, j;
      char orientacao;
      if (sscanf(linha, "a %d %d %c", &i, &j, &orientacao) == 3)
      {
        fprintf(txtOut, "Transformando formas %d-%d em anteparos (%c)\n", i, j, orientacao);
        transformarAnteparos(formas, i, j, orientacao, txtOut);
      }
    }
    else if (strcmp(cmd, "d") == 0)
    {
      double x, y;
      char sufixo[64];
      if (sscanf(linha, "d %lf %lf %s", &x, &y, sufixo) == 3)
      {
        fprintf(txtOut, "\nBomba de destruição em (%.2f, %.2f)\n", x, y);

        Poligono *regiao = calcularVisibilidade(formas, x, y);

        // VERIFICAÇÃO: Se calcularVisibilidade retornar NULL
        if (!regiao)
        {
          fprintf(stderr, "ERRO: calcularVisibilidade retornou NULL\n");
          fprintf(txtOut, "ERRO: não foi possível calcular região de visibilidade\n");
          continue;
        }

        fprintf(txtOut, "Formas destruídas:\n");
        No *no = formas->inicio;
        while (no)
        {
          Forma *forma = (Forma *)no->dado;
          if (forma && forma->ativo && formaEmRegiao(forma, regiao))
          {
            fprintf(txtOut, "  ID: %d, Tipo: %s\n", forma->id, obterTipoForma(forma->tipo));
            forma->ativo = 0;
          }
          no = no->prox;
        }

        char caminhoSvg[512];
        if (strcmp(sufixo, "-") == 0)
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry);
        }
        else
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
        }
        desenharRegiaoVisibilidade(regiao, caminhoSvg);

        // TXT com sufixo só se sufixo não for "-"
        if (strcmp(sufixo, "-") != 0)
        {
          char caminhoTxt[512];
          snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/%s-%s-%s.txt", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
          FILE *txtSufixo = fopen(caminhoTxt, "w");
          if (txtSufixo)
          {
            fprintf(txtSufixo, "Região de visibilidade - Bomba de destruição\n");
            fprintf(txtSufixo, "Ponto: (%.2f, %.2f)\n", x, y);
            fprintf(txtSufixo, "Vértices do polígono: %d\n", regiao->numVertices);
            fclose(txtSufixo);
          }
        }

        destruirPoligono(regiao);
      }
    }
    else if (strcmp(cmd, "p") == 0)
    {
      double x, y;
      char cor[64], sufixo[64];
      if (sscanf(linha, "p %lf %lf %s %s", &x, &y, cor, sufixo) == 4)
      {
        fprintf(txtOut, "\nBomba de pintura em (%.2f, %.2f) - cor: %s\n", x, y, cor);

        Poligono *regiao = calcularVisibilidade(formas, x, y);

        if (!regiao)
        {
          fprintf(stderr, "ERRO: calcularVisibilidade retornou NULL\n");
          fprintf(txtOut, "ERRO: não foi possível calcular região de visibilidade\n");
          continue;
        }

        fprintf(txtOut, "Formas pintadas:\n");
        No *no = formas->inicio;
        while (no)
        {
          Forma *forma = (Forma *)no->dado;
          if (forma && forma->ativo && formaEmRegiao(forma, regiao))
          {
            fprintf(txtOut, "  ID: %d, Tipo: %s\n", forma->id, obterTipoForma(forma->tipo));
            strcpy(forma->corBorda, cor);
            strcpy(forma->corPreenchimento, cor);
          }
          no = no->prox;
        }

        char caminhoSvg[512];
        if (strcmp(sufixo, "-") == 0)
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry);
        }
        else
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
        }
        desenharRegiaoVisibilidade(regiao, caminhoSvg);

        // TXT com sufixo só se sufixo não for "-"
        if (strcmp(sufixo, "-") != 0)
        {
          char caminhoTxt[512];
          snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/%s-%s-%s.txt", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
          FILE *txtSufixo = fopen(caminhoTxt, "w");
          if (txtSufixo)
          {
            fprintf(txtSufixo, "Região de visibilidade - Bomba de pintura\n");
            fprintf(txtSufixo, "Ponto: (%.2f, %.2f)\n", x, y);
            fprintf(txtSufixo, "Cor: %s\n", cor);
            fprintf(txtSufixo, "Vértices do polígono: %d\n", regiao->numVertices);
            fclose(txtSufixo);
          }
        }

        destruirPoligono(regiao);
      }
    }
    else if (strcmp(cmd, "cln") == 0)
    {
      double x, y, dx, dy;
      char sufixo[64];
      if (sscanf(linha, "cln %lf %lf %lf %lf %s", &x, &y, &dx, &dy, sufixo) == 5)
      {
        fprintf(txtOut, "\nBomba de clonagem em (%.2f, %.2f) - deslocamento: (%.2f, %.2f)\n",
                x, y, dx, dy);

        Poligono *regiao = calcularVisibilidade(formas, x, y);

        if (!regiao)
        {
          fprintf(stderr, "ERRO: calcularVisibilidade retornou NULL\n");
          fprintf(txtOut, "ERRO: não foi possível calcular região de visibilidade\n");
          continue;
        }

        fprintf(txtOut, "Formas clonadas:\n");
        Lista *clones = criarLista();
        No *no = formas->inicio;
        int proximoId = obterProximoId(formas);

        while (no)
        {
          Forma *forma = (Forma *)no->dado;
          if (forma && forma->ativo && formaEmRegiao(forma, regiao))
          {
            fprintf(txtOut, "  Original ID: %d, Clone ID: %d\n", forma->id, proximoId);
            Forma *clone = clonarForma(forma, proximoId++, dx, dy);
            inserirLista(clones, clone);
          }
          no = no->prox;
        }

        no = clones->inicio;
        while (no)
        {
          inserirLista(formas, no->dado);
          no = no->prox;
        }
        free(clones);

        char caminhoSvg[512];
        if (strcmp(sufixo, "-") == 0)
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry);
        }
        else
        {
          snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/%s-%s-%s.svg", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
        }
        desenharRegiaoVisibilidade(regiao, caminhoSvg);

        // TXT com sufixo só se sufixo não for "-"
        if (strcmp(sufixo, "-") != 0)
        {
          char caminhoTxt[512];
          snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/%s-%s-%s.txt", dirSaida, nomeBaseGeo, nomeBaseQry, sufixo);
          FILE *txtSufixo = fopen(caminhoTxt, "w");
          if (txtSufixo)
          {
            fprintf(txtSufixo, "Região de visibilidade - Bomba de clonagem\n");
            fprintf(txtSufixo, "Ponto: (%.2f, %.2f)\n", x, y);
            fprintf(txtSufixo, "Deslocamento: (%.2f, %.2f)\n", dx, dy);
            fprintf(txtSufixo, "Vértices do polígono: %d\n", regiao->numVertices);
            fclose(txtSufixo);
          }
        }

        destruirPoligono(regiao);
      }
    }
  }

  fclose(arquivo);
}

int main(int argc, char *argv[])
{
  Parametros params;
  inicializarParametros(&params);
  parseArgumentos(argc, argv, &params);

  if (!params.arquivoGeo || !params.dirSaida)
  {
    fprintf(stderr, "Uso: %s -f arquivo.geo -o dirSaida [-e dirEntrada] [-q arquivo.qry] [-to q|m] [-i n]\n", argv[0]);
    return 1;
  }

  // Construir caminho completo do .geo
  char caminhoGeo[512];
  construirCaminho(caminhoGeo, params.dirEntrada, params.arquivoGeo);

  // Extrair nome base do .geo (sem diretórios, sem extensão)
  char nomeBaseGeo[256];
  extrairNomeBase(params.arquivoGeo, nomeBaseGeo);

  Lista *formas = criarLista();
  EstiloTexto estilo = {"sans", "n", 12};

  processarArquivoGeo(caminhoGeo, formas, &estilo);

  // Gerar SVG inicial
  char caminhoSvgGeo[512];
  snprintf(caminhoSvgGeo, sizeof(caminhoSvgGeo), "%s/%s.svg", params.dirSaida, nomeBaseGeo);
  gerarSVG(formas, caminhoSvgGeo);

  if (params.arquivoQry)
  {
    char caminhoQry[512];
    construirCaminho(caminhoQry, params.dirEntrada, params.arquivoQry);

    char nomeBaseQry[256];
    extrairNomeBase(params.arquivoQry, nomeBaseQry);

    // Criar arquivo TXT de saída
    char caminhoTxt[512];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/%s-%s.txt", params.dirSaida, nomeBaseGeo, nomeBaseQry);
    FILE *txtOut = fopen(caminhoTxt, "w");

    if (!txtOut)
    {
      fprintf(stderr, "ERRO: não foi possível criar arquivo de saída: %s\n", caminhoTxt);
      destruirLista(formas, destruirForma);
      return 1;
    }

    processarArquivoQry(caminhoQry, nomeBaseGeo, nomeBaseQry, formas, params.dirSaida, txtOut);

    fclose(txtOut);

    char caminhoSvgQry[512];
    snprintf(caminhoSvgQry, sizeof(caminhoSvgQry), "%s/%s-%s.svg", params.dirSaida, nomeBaseGeo, nomeBaseQry);
    gerarSVG(formas, caminhoSvgQry);
  }

  destruirLista(formas, destruirForma);

  return 0;