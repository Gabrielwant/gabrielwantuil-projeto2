#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct
{
  int id;
  char nome[50];
} Dados;

int compararDados(void *d1, void *d2)
{
  Dados *dado1 = (Dados *)d1;
  Dados *dado2 = (Dados *)d2;
  return dado1->id - dado2->id;
}

void imprimirDados(void *d)
{
  Dados *dado = (Dados *)d;
  printf("[%d: %s]", dado->id, dado->nome);
}

void destruirDados(void *d)
{
  free(d);
}

void testarCriacaoLista()
{
  printf("Teste 1: Criação de lista... ");
  Lista *lista = criarLista();
  assert(lista != NULL);
  assert(lista->inicio == NULL);
  assert(lista->fim == NULL);
  assert(lista->tamanho == 0);
  destruirLista(lista, NULL);
  printf("OK\n");
}

void testarInsercao()
{
  printf("Teste 2: Inserção de elementos... ");
  Lista *lista = criarLista();

  Dados *d1 = (Dados *)malloc(sizeof(Dados));
  d1->id = 1;
  sprintf(d1->nome, "Primeiro");
  inserirLista(lista, d1);

  assert(lista->tamanho == 1);
  assert(lista->inicio == lista->fim);

  Dados *d2 = (Dados *)malloc(sizeof(Dados));
  d2->id = 2;
  sprintf(d2->nome, "Segundo");
  inserirLista(lista, d2);

  assert(lista->tamanho == 2);
  assert(lista->inicio != lista->fim);

  Dados *d3 = (Dados *)malloc(sizeof(Dados));
  d3->id = 3;
  sprintf(d3->nome, "Terceiro");
  inserirLista(lista, d3);

  assert(lista->tamanho == 3);

  destruirLista(lista, destruirDados);
  printf("OK\n");
}

void testarBusca()
{
  printf("Teste 3: Busca de elementos... ");
  Lista *lista = criarLista();

  for (int i = 1; i <= 5; i++)
  {
    Dados *d = (Dados *)malloc(sizeof(Dados));
    d->id = i;
    sprintf(d->nome, "Item %d", i);
    inserirLista(lista, d);
  }

  Dados chave;
  chave.id = 3;
  Dados *encontrado = (Dados *)buscarLista(lista, compararDados, &chave);

  assert(encontrado != NULL);
  assert(encontrado->id == 3);

  chave.id = 10;
  encontrado = (Dados *)buscarLista(lista, compararDados, &chave);
  assert(encontrado == NULL);

  destruirLista(lista, destruirDados);
  printf("OK\n");
}

void testarRemocao()
{
  printf("Teste 4: Remoção de elementos... ");
  Lista *lista = criarLista();

  for (int i = 1; i <= 5; i++)
  {
    Dados *d = (Dados *)malloc(sizeof(Dados));
    d->id = i;
    sprintf(d->nome, "Item %d", i);
    inserirLista(lista, d);
  }

  assert(lista->tamanho == 5);

  Dados chave;
  chave.id = 3;
  Dados *removido = (Dados *)removerLista(lista, compararDados, &chave);

  assert(removido != NULL);
  assert(removido->id == 3);
  assert(lista->tamanho == 4);
  free(removido);

  chave.id = 1;
  removido = (Dados *)removerLista(lista, compararDados, &chave);
  assert(removido != NULL);
  assert(lista->tamanho == 3);
  free(removido);

  chave.id = 5;
  removido = (Dados *)removerLista(lista, compararDados, &chave);
  assert(removido != NULL);
  assert(lista->tamanho == 2);
  free(removido);

  destruirLista(lista, destruirDados);
  printf("OK\n");
}

void testarTamanho()
{
  printf("Teste 5: Verificação de tamanho... ");
  Lista *lista = criarLista();

  assert(tamanhoLista(lista) == 0);

  for (int i = 1; i <= 10; i++)
  {
    Dados *d = (Dados *)malloc(sizeof(Dados));
    d->id = i;
    sprintf(d->nome, "Item %d", i);
    inserirLista(lista, d);
    assert(tamanhoLista(lista) == i);
  }

  destruirLista(lista, destruirDados);
  printf("OK\n");
}

void testarImpressao()
{
  printf("Teste 6: Impressão da lista... ");
  Lista *lista = criarLista();

  for (int i = 1; i <= 5; i++)
  {
    Dados *d = (Dados *)malloc(sizeof(Dados));
    d->id = i;
    sprintf(d->nome, "Item %d", i);
    inserirLista(lista, d);
  }

  printf("\n");
  imprimirLista(lista, imprimirDados);

  destruirLista(lista, destruirDados);
  printf("OK\n");
}

int main()
{
  printf("=== TESTES UNITÁRIOS - LISTA ENCADEADA ===\n\n");

  testarCriacaoLista();
  testarInsercao();
  testarBusca();
  testarRemocao();
  testarTamanho();
  testarImpressao();

  printf("\n=== TODOS OS TESTES PASSARAM ===\n");
  return 0;
}