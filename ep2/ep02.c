#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAG_MAX_SIZE 128

struct Tag {
  struct Tag * prox;
  char nome[TAG_MAX_SIZE];
  int linha;
};
typedef struct Tag Tag;
typedef enum boolean {false, true} bool;
typedef struct Tag* Pilha;

Pilha CriaPilha() {
  Pilha p = malloc(sizeof(Pilha));
  p->prox = NULL;
  return p;
}

bool PilhaVazia(Pilha p) {
  return (p->prox == NULL);
}

void Empilha(Pilha p, Tag * tag) {
  Tag * t = tag;
  t->prox = p->prox;
  p->prox = t;
}

Tag * Desempilha(Pilha p) {
  Tag * q;
  q = p->prox;
  p->prox = q->prox;
  return q;
}

FILE* lerArquivo(char nome[]) {
  FILE * file;
  file = fopen(nome, "r");
  if (file == NULL) {
    printf("Erro ao ler o arquivo, tente novamente!\n");
    exit(-1);
  }
  return file;
}

int tagContemBarra(char tag[]) {
  char* verificador = strchr(tag, '/');
  if (verificador == NULL) {
    return 0;
  } else {
    return 1;
  }
}

void removerChar(char *s, char c) {
    int writer = 0, reader = 0;
    while (s[reader]) {
        if (s[reader]!=c) {   
            s[writer++] = s[reader];
        }
        reader++;       
    }
    s[writer] = 0;
}

void lowerString(char s[]) {
   int c = 0;
   while (s[c] != '\0') {
      if (s[c] >= 'A' && s[c] <= 'Z') {
         s[c] = s[c] + 32;
      }
      c++;
   }
}

int formaParDeTagsBalanceados(Tag *tag1, Tag *tag2) {
  removerChar(tag2->nome, '/'); // retirando a barra para comparar as tags

  int verificacao = strcmp(tag2->nome, tag1->nome); // verificando se as tags sao iguais 
  if (verificacao != 0) { // caso em que elas sao diferentes

    char* tagLower;
    strcpy(tagLower, tag2->nome);
    lowerString(tagLower); // colocando a tag errada em lower case
    if (strcmp(tagLower, tag1->nome) == 0) { // verificando se as tags sao iguais dessa maneira
      printf("Violacao da regra #1:\n");
    } else {
      printf("Violacao da regra #2:\n");
    }
    printf("Tags incompativeis (abertura '%s' na linha %d e fechamento '%s' na linha %d)\n", tag1->nome, tag1->linha, tag2->nome, tag2->linha);
    return 0;
  } else {
    return 1;
  }
}

void LiberaPilha(Pilha p) {
  Tag *q, *t;
  q = p;
  while (q != NULL) {
    t = q;
    q = q->prox;
    free(t);
  }
}

Tag * criarTag(char nome[], int linha) {
  Tag * tag = malloc(sizeof(Tag));
  strcpy(tag->nome, nome);
  tag->linha = linha;
  return tag;
}

int main() {
  char ch;
  int i = 0, linha = 1;
  Pilha pilha = CriaPilha();
  
  char nome_arquivo[TAG_MAX_SIZE];
  scanf("%s", nome_arquivo);
  FILE * fp = lerArquivo(nome_arquivo);

  fscanf(fp, "%*[^\n]\n"); // ignorando a primeira linha 
  linha++;

  while((ch = fgetc(fp)) != EOF) { // percorrendo todo o arquivo
    if (ch == '\n') {
      linha++;
    }
    char tagComAtributos[TAG_MAX_SIZE];
    if (ch == '<') {

      fscanf(fp, "%[^>]>", tagComAtributos); // lendo a tag com os atributos
      char* nome = strtok(tagComAtributos, " "); // limpando a tag dos atributos

      Tag * tag = criarTag(nome, linha);

      if (PilhaVazia(pilha)) {
        // verificando se a pilha esta vazia depois se ter sido preenchida (indicando uma segunda raiz)
        if (i != 0) {
          printf("Violacao da regra #3:\n");
          printf("Raiz adicional ('%s' na linha %d)\n", tag->nome, tag->linha);
          exit(0);
        }
        i++;
      }

      if (tagContemBarra(tag->nome)) { // verificando se eh uma tag de fechamento
        if (PilhaVazia(pilha)) {
          break;
        }
        Tag* tag_ = Desempilha(pilha);
        // verificando se as tags formam um par balanceado
        if (!formaParDeTagsBalanceados(tag_, tag)) {
          break;
        }
      } else {
        Empilha(pilha, tag);
      }
    }
  }
  fclose(fp);
  if (PilhaVazia(pilha)) {
    printf("Tags balanceadas\n");
  }
  LiberaPilha(pilha);
  return 0;
}
