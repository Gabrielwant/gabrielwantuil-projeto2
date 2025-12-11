#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdio.h>

// Enum que identifica qual tipo de forma está armazenada na struct Forma
typedef enum
{
  CIRCULO,
  RETANGULO,
  LINHA,
  TEXTO,
  SEGMENTO
} TipoForma;

typedef struct
{
  double x;
  double y;
} Ponto;

// Define estilo visual usado especificamente para textos (fonte, peso e tamanho)
typedef struct
{
  char family[32];
  char weight[32];
  int size;
} EstiloTexto;

// Estrutura genérica que pode representar círculo, retângulo, linha, texto ou segmento
typedef struct
{
  int id;
  TipoForma tipo;
  int ativo;
  char corBorda[64];
  char corPreenchimento[64];

  union
  {
    struct
    {
      Ponto centro;
      double raio;
    } circulo;

    struct
    {
      Ponto ancora;
      double largura;
      double altura;
    } retangulo;

    struct
    {
      Ponto p1;
      Ponto p2;
    } linha;

    struct
    {
      Ponto ancora;
      char texto[512];
      char posicaoAncora;
      EstiloTexto estilo;
    } texto;
  } dados;
} Forma;

Forma *criarCirculo(int id, double x, double y, double r, const char *corb, const char *corp);
Forma *criarRetangulo(int id, double x, double y, double w, double h, const char *corb, const char *corp);
Forma *criarLinha(int id, double x1, double y1, double x2, double y2, const char *cor);
Forma *criarTexto(int id, double x, double y, const char *corb, const char *corp,
                  char ancora, const char *txt, EstiloTexto *estilo);
Forma *criarSegmento(int id, double x1, double y1, double x2, double y2, const char *cor);

void destruirForma(void *forma);
const char *obterTipoForma(TipoForma tipo);

Ponto criarPonto(double x, double y);
double distanciaPontos(Ponto p1, Ponto p2);
double produtoVetorial(Ponto p1, Ponto p2, Ponto p3);
int viradaEsquerda(Ponto p1, Ponto p2, Ponto p3);
int viradaDireita(Ponto p1, Ponto p2, Ponto p3);
int colineares(Ponto p1, Ponto p2, Ponto p3);

int segmentosIntersectam(Ponto p1, Ponto p2, Ponto p3, Ponto p4);
Ponto intersecaoSegmentos(Ponto p1, Ponto p2, Ponto p3, Ponto p4);
int pontoNoSegmento(Ponto p, Ponto s1, Ponto s2);

// Estrutura usada para representar um polígono dinâmico de vértices
typedef struct
{
  Ponto *vertices;
  int numVertices;
  int capacidade;
} Poligono;

Poligono *criarPoligono(int capacidadeInicial);
void adicionarVerticePoligono(Poligono *pol, Ponto p);
void destruirPoligono(Poligono *pol);
int pontoEmPoligono(Ponto p, Poligono *pol);

#endif
