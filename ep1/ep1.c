/********************************/
/*            EP01              */
/* Nome: Vinicius Roland Crisci */
/* NUSP: 10773381               */
/* Disciplina: MAC0122          */
/* Professor: Paulo Miranda     */
/********************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Vertice {
  double x;
  double y;
  struct Vertice * prox;
};

typedef struct Vertice * Poligono;

Poligono CriaPoligono(int lados) {
  // leitura do primeiro ponto
  Poligono cabeca = (Poligono) malloc(sizeof(Poligono));
  scanf("%lf %lf", &(cabeca->x), &(cabeca->y));

  struct Vertice * atual = cabeca;

  //leitura dos demais pontos
  for(int i = 1; i < lados; i++) {
    struct Vertice * proximo = (struct Vertice *)malloc(sizeof(struct Vertice));
    scanf("%lf %lf", &(proximo->x), &(proximo->y));

    atual->prox = proximo;
    atual = proximo;
  }
  //tornando circular
  atual->prox = cabeca;
  return cabeca;
}

int NumeroDeVertices(Poligono P) {
  struct Vertice * inicio = P;
  struct Vertice * atual = inicio->prox;

  int vertices = 1;
  while(atual != inicio) {
    vertices++;
    atual = atual->prox;
  }

  return vertices;
}

double Distancia(struct Vertice * A, struct Vertice * B) {
  double dx = A->x - B->x;
  double dy = A->y - B->y;

  return sqrt(dx * dx + dy * dy);
}

double Perimetro(Poligono P) {
  double perimetro = 0.0;
  int vertices = NumeroDeVertices(P);
  struct Vertice * atual = P;

  // basta varrer por todos os vertices do poligono calculando a distancia entre o vertice atual e o proximo
  for(int i = 0; i < vertices; i++) {
    perimetro += Distancia(atual, atual->prox);
    atual = atual->prox;
  }

  return perimetro;
}

double Area(Poligono P) {
  // para calcular a area precisamos montar um determinante com os pontos do poligono
  double dp = 0.0; // somamos o produto das 'diagonais principais', denominado dp
  double ds = 0.0; // somamos o produto das 'diagonais secundarias', denominado ds
  // e entao pegamos o módulo de dp - ds e dividimos por 2

  struct Vertice * inicio = P;
  struct Vertice * atual = inicio->prox;

  int vertice = 1;
  while(atual != inicio) {
    vertice++;
    if(vertice == 2) {
      //andando um elemento com o ponteiro 'inicio' para conseguirmos montar o determinante completo (repetindo o primeiro elemento)
      inicio = inicio->prox;
    }
    dp += atual->x * atual->prox->y;
    ds += atual->y * atual->prox->x;

    atual = atual->prox;
  }
  double area = ((dp > ds) ? (dp - ds) / 2 : (ds - dp) / 2);
  return area;
}

void LiberaPoligono(Poligono P) {
  int vertices = NumeroDeVertices(P);
  struct Vertice * tmp = (struct Vertice *) malloc(sizeof(struct Vertice));
  for(int i = 0; i < vertices; i++) {
    tmp = P;
    P = P->prox;
    free(tmp);
  }
}

int main() {
  int lados; scanf("%d", &lados);

  Poligono poligono = CriaPoligono(lados);

  printf("Perimetro: %.1f\n", Perimetro(poligono));
  printf("Area: %.1f\n", Area(poligono));

  LiberaPoligono(poligono);
  return 0;
}
