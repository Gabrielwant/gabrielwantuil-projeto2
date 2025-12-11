#ifndef LISTA_H
#define LISTA_H

// Nó genérico que armazena um ponteiro para dado e o próximo elemento
typedef struct No
{
  void *dado;
  struct No *prox; // Aponta para o próximo nó da lista encadeada
} No;

// Estrutura da lista, contendo ponteiros para início, fim e controle de tamanho
typedef struct
{
  No *inicio;
  No *fim;     // Facilita inserção rápida no final
  int tamanho; // Quantidade de elementos atualmente na lista
} Lista;

Lista *criarLista();
void inserirLista(Lista *lista, void *dado);
void *buscarLista(Lista *lista, int (*comparar)(void *, void *), void *chave);
void *removerLista(Lista *lista, int (*comparar)(void *, void *), void *chave);
void destruirLista(Lista *lista, void (*destruirDado)(void *));
int tamanhoLista(Lista *lista);
void imprimirLista(Lista *lista, void (*imprimirDado)(void *));

#endif
