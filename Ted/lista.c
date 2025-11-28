#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

Lista *criarLista()
{
    Lista *lista = (Lista *)malloc(sizeof(Lista));
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

void inserirLista(Lista *lista, void *dado)
{
    No *novo = (No *)malloc(sizeof(No));
    novo->dado = dado;
    novo->prox = NULL;

    if (lista->inicio == NULL)
    {
        lista->inicio = novo;
        lista->fim = novo;
    }
    else
    {
        lista->fim->prox = novo;
        lista->fim = novo;
    }

    lista->tamanho++;
}

void *buscarLista(Lista *lista, int (*comparar)(void *, void *), void *chave)
{
    No *atual = lista->inicio;

    while (atual != NULL)
    {
        if (comparar(atual->dado, chave) == 0)
        {
            return atual->dado;
        }
        atual = atual->prox;
    }

    return NULL;
}

void *removerLista(Lista *lista, int (*comparar)(void *, void *), void *chave)
{
    No *atual = lista->inicio;
    No *anterior = NULL;

    while (atual != NULL)
    {
        if (comparar(atual->dado, chave) == 0)
        {
            void *dado = atual->dado;

            if (anterior == NULL)
            {
                lista->inicio = atual->prox;
            }
            else
            {
                anterior->prox = atual->prox;
            }

            if (atual == lista->fim)
            {
                lista->fim = anterior;
            }

            free(atual);
            lista->tamanho--;
            return dado;
        }

        anterior = atual;
        atual = atual->prox;
    }

    return NULL;
}

void destruirLista(Lista *lista, void (*destruirDado)(void *))
{
    No *atual = lista->inicio;

    while (atual != NULL)
    {
        No *prox = atual->prox;
        if (destruirDado != NULL)
        {
            destruirDado(atual->dado);
        }
        free(atual);
        atual = prox;
    }

    free(lista);
}

int tamanhoLista(Lista *lista)
{
    return lista->tamanho;
}

void imprimirLista(Lista *lista, void (*imprimirDado)(void *))
{
    No *atual = lista->inicio;

    printf("Lista [%d elementos]: ", lista->tamanho);
    while (atual != NULL)
    {
        if (imprimirDado != NULL)
        {
            imprimirDado(atual->dado);
        }
        printf(" -> ");
        atual = atual->prox;
    }
    printf("NULL\n");
}