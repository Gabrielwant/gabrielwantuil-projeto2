#include "visibilidade.h"
#include "arvore.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX_PONTOS 1000
#define RAIO_VISIBILIDADE 10000.0

// Estrutura para armazenar um ponto com ângulo
typedef struct
{
  Ponto ponto;
  double angulo;
} PontoAngular;

// Calcula o ângulo de um ponto em relação à origem
double calcularAngulo(double x, double y, double ox, double oy)
{
  return atan2(y - oy, x - ox);
}

// Compara dois pontos angulares (para ordenação)
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

// Verifica se um raio intersecta um segmento
int raioIntersectaSegmento(Ponto origem, double angulo, Ponto p1, Ponto p2, Ponto *intersecao)
{
  // Cria um ponto muito longe na direção do ângulo
  Ponto destino;
  destino.x = origem.x + RAIO_VISIBILIDADE * cos(angulo);
  destino.y = origem.y + RAIO_VISIBILIDADE * sin(angulo);

  // Verifica se o raio intersecta o segmento
  if (segmentosIntersectam(origem, destino, p1, p2))
  {
    *intersecao = intersecaoSegmentos(origem, destino, p1, p2);
    return 1;
  }
  return 0;
}

// Encontra o ponto visível mais próximo em um determinado ângulo
Ponto encontrarPontoVisivel(Lista *formas, Ponto origem, double angulo)
{
  Ponto maisProximo;
  maisProximo.x = origem.x + RAIO_VISIBILIDADE * cos(angulo);
  maisProximo.y = origem.y + RAIO_VISIBILIDADE * sin(angulo);

  double distanciaMin = RAIO_VISIBILIDADE;

  // Verifica interseção com todos os anteparos
  No *no = formas->inicio;
  while (no)
  {
    if (!no->dado)
    {
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;

    // Apenas segmentos bloqueiam visão
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

  // Coleta todos os vértices dos anteparos
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
      // Adiciona os dois extremos do segmento
      Ponto p1 = forma->dados.linha.p1;
      Ponto p2 = forma->dados.linha.p2;

      // Para cada extremo, adiciona 3 ângulos: antes, no ponto, e depois
      double ang1 = calcularAngulo(p1.x, p1.y, px, py);
      double ang2 = calcularAngulo(p2.x, p2.y, px, py);

      double epsilon = 0.0001;

      // Adiciona ângulos ao redor de p1
      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1 - epsilon};
      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1};
      pontosAngulares[numPontos++] = (PontoAngular){p1, ang1 + epsilon};

      // Adiciona ângulos ao redor de p2
      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2 - epsilon};
      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2};
      pontosAngulares[numPontos++] = (PontoAngular){p2, ang2 + epsilon};
    }

    no = no->prox;
  }

  // Se não há anteparos, retorna uma região grande ao redor do ponto
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

  // Ordena os pontos por ângulo
  qsort(pontosAngulares, numPontos, sizeof(PontoAngular), compararPontosAngulares);

  // Cria o polígono de visibilidade
  Poligono *pol = criarPoligono(numPontos);
  if (!pol)
  {
    fprintf(stderr, "ERRO: calcularVisibilidade - falha ao criar polígono\n");
    return NULL;
  }

  // Para cada ângulo, encontra o ponto visível mais próximo
  for (int i = 0; i < numPontos; i++)
  {
    Ponto pontoVisivel = encontrarPontoVisivel(formas, origem, pontosAngulares[i].angulo);
    adicionarVerticePoligono(pol, pontoVisivel);
  }

  return pol;
}

void transformarAnteparos(Lista *formas, int idMin, int idMax, char orientacao, FILE *txtOut)
{
  if (!formas)
  {
    fprintf(stderr, "ERRO: transformarAnteparos - lista de formas é NULL\n");
    return;
  }

  if (!txtOut)
  {
    fprintf(stderr, "ERRO: transformarAnteparos - arquivo de saída é NULL\n");
    return;
  }

  No *no = formas->inicio;

  while (no)
  {
    if (!no->dado)
    {
      fprintf(stderr, "AVISO: transformarAnteparos - nó com dado NULL encontrado\n");
      no = no->prox;
      continue;
    }

    Forma *forma = (Forma *)no->dado;

    if (forma->ativo && forma->id >= idMin && forma->id <= idMax)
    {
      if (forma->tipo == LINHA)
      {
        forma->tipo = SEGMENTO;
        fprintf(txtOut, "  Forma %d transformada em anteparo (%c)\n", forma->id, orientacao);
      }
    }

    no = no->prox;
  }
}

int formaEmRegiao(Forma *forma, Poligono *regiao)
{
  if (!forma || !regiao)
  {
    return 0;
  }

  Ponto centro;

  switch (forma->tipo)
  {
  case CIRCULO:
    centro = forma->dados.circulo.centro;
    return pontoEmPoligono(centro, regiao);

  case RETANGULO:
    centro.x = forma->dados.retangulo.ancora.x + forma->dados.retangulo.largura / 2;
    centro.y = forma->dados.retangulo.ancora.y + forma->dados.retangulo.altura / 2;
    return pontoEmPoligono(centro, regiao);

  case LINHA:
  case SEGMENTO:
    centro.x = (forma->dados.linha.p1.x + forma->dados.linha.p2.x) / 2;
    centro.y = (forma->dados.linha.p1.y + forma->dados.linha.p2.y) / 2;
    return pontoEmPoligono(centro, regiao);

  case TEXTO:
    centro = forma->dados.texto.ancora;
    return pontoEmPoligono(centro, regiao);

  default:
    return 0;
  }
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