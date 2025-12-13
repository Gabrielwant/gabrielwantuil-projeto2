#ifndef SVG_H
#define SVG_H

#include "geometria.h"
#include "lista.h"

void gerarSVG(Lista *formas, const char *nomeArquivo);
void gerarSVGComVisibilidade(Lista *formas, Poligono *regiao, const char *nomeArquivo);
void desenharRegiaoVisibilidade(Poligono *regiao, const char *nomeArquivo);

#endif