#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

Lista *criarLista()
{
    Lista *lista = (Lista *)malloc(sizeof(Lista));
    // VERIFICAÇÃO CRÍTICA: malloc pode falhar
    if (!lista)
    {
        fprintf(stderr, "ERRO: criarLista - falha ao alocar memória\n");
        return NULL;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

void inserirLista(Lista *lista, void *dado)
{
    // VERIFICAÇÃO CRÍTICA: lista e dado não podem ser NULL
    if (!lista)
    {
        fprintf(stderr, "ERRO: inserirLista - lista é NULL\n");
        return;
    }

    if (!dado)
    {
        fprintf(stderr, "AVISO: inserirLista - tentando inserir dado NULL\n");
        return;
    }

    No *novo = (No *)malloc(sizeof(No));
    // VERIFICAÇÃO: malloc pode falhar
    if (!novo)
    {
        fprintf(stderr, "ERRO: inserirLista - falha ao alocar nó\n");
        return;
    }

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
    // VERIFICAÇÃO: parâmetros válidos
    if (!lista || !comparar)
    {
        return NULL;
    }

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
    // VERIFICAÇÃO: parâmetros válidos
    if (!lista || !comparar)
    {
        return NULL;
    }

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
    // VERIFICAÇÃO: lista válida
    if (!lista)
    {
        return;
    }

    No *atual = lista->inicio;

    while (atual != NULL)
    {
        No *prox = atual->prox;

        // Se houver função para destruir dado, usa ela
        if (destruirDado != NULL && atual->dado != NULL)
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
    // VERIFICAÇÃO: lista válida
    if (!lista)
    {
        return 0;
    }
    return lista->tamanho;
}

void imprimirLista(Lista *lista, void (*imprimirDado)(void *))
{
    // VERIFICAÇÃO: lista válida
    if (!lista)
    {
        printf("Lista: NULL\n");
        return;
    }

    No *atual = lista->inicio;

    printf("Lista [%d elementos]: ", lista->tamanho);
    while (atual != NULL)
    {
        if (imprimirDado != NULL && atual->dado != NULL)
        {
            imprimirDado(atual->dado);
        }
        else
        {
            printf("(NULL)");
        }
        printf(" -> ");
        atual = atual->prox;
    }
    printf("NULL\n");
}