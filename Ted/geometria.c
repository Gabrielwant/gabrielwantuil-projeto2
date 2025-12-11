#include "geometria.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

Forma *criarCirculo(int id, double x, double y, double r, const char *corb, const char *corp)
{
  Forma *forma = (Forma *)malloc(sizeof(Forma));
  if (!forma)
  {
    fprintf(stderr, "ERRO: criarCirculo - falha ao alocar memória\n");
    return NULL;
  }
  forma->id = id;
  forma->tipo = CIRCULO;
  forma->ativo = 1;
  strcpy(forma->corBorda, corb);
  strcpy(forma->corPreenchimento, corp);
  forma->dados.circulo.centro.x = x;
  forma->dados.circulo.centro.y = y;
  forma->dados.circulo.raio = r;
  return forma;
}

Forma *criarRetangulo(int id, double x, double y, double w, double h, const char *corb, const char *corp)
{
  Forma *forma = (Forma *)malloc(sizeof(Forma));
  if (!forma)
  {
    fprintf(stderr, "ERRO: criarRetangulo - falha ao alocar memória\n");
    return NULL;
  }
  forma->id = id;
  forma->tipo = RETANGULO;
  forma->ativo = 1;
  strcpy(forma->corBorda, corb);
  strcpy(forma->corPreenchimento, corp);
  forma->dados.retangulo.ancora.x = x;
  forma->dados.retangulo.ancora.y = y;
  forma->dados.retangulo.largura = w;
  forma->dados.retangulo.altura = h;
  return forma;
}

Forma *criarLinha(int id, double x1, double y1, double x2, double y2, const char *cor)
{
  Forma *forma = (Forma *)malloc(sizeof(Forma));
  if (!forma)
  {
    fprintf(stderr, "ERRO: criarLinha - falha ao alocar memória\n");
    return NULL;
  }
  forma->id = id;
  forma->tipo = LINHA;
  forma->ativo = 1;
  strcpy(forma->corBorda, cor);
  strcpy(forma->corPreenchimento, "none");
  forma->dados.linha.p1.x = x1;
  forma->dados.linha.p1.y = y1;
  forma->dados.linha.p2.x = x2;
  forma->dados.linha.p2.y = y2;
  return forma;
}

Forma *criarTexto(int id, double x, double y, const char *corb, const char *corp,
                  char ancora, const char *txt, EstiloTexto *estilo)
{
  Forma *forma = (Forma *)malloc(sizeof(Forma));
  if (!forma)
  {
    fprintf(stderr, "ERRO: criarTexto - falha ao alocar memória\n");
    return NULL;
  }
  forma->id = id;
  forma->tipo = TEXTO;
  forma->ativo = 1;
  strcpy(forma->corBorda, corb);
  strcpy(forma->corPreenchimento, corp);
  forma->dados.texto.ancora.x = x;
  forma->dados.texto.ancora.y = y;
  strcpy(forma->dados.texto.texto, txt);
  forma->dados.texto.posicaoAncora = ancora;
  forma->dados.texto.estilo = *estilo;
  return forma;
}

Forma *criarSegmento(int id, double x1, double y1, double x2, double y2, const char *cor)
{
  Forma *forma = (Forma *)malloc(sizeof(Forma));
  if (!forma)
  {
    fprintf(stderr, "ERRO: criarSegmento - falha ao alocar memória\n");
    return NULL;
  }
  forma->id = id;
  forma->tipo = SEGMENTO;
  forma->ativo = 1;
  strcpy(forma->corBorda, cor);
  strcpy(forma->corPreenchimento, "none");
  forma->dados.linha.p1.x = x1;
  forma->dados.linha.p1.y = y1;
  forma->dados.linha.p2.x = x2;
  forma->dados.linha.p2.y = y2;
  return forma;
}

void destruirForma(void *forma)
{
  if (forma)
  {
    free(forma);
  }
}

const char *obterTipoForma(TipoForma tipo)
{
  switch (tipo)
  {
  case CIRCULO:
    return "Círculo";
  case RETANGULO:
    return "Retângulo";
  case LINHA:
    return "Linha";
  case TEXTO:
    return "Texto";
  case SEGMENTO:
    return "Segmento";
  default:
    return "Desconhecido";
  }
}

Ponto criarPonto(double x, double y)
{
  Ponto p = {x, y};
  return p;
}

double distanciaPontos(Ponto p1, Ponto p2)
{
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

double produtoVetorial(Ponto p1, Ponto p2, Ponto p3)
{
  return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

int viradaEsquerda(Ponto p1, Ponto p2, Ponto p3)
{
  return produtoVetorial(p1, p2, p3) > 0;
}

int viradaDireita(Ponto p1, Ponto p2, Ponto p3)
{
  return produtoVetorial(p1, p2, p3) < 0;
}

int colineares(Ponto p1, Ponto p2, Ponto p3)
{
  return fabs(produtoVetorial(p1, p2, p3)) < 1e-10;
}

int pontoNoSegmento(Ponto p, Ponto s1, Ponto s2)
{
  if (!colineares(s1, s2, p))
    return 0;

  double minX = fmin(s1.x, s2.x);
  double maxX = fmax(s1.x, s2.x);
  double minY = fmin(s1.y, s2.y);
  double maxY = fmax(s1.y, s2.y);

  return p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
}

int segmentosIntersectam(Ponto p1, Ponto p2, Ponto p3, Ponto p4)
{
  double d1 = produtoVetorial(p3, p4, p1);
  double d2 = produtoVetorial(p3, p4, p2);
  double d3 = produtoVetorial(p1, p2, p3);
  double d4 = produtoVetorial(p1, p2, p4);

  if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
      ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
  {
    return 1;
  }

  if (fabs(d1) < 1e-10 && pontoNoSegmento(p1, p3, p4))
    return 1;
  if (fabs(d2) < 1e-10 && pontoNoSegmento(p2, p3, p4))
    return 1;
  if (fabs(d3) < 1e-10 && pontoNoSegmento(p3, p1, p2))
    return 1;
  if (fabs(d4) < 1e-10 && pontoNoSegmento(p4, p1, p2))
    return 1;

  return 0;
}

Ponto intersecaoSegmentos(Ponto p1, Ponto p2, Ponto p3, Ponto p4)
{
  double x1 = p1.x, y1 = p1.y;
  double x2 = p2.x, y2 = p2.y;
  double x3 = p3.x, y3 = p3.y;
  double x4 = p4.x, y4 = p4.y;

  double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (fabs(denom) < 1e-10)
  {
    return criarPonto((x1 + x2) / 2, (y1 + y2) / 2);
  }

  double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;

  return criarPonto(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
}

Poligono *criarPoligono(int capacidadeInicial)
{
  Poligono *pol = (Poligono *)malloc(sizeof(Poligono));
  if (!pol)
  {
    fprintf(stderr, "ERRO: criarPoligono - falha ao alocar polígono\n");
    return NULL;
  }

  pol->vertices = (Ponto *)malloc(capacidadeInicial * sizeof(Ponto));
  if (!pol->vertices)
  {
    fprintf(stderr, "ERRO: criarPoligono - falha ao alocar vértices\n");
    free(pol);
    return NULL;
  }

  pol->numVertices = 0;
  pol->capacidade = capacidadeInicial;
  return pol;
}

void adicionarVerticePoligono(Poligono *pol, Ponto p)
{
  if (!pol)
  {
    fprintf(stderr, "ERRO: adicionarVerticePoligono - polígono é NULL\n");
    return;
  }

  if (pol->numVertices >= pol->capacidade)
  {
    pol->capacidade *= 2;
    Ponto *novosVertices = (Ponto *)realloc(pol->vertices, pol->capacidade * sizeof(Ponto));
    if (!novosVertices)
    {
      fprintf(stderr, "ERRO: adicionarVerticePoligono - falha ao realocar vértices\n");
      return;
    }
    pol->vertices = novosVertices;
  }
  pol->vertices[pol->numVertices++] = p;
}

void destruirPoligono(Poligono *pol)
{
  if (pol)
  {
    if (pol->vertices)
    {
      free(pol->vertices);
    }
    free(pol);
  }
}

int pontoEmPoligono(Ponto p, Poligono *pol)
{
  if (!pol || !pol->vertices || pol->numVertices < 3)
  {
    return 0;
  }

  int dentro = 0;
  for (int i = 0, j = pol->numVertices - 1; i < pol->numVertices; j = i++)
  {
    Ponto vi = pol->vertices[i];
    Ponto vj = pol->vertices[j];

    if ((vi.y > p.y) != (vj.y > p.y) &&
        p.x < (vj.x - vi.x) * (p.y - vi.y) / (vj.y - vi.y) + vi.x)
    {
      dentro = !dentro;
    }
  }
  return dentro;
}