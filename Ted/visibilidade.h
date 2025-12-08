#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "geometria.h"
#include "lista.h"

typedef struct
{
  Ponto ponto;
  int tipo;
  void *segmento;
  int codigo;
} Vertice;

Poligono *calcularVisibilidade(Lista *formas, double px, double py);
void transformarAnteparos(Lista *formas, int idMin, int idMax, char orientacao, FILE *txtOut);
int formaEmRegiao(Forma *forma, Poligono *regiao);
Forma *clonarForma(Forma *forma, int novoId, double dx, double dy);
int obterProximoId(Lista *formas);

#endif