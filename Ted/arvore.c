#include "arvore.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int altura(NoArvore *no)
{
  return no ? no->altura : 0;
}

static int fatorBalanceamento(NoArvore *no)
{
  return no ? altura(no->esq) - altura(no->dir) : 0;
}

static void atualizarAltura(NoArvore *no)
{
  int altEsq = altura(no->esq);
  int altDir = altura(no->dir);
  no->altura = 1 + (altEsq > altDir ? altEsq : altDir);
}

static NoArvore *rotacaoDireita(NoArvore *y)
{
  NoArvore *x = y->esq;
  NoArvore *T2 = x->dir;

  x->dir = y;
  y->esq = T2;

  atualizarAltura(y);
  atualizarAltura(x);

  return x;
}

static NoArvore *rotacaoEsquerda(NoArvore *x)
{
  NoArvore *y = x->dir;
  NoArvore *T2 = y->esq;

  y->esq = x;
  x->dir = T2;

  atualizarAltura(x);
  atualizarAltura(y);

  return y;
}

static NoArvore *balancear(NoArvore *no)
{
  atualizarAltura(no);
  int fb = fatorBalanceamento(no);

  if (fb > 1)
  {
    if (fatorBalanceamento(no->esq) < 0)
    {
      no->esq = rotacaoEsquerda(no->esq);
    }
    return rotacaoDireita(no);
  }

  if (fb < -1)
  {
    if (fatorBalanceamento(no->dir) > 0)
    {
      no->dir = rotacaoDireita(no->dir);
    }
    return rotacaoEsquerda(no);
  }

  return no;
}

static NoArvore *inserirNoArvore(NoArvore *no, void *dado,
                                 int (*comparar)(void *, void *, Ponto),
                                 Ponto pontoRef)
{
  if (!no)
  {
    NoArvore *novo = (NoArvore *)malloc(sizeof(NoArvore));
    novo->dado = dado;
    novo->esq = novo->dir = NULL;
    novo->altura = 1;
    return novo;
  }

  int cmp = comparar(dado, no->dado, pontoRef);

  if (cmp < 0)
  {
    no->esq = inserirNoArvore(no->esq, dado, comparar, pontoRef);
  }
  else if (cmp > 0)
  {
    no->dir = inserirNoArvore(no->dir, dado, comparar, pontoRef);
  }
  else
  {
    return no;
  }

  return balancear(no);
}

static NoArvore *obterMinimo(NoArvore *no)
{
  while (no->esq)
  {
    no = no->esq;
  }
  return no;
}

static NoArvore *removerNoArvore(NoArvore *no, void *chave,
                                 int (*comparar)(void *, void *, Ponto),
                                 Ponto pontoRef)
{
  if (!no)
    return NULL;

  int cmp = comparar(chave, no->dado, pontoRef);

  if (cmp < 0)
  {
    no->esq = removerNoArvore(no->esq, chave, comparar, pontoRef);
  }
  else if (cmp > 0)
  {
    no->dir = removerNoArvore(no->dir, chave, comparar, pontoRef);
  }
  else
  {
    if (!no->esq || !no->dir)
    {
      NoArvore *temp = no->esq ? no->esq : no->dir;
      free(no);
      return temp;
    }

    NoArvore *temp = obterMinimo(no->dir);
    no->dado = temp->dado;
    no->dir = removerNoArvore(no->dir, temp->dado, comparar, pontoRef);
  }

  return balancear(no);
}

static void *buscarNoArvore(NoArvore *no, void *chave,
                            int (*comparar)(void *, void *, Ponto),
                            Ponto pontoRef)
{
  if (!no)
    return NULL;

  int cmp = comparar(chave, no->dado, pontoRef);

  if (cmp < 0)
  {
    return buscarNoArvore(no->esq, chave, comparar, pontoRef);
  }
  else if (cmp > 0)
  {
    return buscarNoArvore(no->dir, chave, comparar, pontoRef);
  }
  else
  {
    return no->dado;
  }
}

static void destruirNoArvore(NoArvore *no, void (*destruirDado)(void *))
{
  if (!no)
    return;

  destruirNoArvore(no->esq, destruirDado);
  destruirNoArvore(no->dir, destruirDado);

  if (destruirDado)
  {
    destruirDado(no->dado);
  }

  free(no);
}

ArvoreAVL *criarArvoreAVL(int (*comparar)(void *, void *, Ponto), Ponto pontoRef)
{
  ArvoreAVL *arvore = (ArvoreAVL *)malloc(sizeof(ArvoreAVL));
  arvore->raiz = NULL;
  arvore->comparar = comparar;
  arvore->pontoReferencia = pontoRef;
  return arvore;
}

void inserirArvore(ArvoreAVL *arvore, void *dado)
{
  arvore->raiz = inserirNoArvore(arvore->raiz, dado, arvore->comparar, arvore->pontoReferencia);
}

void *buscarArvore(ArvoreAVL *arvore, void *chave)
{
  return buscarNoArvore(arvore->raiz, chave, arvore->comparar, arvore->pontoReferencia);
}

void *removerArvore(ArvoreAVL *arvore, void *chave)
{
  void *dado = buscarArvore(arvore, chave);
  if (dado)
  {
    arvore->raiz = removerNoArvore(arvore->raiz, chave, arvore->comparar, arvore->pontoReferencia);
  }
  return dado;
}

void destruirArvoreAVL(ArvoreAVL *arvore, void (*destruirDado)(void *))
{
  destruirNoArvore(arvore->raiz, destruirDado);
  free(arvore);
}

Segmento *criarSegmentoArvore(Ponto ini, Ponto fim, const char *cor, int idOriginal)
{
  Segmento *seg = (Segmento *)malloc(sizeof(Segmento));
  seg->ini = ini;
  seg->fim = fim;
  strcpy(seg->cor, cor);
  seg->idOriginal = idOriginal;
  return seg;
}

int compararSegmentos(void *s1, void *s2, Ponto pontoRef)
{
  Segmento *seg1 = (Segmento *)s1;
  Segmento *seg2 = (Segmento *)s2;

  double prod = produtoVetorial(pontoRef, seg1->ini, seg2->ini);

  if (fabs(prod) > 1e-10)
  {
    return prod < 0 ? -1 : 1;
  }

  double dist1 = distanciaPontos(pontoRef, seg1->ini);
  double dist2 = distanciaPontos(pontoRef, seg2->ini);

  if (fabs(dist1 - dist2) > 1e-10)
  {
    return dist1 < dist2 ? -1 : 1;
  }

  return 0;
}

void *obterMaisProximo(ArvoreAVL *arvore, Ponto vertice)
{
  if (!arvore->raiz)
    return NULL;

  NoArvore *atual = arvore->raiz;
  void *maisProximo = NULL;
  double menorDist = INFINITY;

  while (atual)
  {
    Segmento *seg = (Segmento *)atual->dado;
    double dist = distanciaPontos(arvore->pontoReferencia, seg->ini);

    if (dist < menorDist)
    {
      menorDist = dist;
      maisProximo = atual->dado;
    }

    int cmp = compararSegmentos(&vertice, atual->dado, arvore->pontoReferencia);

    if (cmp < 0)
    {
      atual = atual->esq;
    }
    else if (cmp > 0)
    {
      atual = atual->dir;
    }
    else
    {
      break;
    }
  }

  return maisProximo;
}