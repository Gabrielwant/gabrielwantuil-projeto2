#include "visibilidade.h"
#include "arvore.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX_PONTOS 1000
#define RAIO_VISIBILIDADE 10000.0

typedef struct
{
  Ponto ponto;
  double angulo;
} PontoAngular;

double calcularAngulo(double x, double y, double ox, double oy)
{
  return atan2(y - oy, x - ox);
}

int compararPontosAngulares(const void *a, const void *b)
{
  PontoAngular *pa = (PontoAngular *)a;
  PontoAngular *pb = (PontoAngular *)b;
  if (pa->angulo < pb->angulo)
    return -1;
  if (pa->angulo > pb->angulo)
    return 1;
  return 0;
}

int raioIntersectaSegmento(Ponto origem, double angulo, Ponto p1, Ponto p2, Ponto *intersecao)
{
  Ponto destino;
  destino.x = origem.x + RAIO_VISIBILIDADE * cos(angulo);
  destino.y = origem.y + RAIO_VISIBILIDADE * sin(angulo);

  if (segmentosIntersectam(origem, destino, p1, p2))
  {
    *intersecao = intersecaoSegmentos(origem, destino, p1, p2);
    return 1;
  }
  return 0;
}

Ponto encontrarPontoVisivel(Lista *formas, Ponto origem, double angulo)
{
  Ponto maisProximo;
  maisProximo.x = origem.x + RAIO_VISIBILIDADE * cos(angulo);
  maisProximo.y = origem.y + RAIO_VISIBILIDADE * sin(angulo);

  double distanciaMin = RAIO_VISIBILIDADE;

  No *no = formas->inicio;
  while (no)
  {
    if (!no->dado)
    {
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;

    if (forma->ativo && forma->tipo == SEGMENTO)
    {
      Ponto intersecao;
      if (raioIntersectaSegmento(origem, angulo,
                                 forma->dados.linha.p1,
                                 forma->dados.linha.p2,
                                 &intersecao))
      {
        double dist = distanciaPontos(origem, intersecao);
        if (dist < distanciaMin)
        {
          distanciaMin = dist;
          maisProximo = intersecao;
        }
      }
    }
    no = no->prox;
  }

  return maisProximo;
}

Poligono *calcularVisibilidade(Lista *formas, double px, double py)
{
  if (!formas)
  {
    fprintf(stderr, "ERRO: calcularVisibilidade - lista de formas é NULL\n");
    return NULL;
  }

  Ponto origem = {px, py};
  PontoAngular pontosAngulares[MAX_PONTOS];
  int numPontos = 0;

  No *no = formas->inicio;
  while (no && numPontos < MAX_PONTOS - 8)
  {
    if (!no->dado)
    {
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;

    if (forma->ativo && forma->tipo == SEGMENTO)
    {
      Ponto p1 = forma->dados.linha.p1;
      Ponto p2 = forma->dados.linha.p2;

      double ang1 = calcularAngulo(p1.x, p1.y, px, py);
      double ang2 = calcularAngulo(p2.x, p2.y, px, py);

      double epsilon = 0.0001;

      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1 - epsilon};
      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1};
      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1 + epsilon};

      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2 - epsilon};
      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2};
      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2 + epsilon};
    }

    no = no->prox;
  }

  if (numPontos == 0)
  {
    Poligono *pol = criarPoligono(4);
    if (!pol)
      return NULL;

    adicionarVerticePoligono(pol, criarPonto(px - 1000, py - 1000));
    adicionarVerticePoligono(pol, criarPonto(px + 1000, py - 1000));
    adicionarVerticePoligono(pol, criarPonto(px + 1000, py + 1000));
    adicionarVerticePoligono(pol, criarPonto(px - 1000, py + 1000));

    return pol;
  }

  qsort(pontosAngulares, numPontos, sizeof(PontoAngular), compararPontosAngulares);

  Poligono *pol = criarPoligono(numPontos);
  if (!pol)
  {
    fprintf(stderr, "ERRO: calcularVisibilidade - falha ao criar polígono\n");
    return NULL;
  }

  for (int i = 0; i < numPontos; i++)
  {
    Ponto pontoVisivel = encontrarPontoVisivel(formas, origem, pontosAngulares[i].angulo);
    adicionarVerticePoligono(pol, pontoVisivel);
  }

  return pol;
}

// CORRIGIDO: Agora transforma TODAS as formas em anteparos
void transformarAnteparos(Lista *formas, int idMin, int idMax, char orientacao, FILE *txtOut)
{
  if (!formas || !txtOut)
  {
    fprintf(stderr, "ERRO: transformarAnteparos - parâmetros inválidos\n");
    return;
  }

  No *no = formas->inicio;
  int proximoId = obterProximoId(formas);
  Lista *novosSegmentos = criarLista();

  while (no)
  {
    if (!no->dado)
    {
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;

    if (forma->ativo && forma->id >= idMin && forma->id <= idMax)
    {
      fprintf(txtOut, "  Transformando forma %d (%s) em anteparo\n",
              forma->id, obterTipoForma(forma->tipo));

      if (forma->tipo == LINHA)
      {
        // Linha vira segmento (mantém o mesmo)
        forma->tipo = SEGMENTO;
        fprintf(txtOut, "    -> Segmento mantido (ID %d)\n", forma->id);
      }
      else if (forma->tipo == CIRCULO)
      {
        // Círculo vira 1 segmento (horizontal ou vertical)
        double cx = forma->dados.circulo.centro.x;
        double cy = forma->dados.circulo.centro.y;
        double r = forma->dados.circulo.raio;

        Forma *seg;
        if (orientacao == 'h')
        {
          // Segmento horizontal
          seg = criarSegmento(proximoId, cx - r, cy, cx + r, cy, forma->corBorda);
          fprintf(txtOut, "    Círculo %d -> Segmento horizontal ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                  forma->id, proximoId, cx - r, cy, cx + r, cy);
          proximoId++;
        }
        else
        {
          // Segmento vertical
          seg = criarSegmento(proximoId, cx, cy - r, cx, cy + r, forma->corBorda);
          fprintf(txtOut, "    Círculo %d -> Segmento vertical ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                  forma->id, proximoId, cx, cy - r, cx, cy + r);
          proximoId++;
        }

        inserirLista(novosSegmentos, seg);
        forma->ativo = 0; // Desativa o círculo original
      }
      else if (forma->tipo == RETANGULO)
      {
        // Retângulo vira 4 segmentos (os 4 lados)
        double x = forma->dados.retangulo.ancora.x;
        double y = forma->dados.retangulo.ancora.y;
        double w = forma->dados.retangulo.largura;
        double h = forma->dados.retangulo.altura;

        // Lado superior
        Forma *seg1 = criarSegmento(proximoId, x, y, x + w, y, forma->corBorda);
        fprintf(txtOut, "    Retângulo %d -> Segmento superior ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                forma->id, proximoId, x, y, x + w, y);
        inserirLista(novosSegmentos, seg1);
        proximoId++;

        // Lado direito
        Forma *seg2 = criarSegmento(proximoId, x + w, y, x + w, y + h, forma->corBorda);
        fprintf(txtOut, "    Retângulo %d -> Segmento direito ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                forma->id, proximoId, x + w, y, x + w, y + h);
        inserirLista(novosSegmentos, seg2);
        proximoId++;

        // Lado inferior
        Forma *seg3 = criarSegmento(proximoId, x + w, y + h, x, y + h, forma->corBorda);
        fprintf(txtOut, "    Retângulo %d -> Segmento inferior ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                forma->id, proximoId, x + w, y + h, x, y + h);
        inserirLista(novosSegmentos, seg3);
        proximoId++;

        // Lado esquerdo
        Forma *seg4 = criarSegmento(proximoId, x, y + h, x, y, forma->corBorda);
        fprintf(txtOut, "    Retângulo %d -> Segmento esquerdo ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                forma->id, proximoId, x, y + h, x, y);
        inserirLista(novosSegmentos, seg4);
        proximoId++;

        forma->ativo = 0; // Desativa o retângulo original
      }
      else if (forma->tipo == TEXTO)
      {
        // Texto vira 1 segmento horizontal
        double x = forma->dados.texto.ancora.x;
        double y = forma->dados.texto.ancora.y;
        int tamanho = strlen(forma->dados.texto.texto);
        double comprimento = 10.0 * tamanho;

        double x1, x2;
        if (forma->dados.texto.posicaoAncora == 'i')
        {
          x1 = x;
          x2 = x + comprimento;
        }
        else if (forma->dados.texto.posicaoAncora == 'f')
        {
          x1 = x - comprimento;
          x2 = x;
        }
        else // 'm'
        {
          x1 = x - comprimento / 2;
          x2 = x + comprimento / 2;
        }

        Forma *seg = criarSegmento(proximoId, x1, y, x2, y, forma->corBorda);
        fprintf(txtOut, "    Texto %d -> Segmento ID %d: extremos (%.2f, %.2f) - (%.2f, %.2f)\n",
                forma->id, proximoId, x1, y, x2, y);
        inserirLista(novosSegmentos, seg);
        proximoId++;

        forma->ativo = 0; // Desativa o texto original
      }
    }

    no = no->prox;
  }

  // Adiciona os novos segmentos à lista principal
  no = novosSegmentos->inicio;
  while (no)
  {
    inserirLista(formas, no->dado);
    no = no->prox;
  }

  free(novosSegmentos); // Libera apenas a estrutura da lista, não os dados
}

// CORRIGIDO: Verificação mais precisa se forma está na região
int formaEmRegiao(Forma *forma, Poligono *regiao)
{
  if (!forma || !regiao)
  {
    return 0;
  }

  // Para círculos: verifica se o centro está dentro OU se algum vértice do polígono está perto
  if (forma->tipo == CIRCULO)
  {
    Ponto centro = forma->dados.circulo.centro;
    double raio = forma->dados.circulo.raio;

    // Se centro está dentro, está na região
    if (pontoEmPoligono(centro, regiao))
      return 1;

    // Verifica se algum vértice do polígono está dentro do círculo
    for (int i = 0; i < regiao->numVertices; i++)
    {
      if (distanciaPontos(centro, regiao->vertices[i]) <= raio)
        return 1;
    }

    // Verifica se alguma aresta do polígono está perto do centro
    for (int i = 0; i < regiao->numVertices; i++)
    {
      int j = (i + 1) % regiao->numVertices;
      // Distância do ponto à linha (implementação simplificada)
      double dist = fabs(produtoVetorial(regiao->vertices[i], regiao->vertices[j], centro)) /
                    distanciaPontos(regiao->vertices[i], regiao->vertices[j]);
      if (dist <= raio && pontoNoSegmento(centro, regiao->vertices[i], regiao->vertices[j]))
        return 1;
    }

    return 0;
  }

  // Para retângulos: verifica os 4 vértices
  if (forma->tipo == RETANGULO)
  {
    double x = forma->dados.retangulo.ancora.x;
    double y = forma->dados.retangulo.ancora.y;
    double w = forma->dados.retangulo.largura;
    double h = forma->dados.retangulo.altura;

    Ponto vertices[4] = {
        {x, y},
        {x + w, y},
        {x + w, y + h},
        {x, y + h}};

    // Se qualquer vértice está dentro, está na região
    for (int i = 0; i < 4; i++)
    {
      if (pontoEmPoligono(vertices[i], regiao))
        return 1;
    }

    // Verifica se algum vértice da região está dentro do retângulo
    for (int i = 0; i < regiao->numVertices; i++)
    {
      Ponto p = regiao->vertices[i];
      if (p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h)
        return 1;
    }

    return 0;
  }

  // Para linhas e segmentos: verifica se uma das extremidades está dentro
  if (forma->tipo == LINHA || forma->tipo == SEGMENTO)
  {
    if (pontoEmPoligono(forma->dados.linha.p1, regiao))
      return 1;
    if (pontoEmPoligono(forma->dados.linha.p2, regiao))
      return 1;

    // Verifica se a linha intersecta alguma aresta do polígono
    for (int i = 0; i < regiao->numVertices; i++)
    {
      int j = (i + 1) % regiao->numVertices;
      if (segmentosIntersectam(forma->dados.linha.p1, forma->dados.linha.p2,
                               regiao->vertices[i], regiao->vertices[j]))
        return 1;
    }

    return 0;
  }

  // Para texto: verifica apenas a âncora
  if (forma->tipo == TEXTO)
  {
    return pontoEmPoligono(forma->dados.texto.ancora, regiao);
  }

  return 0;
}

Forma *clonarForma(Forma *forma, int novoId, double dx, double dy)
{
  if (!forma)
  {
    fprintf(stderr, "ERRO: clonarForma - forma é NULL\n");
    return NULL;
  }

  Forma *clone = NULL;

  switch (forma->tipo)
  {
  case CIRCULO:
    clone = criarCirculo(
        novoId,
        forma->dados.circulo.centro.x + dx,
        forma->dados.circulo.centro.y + dy,
        forma->dados.circulo.raio,
        forma->corBorda,
        forma->corPreenchimento);
    break;

  case RETANGULO:
    clone = criarRetangulo(
        novoId,
        forma->dados.retangulo.ancora.x + dx,
        forma->dados.retangulo.ancora.y + dy,
        forma->dados.retangulo.largura,
        forma->dados.retangulo.altura,
        forma->corBorda,
        forma->corPreenchimento);
    break;

  case LINHA:
    clone = criarLinha(
        novoId,
        forma->dados.linha.p1.x + dx,
        forma->dados.linha.p1.y + dy,
        forma->dados.linha.p2.x + dx,
        forma->dados.linha.p2.y + dy,
        forma->corBorda);
    break;

  case SEGMENTO:
    clone = criarSegmento(
        novoId,
        forma->dados.linha.p1.x + dx,
        forma->dados.linha.p1.y + dy,
        forma->dados.linha.p2.x + dx,
        forma->dados.linha.p2.y + dy,
        forma->corBorda);
    break;

  case TEXTO:
    clone = criarTexto(
        novoId,
        forma->dados.texto.ancora.x + dx,
        forma->dados.texto.ancora.y + dy,
        forma->corBorda,
        forma->corPreenchimento,
        forma->dados.texto.posicaoAncora,
        forma->dados.texto.texto,
        &forma->dados.texto.estilo);
    break;

  default:
    fprintf(stderr, "AVISO: clonarForma - tipo de forma desconhecido: %d\n", forma->tipo);
    break;
  }

  if (!clone)
  {
    fprintf(stderr, "ERRO: clonarForma - falha ao criar clone da forma %d\n", forma->id);
  }

  return clone;
}

int obterProximoId(Lista *formas)
{
  if (!formas)
  {
    fprintf(stderr, "ERRO: obterProximoId - lista de formas é NULL\n");
    return 1;
  }

  int maxId = 0;
  No *no = formas->inicio;

  while (no)
  {
    if (!no->dado)
    {
      fprintf(stderr, "AVISO: obterProximoId - nó com dado NULL encontrado\n");
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;
    if (forma->id > maxId)
    {
      maxId = forma->id;
    }
    no = no->prox;
  }

  return maxId + 1;
}