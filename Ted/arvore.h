#ifndef ARVORE_H
#define ARVORE_H

#include "geometria.h"

typedef struct NoArvore
{
  void *dado;
  struct NoArvore *esq;
  struct NoArvore *dir;
  int altura;
} NoArvore;

typedef struct
{
  NoArvore *raiz;
  Ponto pontoReferencia;
  int (*comparar)(void *, void *, Ponto);
} ArvoreAVL;

ArvoreAVL *criarArvoreAVL(int (*comparar)(void *, void *, Ponto), Ponto pontoRef);
void inserirArvore(ArvoreAVL *arvore, void *dado);
void *buscarArvore(ArvoreAVL *arvore, void *chave);
void *removerArvore(ArvoreAVL *arvore, void *chave);
void destruirArvoreAVL(ArvoreAVL *arvore, void (*destruirDado)(void *));
void *obterMaisProximo(ArvoreAVL *arvore, Ponto vertice);

typedef struct
{
  Ponto ini;
  Ponto fim;
  char cor[64];
  int idOriginal;
} Segmento;

Segmento *criarSegmentoArvore(Ponto ini, Ponto fim, const char *cor, int idOriginal);
int compararSegmentos(void *s1, void *s2, Ponto pontoRef);

#endif