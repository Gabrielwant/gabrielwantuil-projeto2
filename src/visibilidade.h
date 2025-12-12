#ifndef VISIBILIDADE_H
#define VISIBILIDADE_H

#include "geometria.h"
#include "lista.h"

// Estrutura usada para representar um vértice relevante no cálculo de visibilidade
typedef struct
{
  Ponto ponto;    // Coordenadas do vértice
  int tipo;       // Indica se é início, fim ou ponto especial no segmento
  void *segmento; // Referência ao segmento ao qual o vértice pertence
  int codigo;     // Código auxiliar para ordenação ou classificação
} Vertice;

// Gera o polígono de visibilidade a partir das formas e do ponto observador
Poligono *calcularVisibilidade(Lista *formas, double px, double py);

// Converte formas em anteparos para uso no algoritmo de visibilidade
void transformarAnteparos(Lista *formas, int idMin, int idMax, char orientacao, FILE *txtOut);

int formaEmRegiao(Forma *forma, Poligono *regiao);

// Cria uma cópia da forma deslocada e com novo ID
Forma *clonarForma(Forma *forma, int novoId, double dx, double dy);

int obterProximoId(Lista *formas);

#endif
