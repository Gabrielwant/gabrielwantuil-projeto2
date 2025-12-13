#include "svg.h"
#include <stdio.h>
#include <string.h>

static void calcularDimensoes(Lista *formas, double *minX, double *minY,
                              double *maxX, double *maxY)
{
  *minX = *minY = 999999.0;
  *maxX = *maxY = -999999.0;

  No *no = formas->inicio;
  while (no)
  {
    Forma *forma = (Forma *)no->dado;

    if (!forma || !forma->ativo)
    {
      no = no->prox;
      continue;
    }

    if (forma->tipo == CIRCULO)
    {
      double x = forma->dados.circulo.centro.x;
      double y = forma->dados.circulo.centro.y;
      double r = forma->dados.circulo.raio;

      if (x - r < *minX)
        *minX = x - r;
      if (x + r > *maxX)
        *maxX = x + r;
      if (y - r < *minY)
        *minY = y - r;
      if (y + r > *maxY)
        *maxY = y + r;
    }
    else if (forma->tipo == RETANGULO)
    {
      double x = forma->dados.retangulo.ancora.x;
      double y = forma->dados.retangulo.ancora.y;
      double w = forma->dados.retangulo.largura;
      double h = forma->dados.retangulo.altura;

      if (x < *minX)
        *minX = x;
      if (x + w > *maxX)
        *maxX = x + w;
      if (y < *minY)
        *minY = y;
      if (y + h > *maxY)
        *maxY = y + h;
    }
    else if (forma->tipo == LINHA || forma->tipo == SEGMENTO)
    {
      double x1 = forma->dados.linha.p1.x;
      double y1 = forma->dados.linha.p1.y;
      double x2 = forma->dados.linha.p2.x;
      double y2 = forma->dados.linha.p2.y;

      if (x1 < *minX)
        *minX = x1;
      if (x2 < *minX)
        *minX = x2;
      if (x1 > *maxX)
        *maxX = x1;
      if (x2 > *maxX)
        *maxX = x2;
      if (y1 < *minY)
        *minY = y1;
      if (y2 < *minY)
        *minY = y2;
      if (y1 > *maxY)
        *maxY = y1;
      if (y2 > *maxY)
        *maxY = y2;
    }
    else if (forma->tipo == TEXTO)
    {
      double x = forma->dados.texto.ancora.x;
      double y = forma->dados.texto.ancora.y;

      if (x < *minX)
        *minX = x;
      if (x > *maxX)
        *maxX = x;
      if (y < *minY)
        *minY = y;
      if (y > *maxY)
        *maxY = y;
    }

    no = no->prox;
  }

  if (*minX > *maxX)
  {
    *minX = 0;
    *maxX = 800;
    *minY = 0;
    *maxY = 600;
  }
}

void gerarSVG(Lista *formas, const char *nomeArquivo)
{
  FILE *arquivo = fopen(nomeArquivo, "w");
  if (!arquivo)
  {
    fprintf(stderr, "Erro ao criar arquivo SVG: %s\n", nomeArquivo);
    return;
  }

  double minX, minY, maxX, maxY;
  calcularDimensoes(formas, &minX, &minY, &maxX, &maxY);

  double margem = 50.0;
  double largura = maxX - minX + 2 * margem;
  double altura = maxY - minY + 2 * margem;

  fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(arquivo, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
  fprintf(arquivo, "width=\"%.2f\" height=\"%.2f\" ", largura, altura);
  fprintf(arquivo, "viewBox=\"%.2f %.2f %.2f %.2f\">\n",
          minX - margem, minY - margem, largura, altura);

  fprintf(arquivo, "  <!-- Fundo -->\n");
  fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
          minX - margem, minY - margem, largura, altura);
  fprintf(arquivo, "fill=\"#f8f9fa\" stroke=\"none\"/>\n\n");

  fprintf(arquivo, "  <defs>\n");
  fprintf(arquivo, "    <filter id=\"sombra\" x=\"-50%%\" y=\"-50%%\" width=\"200%%\" height=\"200%%\">\n");
  fprintf(arquivo, "      <feGaussianBlur in=\"SourceAlpha\" stdDeviation=\"3\"/>\n");
  fprintf(arquivo, "      <feOffset dx=\"2\" dy=\"2\" result=\"offsetblur\"/>\n");
  fprintf(arquivo, "      <feComponentTransfer>\n");
  fprintf(arquivo, "        <feFuncA type=\"linear\" slope=\"0.3\"/>\n");
  fprintf(arquivo, "      </feComponentTransfer>\n");
  fprintf(arquivo, "      <feMerge>\n");
  fprintf(arquivo, "        <feMergeNode/>\n");
  fprintf(arquivo, "        <feMergeNode in=\"SourceGraphic\"/>\n");
  fprintf(arquivo, "      </feMerge>\n");
  fprintf(arquivo, "    </filter>\n");
  fprintf(arquivo, "  </defs>\n\n");

  fprintf(arquivo, "  <!-- Formas geométricas -->\n");
  No *no = formas->inicio;
  while (no)
  {
    Forma *forma = (Forma *)no->dado;

    if (!forma || !forma->ativo)
    {
      no = no->prox;
      continue;
    }

    if (forma->tipo == CIRCULO)
    {
      fprintf(arquivo, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" ",
              forma->dados.circulo.centro.x,
              forma->dados.circulo.centro.y,
              forma->dados.circulo.raio);
      fprintf(arquivo, "stroke=\"%s\" fill=\"%s\" stroke-width=\"2\" ",
              forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\"/>\n");
    }
    else if (forma->tipo == RETANGULO)
    {
      fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
              forma->dados.retangulo.ancora.x,
              forma->dados.retangulo.ancora.y,
              forma->dados.retangulo.largura,
              forma->dados.retangulo.altura);
      fprintf(arquivo, "stroke=\"%s\" fill=\"%s\" stroke-width=\"2\" ",
              forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\"/>\n");
    }
    else if (forma->tipo == LINHA || forma->tipo == SEGMENTO)
    {
      fprintf(arquivo, "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ",
              forma->dados.linha.p1.x,
              forma->dados.linha.p1.y,
              forma->dados.linha.p2.x,
              forma->dados.linha.p2.y);
      fprintf(arquivo, "stroke=\"%s\" stroke-width=\"%s\" ",
              forma->corBorda,
              forma->tipo == SEGMENTO ? "4" : "2");

      if (forma->tipo == SEGMENTO)
      {
        fprintf(arquivo, "stroke-linecap=\"round\" opacity=\"0.8\"/>\n");
      }
      else
      {
        fprintf(arquivo, "/>\n");
      }
    }
    else if (forma->tipo == TEXTO)
    {
      const char *anchor = "start";
      if (forma->dados.texto.posicaoAncora == 'm')
        anchor = "middle";
      else if (forma->dados.texto.posicaoAncora == 'f')
        anchor = "end";

      const char *family = "sans-serif";
      if (strcmp(forma->dados.texto.estilo.family, "serif") == 0)
        family = "serif";
      else if (strcmp(forma->dados.texto.estilo.family, "cursive") == 0)
        family = "cursive";

      const char *weight = "normal";
      if (forma->dados.texto.estilo.weight[0] == 'b')
        weight = "bold";
      else if (strcmp(forma->dados.texto.estilo.weight, "b+") == 0)
        weight = "bolder";
      else if (forma->dados.texto.estilo.weight[0] == 'l')
        weight = "lighter";

      fprintf(arquivo, "  <text x=\"%.2f\" y=\"%.2f\" ",
              forma->dados.texto.ancora.x,
              forma->dados.texto.ancora.y);
      fprintf(arquivo, "font-family=\"%s\" font-size=\"%d\" font-weight=\"%s\" ",
              family, forma->dados.texto.estilo.size, weight);
      fprintf(arquivo, "text-anchor=\"%s\" stroke=\"%s\" fill=\"%s\" ",
              anchor, forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\">%s</text>\n",
              forma->dados.texto.texto);
    }

    no = no->prox;
  }

  fprintf(arquivo, "</svg>\n");
  fclose(arquivo);
}

// NOVA FUNÇÃO: Desenha polígono de visibilidade no mesmo SVG das formas
void gerarSVGComVisibilidade(Lista *formas, Poligono *regiao, const char *nomeArquivo)
{
  FILE *arquivo = fopen(nomeArquivo, "w");
  if (!arquivo)
  {
    fprintf(stderr, "Erro ao criar arquivo SVG: %s\n", nomeArquivo);
    return;
  }

  double minX, minY, maxX, maxY;
  calcularDimensoes(formas, &minX, &minY, &maxX, &maxY);

  // Expandir para incluir polígono de visibilidade
  if (regiao)
  {
    for (int i = 0; i < regiao->numVertices; i++)
    {
      if (regiao->vertices[i].x < minX)
        minX = regiao->vertices[i].x;
      if (regiao->vertices[i].x > maxX)
        maxX = regiao->vertices[i].x;
      if (regiao->vertices[i].y < minY)
        minY = regiao->vertices[i].y;
      if (regiao->vertices[i].y > maxY)
        maxY = regiao->vertices[i].y;
    }
  }

  double margem = 50.0;
  double largura = maxX - minX + 2 * margem;
  double altura = maxY - minY + 2 * margem;

  fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(arquivo, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
  fprintf(arquivo, "width=\"%.2f\" height=\"%.2f\" ", largura, altura);
  fprintf(arquivo, "viewBox=\"%.2f %.2f %.2f %.2f\">\n",
          minX - margem, minY - margem, largura, altura);

  fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
          minX - margem, minY - margem, largura, altura);
  fprintf(arquivo, "fill=\"#f8f9fa\" stroke=\"none\"/>\n\n");

  fprintf(arquivo, "  <defs>\n");
  fprintf(arquivo, "    <filter id=\"sombra\" x=\"-50%%\" y=\"-50%%\" width=\"200%%\" height=\"200%%\">\n");
  fprintf(arquivo, "      <feGaussianBlur in=\"SourceAlpha\" stdDeviation=\"3\"/>\n");
  fprintf(arquivo, "      <feOffset dx=\"2\" dy=\"2\" result=\"offsetblur\"/>\n");
  fprintf(arquivo, "      <feComponentTransfer>\n");
  fprintf(arquivo, "        <feFuncA type=\"linear\" slope=\"0.3\"/>\n");
  fprintf(arquivo, "      </feComponentTransfer>\n");
  fprintf(arquivo, "      <feMerge>\n");
  fprintf(arquivo, "        <feMergeNode/>\n");
  fprintf(arquivo, "        <feMergeNode in=\"SourceGraphic\"/>\n");
  fprintf(arquivo, "      </feMerge>\n");
  fprintf(arquivo, "    </filter>\n");
  fprintf(arquivo, "  </defs>\n\n");

  // DESENHA O POLÍGONO DE VISIBILIDADE PRIMEIRO (fica atrás)
  if (regiao && regiao->numVertices >= 3)
  {
    fprintf(arquivo, "  <!-- Região de visibilidade -->\n");
    fprintf(arquivo, "  <polygon points=\"");
    for (int i = 0; i < regiao->numVertices; i++)
    {
      fprintf(arquivo, "%.2f,%.2f ", regiao->vertices[i].x, regiao->vertices[i].y);
    }
    fprintf(arquivo, "\" fill=\"#ffeb3b\" fill-opacity=\"0.3\" ");
    fprintf(arquivo, "stroke=\"#ff9800\" stroke-width=\"2\" stroke-opacity=\"0.6\"/>\n\n");
  }

  // DEPOIS DESENHA AS FORMAS (ficam na frente)
  fprintf(arquivo, "  <!-- Formas geométricas -->\n");
  No *no = formas->inicio;
  while (no)
  {
    Forma *forma = (Forma *)no->dado;

    if (!forma || !forma->ativo)
    {
      no = no->prox;
      continue;
    }

    if (forma->tipo == CIRCULO)
    {
      fprintf(arquivo, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" ",
              forma->dados.circulo.centro.x,
              forma->dados.circulo.centro.y,
              forma->dados.circulo.raio);
      fprintf(arquivo, "stroke=\"%s\" fill=\"%s\" stroke-width=\"2\" ",
              forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\"/>\n");
    }
    else if (forma->tipo == RETANGULO)
    {
      fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
              forma->dados.retangulo.ancora.x,
              forma->dados.retangulo.ancora.y,
              forma->dados.retangulo.largura,
              forma->dados.retangulo.altura);
      fprintf(arquivo, "stroke=\"%s\" fill=\"%s\" stroke-width=\"2\" ",
              forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\"/>\n");
    }
    else if (forma->tipo == LINHA || forma->tipo == SEGMENTO)
    {
      fprintf(arquivo, "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ",
              forma->dados.linha.p1.x,
              forma->dados.linha.p1.y,
              forma->dados.linha.p2.x,
              forma->dados.linha.p2.y);
      fprintf(arquivo, "stroke=\"%s\" stroke-width=\"%s\" ",
              forma->corBorda,
              forma->tipo == SEGMENTO ? "4" : "2");

      if (forma->tipo == SEGMENTO)
      {
        fprintf(arquivo, "stroke-linecap=\"round\" opacity=\"0.8\"/>\n");
      }
      else
      {
        fprintf(arquivo, "/>\n");
      }
    }
    else if (forma->tipo == TEXTO)
    {
      const char *anchor = "start";
      if (forma->dados.texto.posicaoAncora == 'm')
        anchor = "middle";
      else if (forma->dados.texto.posicaoAncora == 'f')
        anchor = "end";

      const char *family = "sans-serif";
      if (strcmp(forma->dados.texto.estilo.family, "serif") == 0)
        family = "serif";
      else if (strcmp(forma->dados.texto.estilo.family, "cursive") == 0)
        family = "cursive";

      const char *weight = "normal";
      if (forma->dados.texto.estilo.weight[0] == 'b')
        weight = "bold";
      else if (strcmp(forma->dados.texto.estilo.weight, "b+") == 0)
        weight = "bolder";
      else if (forma->dados.texto.estilo.weight[0] == 'l')
        weight = "lighter";

      fprintf(arquivo, "  <text x=\"%.2f\" y=\"%.2f\" ",
              forma->dados.texto.ancora.x,
              forma->dados.texto.ancora.y);
      fprintf(arquivo, "font-family=\"%s\" font-size=\"%d\" font-weight=\"%s\" ",
              family, forma->dados.texto.estilo.size, weight);
      fprintf(arquivo, "text-anchor=\"%s\" stroke=\"%s\" fill=\"%s\" ",
              anchor, forma->corBorda, forma->corPreenchimento);
      fprintf(arquivo, "filter=\"url(#sombra)\">%s</text>\n",
              forma->dados.texto.texto);
    }

    no = no->prox;
  }

  fprintf(arquivo, "</svg>\n");
  fclose(arquivo);
}

void desenharRegiaoVisibilidade(Poligono *regiao, const char *nomeArquivo)
{
  if (!regiao || regiao->numVertices < 3)
  {
    fprintf(stderr, "Erro: região de visibilidade inválida\n");
    return;
  }

  FILE *arquivo = fopen(nomeArquivo, "w");
  if (!arquivo)
  {
    fprintf(stderr, "Erro ao criar arquivo SVG: %s\n", nomeArquivo);
    return;
  }

  double minX = 999999.0, minY = 999999.0;
  double maxX = -999999.0, maxY = -999999.0;
  double centroX = 0, centroY = 0;

  for (int i = 0; i < regiao->numVertices; i++)
  {
    if (regiao->vertices[i].x < minX)
      minX = regiao->vertices[i].x;
    if (regiao->vertices[i].x > maxX)
      maxX = regiao->vertices[i].x;
    if (regiao->vertices[i].y < minY)
      minY = regiao->vertices[i].y;
    if (regiao->vertices[i].y > maxY)
      maxY = regiao->vertices[i].y;

    centroX += regiao->vertices[i].x;
    centroY += regiao->vertices[i].y;
  }

  centroX /= regiao->numVertices;
  centroY /= regiao->numVertices;

  double margem = 50.0;
  double largura = maxX - minX + 2 * margem;
  double altura = maxY - minY + 2 * margem;

  fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(arquivo, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
  fprintf(arquivo, "width=\"%.2f\" height=\"%.2f\" ", largura, altura);
  fprintf(arquivo, "viewBox=\"%.2f %.2f %.2f %.2f\">\n",
          minX - margem, minY - margem, largura, altura);

  fprintf(arquivo, "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ",
          minX - margem, minY - margem, largura, altura);
  fprintf(arquivo, "fill=\"#f8f9fa\"/>\n\n");

  fprintf(arquivo, "  <!-- Polígono de visibilidade -->\n");
  fprintf(arquivo, "  <polygon points=\"");
  for (int i = 0; i < regiao->numVertices; i++)
  {
    fprintf(arquivo, "%.2f,%.2f ", regiao->vertices[i].x, regiao->vertices[i].y);
  }
  fprintf(arquivo, "\" fill=\"#ffeb3b\" fill-opacity=\"0.4\" ");
  fprintf(arquivo, "stroke=\"#ff9800\" stroke-width=\"3\"/>\n");

  fprintf(arquivo, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"8\" ",
          centroX, centroY);
  fprintf(arquivo, "fill=\"#ff3d00\" stroke=\"#b71c1c\" stroke-width=\"2\"/>\n");

  fprintf(arquivo, "</svg>\n");
  fclose(arquivo);
}