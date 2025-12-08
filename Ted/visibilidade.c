#include "visibilidade.h"
#include "arvore.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

Poligono *calcularVisibilidade(Lista *formas, double px, double py)
{
  Poligono *pol = criarPoligono(100);

  // Implementação básica - retorna um quadrado ao redor do ponto
  adicionarVerticePoligono(pol, criarPonto(px - 50, py - 50));
  adicionarVerticePoligono(pol, criarPonto(px + 50, py - 50));
  adicionarVerticePoligono(pol, criarPonto(px + 50, py + 50));
  adicionarVerticePoligono(pol, criarPonto(px - 50, py + 50));

  return pol;
}

void transformarAnteparos(Lista *formas, int idMin, int idMax, char orientacao, FILE *txtOut)
{
  No *no = formas->inicio;

  while (no)
  {
    Forma *forma = (Forma *)no->dado;

    if (forma->ativo && forma->id >= idMin && forma->id <= idMax)
    {
      if (forma->tipo == LINHA)
      {
        forma->tipo = SEGMENTO;
        fprintf(txtOut, "  Forma %d transformada em anteparo (%c)\n", forma->id, orientacao);
      }
    }

    no = no->prox;
  }
}

int formaEmRegiao(Forma *forma, Poligono *regiao)
{
  if (!forma || !regiao)
    return 0;

  Ponto centro;

  switch (forma->tipo)
  {
  case CIRCULO:
    centro = forma->dados.circulo.centro;
    return pontoEmPoligono(centro, regiao);

  case RETANGULO:
    centro.x = forma->dados.retangulo.ancora.x + forma->dados.retangulo.largura / 2;
    centro.y = forma->dados.retangulo.ancora.y + forma->dados.retangulo.altura / 2;
    return pontoEmPoligono(centro, regiao);

  case LINHA:
  case SEGMENTO:
    centro.x = (forma->dados.linha.p1.x + forma->dados.linha.p2.x) / 2;
    centro.y = (forma->dados.linha.p1.y + forma->dados.linha.p2.y) / 2;
    return pontoEmPoligono(centro, regiao);

  case TEXTO:
    centro = forma->dados.texto.ancora;
    return pontoEmPoligono(centro, regiao);

  default:
    return 0;
  }
}

Forma *clonarForma(Forma *forma, int novoId, double dx, double dy)
{
  if (!forma)
    return NULL;

  Forma *clone = NULL;

  switch (forma->tipo)
  {
  case CIRCULO:
    clone = criarCirculo(
        novoId,
        forma->dados.circulo.centro.x + dx,
        forma->dados.circulo.centro.y + dy,
        forma->dados.circulo.raio,
        forma->corBorda,
        forma->corPreenchimento);
    break;

  case RETANGULO:
    clone = criarRetangulo(
        novoId,
        forma->dados.retangulo.ancora.x + dx,
        forma->dados.retangulo.ancora.y + dy,
        forma->dados.retangulo.largura,
        forma->dados.retangulo.altura,
        forma->corBorda,
        forma->corPreenchimento);
    break;

  case LINHA:
    clone = criarLinha(
        novoId,
        forma->dados.linha.p1.x + dx,
        forma->dados.linha.p1.y + dy,
        forma->dados.linha.p2.x + dx,
        forma->dados.linha.p2.y + dy,
        forma->corBorda);
    break;

  case SEGMENTO:
    clone = criarSegmento(
        novoId,
        forma->dados.linha.p1.x + dx,
        forma->dados.linha.p1.y + dy,
        forma->dados.linha.p2.x + dx,
        forma->dados.linha.p2.y + dy,
        forma->corBorda);
    break;

  case TEXTO:
    clone = criarTexto(
        novoId,
        forma->dados.texto.ancora.x + dx,
        forma->dados.texto.ancora.y + dy,
        forma->corBorda,
        forma->corPreenchimento,
        forma->dados.texto.posicaoAncora,
        forma->dados.texto.texto,
        &forma->dados.texto.estilo);
    break;
  }

  return clone;
}

int obterProximoId(Lista *formas)
{
  int maxId = 0;
  No *no = formas->inicio;

  while (no)
  {
    Forma *forma = (Forma *)no->dado;
    if (forma->id > maxId)
      maxId = forma->id;
    no = no->prox;
  }

  return maxId + 1;
}