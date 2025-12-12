#ifndef ARVORE_H
#define ARVORE_H

#include "geometria.h"

// Nó básico da árvore AVL, armazenando dado genérico e altura
typedef struct NoArvore
{
  void *dado;
  struct NoArvore *esq; // Ponteiro para subárvore esquerda
  struct NoArvore *dir; // Ponteiro para subárvore direita
  int altura;
} NoArvore;

// Estrutura principal da árvore AVL, com função de comparação customizada
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

// Segmento utilizado na árvore, contendo extremidades, cor e id original
typedef struct
{
  Ponto ini;
  Ponto fim;
  char cor[64]; // Armazena a cor do segmento (até 63 chars + '\0')
  int idOriginal;
} Segmento;

Segmento *criarSegmentoArvore(Ponto ini, Ponto fim, const char *cor, int idOriginal);
int compararSegmentos(void *s1, void *s2, Ponto pontoRef);

#endif
