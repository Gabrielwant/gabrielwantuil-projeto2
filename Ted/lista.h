#ifndef LISTA_H
#define LISTA_H

typedef struct No
{
  void *dado;
  struct No *prox;
} No;

typedef struct
{
  No *inicio;
  No *fim;
  int tamanho;
} Lista;

Lista *criarLista();
void inserirLista(Lista *lista, void *dado);
void *buscarLista(Lista *lista, int (*comparar)(void *, void *), void *chave);
void *removerLista(Lista *lista, int (*comparar)(void *, void *), void *chave);
void destruirLista(Lista *lista, void (*destruirDado)(void *));
int tamanhoLista(Lista *lista);
void imprimirLista(Lista *lista, void (*imprimirDado)(void *));

#endif