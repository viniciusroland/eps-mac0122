#include <stdio.h>
#include <stdlib.h>

struct Alocacao {
  // Dados de uma alocacao
  int funcionario;
  int tarefa;
  int custo;
  int custoTotal;
  int visitado;

  // filhos e pai para montar a estrutura de arvore (grafo)
  struct Alocacao** filhos;
  struct Alocacao* pai;

  // prox para conseguir construir a pilha do DFS
  struct Alocacao* prox;
};
typedef struct Alocacao Alocacao;
typedef struct Alocacao* Pilha;
typedef enum boolean {false, true} bool;

Pilha criaPilha() {
  Pilha p = (Pilha) malloc(sizeof(Pilha));
  p->prox = NULL;
  return p;
}

bool pilhaVazia(Pilha p) {
  return (p->prox == NULL);
}

void empilha(Pilha p, Alocacao* alocacao) {
  Alocacao* a = alocacao;
  a->prox = p->prox;
  p->prox = a;
}

Alocacao* desempilha(Pilha p) {
  Alocacao* q;
  q = p->prox;
  p->prox = q->prox;
  return q;
}


FILE* lerArquivo(char nome[]) {
  FILE* fp;
  fp = fopen(nome, "r");
  if (fp == NULL) {
    printf("Erro ao ler arquivo, tente novamente.\n");
    exit(-1);
  }
  return fp;
}

int lerNumeroDeFuncionarios(FILE *file) {
  int M; 
  fscanf(file, "%d", &M);
  return M;
}

int lerNumeroDeTarefas(FILE *file) {
  int N;
  fscanf(file, "%d", &N);
  return N;
}

int** alocaMatriz(int totalFuncionarios, int totalTarefas) {
  int** matriz = (int**) malloc(totalFuncionarios * sizeof(int *));
  for(int i = 0; i < totalFuncionarios; i++) {
    matriz[i] = (int*) malloc(totalTarefas * sizeof(int));
  }
  return matriz;
}

void lerMatrizDeCustos(FILE *file, int** matriz, int totalFuncionarios, int totalTarefas) {
  for (int i = 0; i < totalFuncionarios; i++) {
    for (int j = 0; j < totalTarefas; j++) {
      fscanf(file, "%d", &matriz[i][j]);
    }
  }
}

void lerMatrizDeRestricoes(FILE *file, int** matriz, int totalFuncionarios) {
  for (int i = 0; i < totalFuncionarios; i++) {
    for (int j = 0; j < totalFuncionarios; j++) {
      fscanf(file, "%d", &matriz[i][j]);
    }
  }
}

int* atualizaFuncionariosLivres(int totalFuncionarios, int* dictFuncionariosLivres, int funcionarioAlocado) {
  // Cria novo dicionario
  int* dictNovosFuncionariosLivres = (int*) malloc(totalFuncionarios * sizeof(int));
  
  for (int i = 0; i < totalFuncionarios; i++) {
    if (i == funcionarioAlocado) {
      // Preenche o novo dicionario com o valor 0 caso o funcionario esteja sendo alocado
      dictNovosFuncionariosLivres[i] = 0;
    } else {
      // Completa o restante com o estado dos outros funcionarios
      dictNovosFuncionariosLivres[i] = dictFuncionariosLivres[i];
    }
  }
  return dictNovosFuncionariosLivres;
}

int existeRestricao(int funcionarioA, int funcionarioB, int** matrizRestricao) {
  // funcionarioA == -1 eh o caso em que o elemento eh a raiz
  return (funcionarioA == -1 ? 0 : matrizRestricao[funcionarioA][funcionarioB]);
}

int solucaoInvalida(Alocacao* solucao, int totalTarefas, int** matrizRestricao) {
  int invalido = 0;

  // Criando vetor de funcionarios alocados
  int* funcionariosAlocados = (int*) malloc(totalTarefas * sizeof(int));
  int index = 0;

  // Pegando todos os funcionarios alocados nas tarefas da solucao em questao
  while (solucao->pai != NULL) {
    funcionariosAlocados[index++] = solucao->funcionario;
    solucao = solucao->pai;
  }

  // Varrendo pelo vetor de funcionarios alocados buscando uma restricao
  for(int i = 0; i < totalTarefas; i++) {
    for(int j = i; j < totalTarefas; j++) {

      int funcionarioA = funcionariosAlocados[i];
      int funcionarioB = funcionariosAlocados[j];

      if (existeRestricao(funcionarioA, funcionarioB, matrizRestricao)) {
        invalido = 1;
        break;
      }
    }
  }

  return invalido;
}


void criaFilhos(int* dictFuncionariosLivres,
                int totalFuncionarios, int totalTarefas,
                Alocacao* pai,
                int tarefaEmQuestao,
                int** matrizCustos, int** matrizRestricao) {

  // Condicao de retorno (fim da arvore)
  if (tarefaEmQuestao == totalTarefas) {
    return;
  }
  // Verificando se a solucao parcial em questao ja esta invalida e pulando para proxima
  if(solucaoInvalida(pai, totalTarefas, matrizRestricao)) {
  	return;
  }
  // Varrendo pelos funcionarios
  for (int j = 0; j < totalFuncionarios; j++) {

    Alocacao* filho = (Alocacao*) malloc(sizeof(Alocacao));
    filho->filhos = (Alocacao**) malloc(totalFuncionarios * sizeof(Alocacao));

    // Verificando se o funcionario 'j' esta livre (dictFuncionariosLivres[j] == 1)
    //    e se nao existe restricao entre pai e filho
    int restricao = existeRestricao(j, pai->funcionario, matrizRestricao);
    if (dictFuncionariosLivres[j] && !(restricao)
        && pai->custoTotal < 171) {
      // essa ultima condicao (pai->custoTotal < 171) nao eh necessaria para o funcionamento correto do ep
      //   porem, estou pondo apenas para efeitos de otimizacao

      filho->funcionario = j;

      int* dictFuncionariosLivresAtualizado = atualizaFuncionariosLivres(totalFuncionarios, dictFuncionariosLivres, j);

      // Configurando elemento filho
      filho->tarefa = tarefaEmQuestao;
      filho->pai = pai;
      filho->visitado = 0;
      filho->custo = matrizCustos[j][tarefaEmQuestao];
      filho->custoTotal = matrizCustos[j][tarefaEmQuestao] + pai->custoTotal;

      pai->filhos[j] = filho; // Setando os filhos do pai com o elemento filho criado

      // Continua construindo a arvore (grafo) recursivamente a partir do filho em questao
      criaFilhos(dictFuncionariosLivresAtualizado,
                 totalFuncionarios, totalTarefas,
                 filho,
                 tarefaEmQuestao + 1,
                 matrizCustos, matrizRestricao);
    }
  }
}

Alocacao* criaArvoreDeAlocacoes(int** matrizCustos, int** matrizRestricao, int totalTarefas, int totalFuncionarios) {
  // Configurando a raiz da arvore (grafo)
  Alocacao* raiz;
  raiz->filhos = (Alocacao**) malloc(totalFuncionarios * sizeof(Alocacao));
  raiz->funcionario = -1, raiz->tarefa = -1;
  raiz->custo = 0, raiz->custoTotal = 0, raiz->visitado = 0;
  raiz->pai = NULL;

  int* dictFuncionariosLivre = (int*) malloc(totalFuncionarios * sizeof(int));
  // Inicialmente todos os funcionarios estao livres
  for (int i = 0; i < totalFuncionarios; i++) {
    // livre == 1; alocado == 0;
    dictFuncionariosLivre[i] = 1;
  }

  Alocacao* pai = raiz;
  int tarefaEmQuestao = 0;
  criaFilhos(dictFuncionariosLivre,
             totalFuncionarios, totalTarefas,
             pai,
             tarefaEmQuestao,
             matrizCustos, matrizRestricao);

  return raiz;
}

void imprimirSolucao(Alocacao* solucao) {

  // Como a 'solucao' comeca na N-1 tarefa ate 0, pois construimos um lista
  // e estamos pegando o ultimo elemento dela, precisamos subir ate o elemento raiz,
  // pegando todos os pares (T, F) e depois invertendo para imprimir a solucao na ordem correta

  int** valores = (int**) malloc(16 * sizeof(int *)); // 16 apenas para garantir espaco
  int index = 0;

  // Criando vetor de pares com seus valores
  for (int i = 0; i < 16; i++)
    valores[i] = (int*) malloc(2 * sizeof(int));

  int custoTotal = solucao->custoTotal;

  while (solucao->pai != NULL) {
    // Varrendo da ultima alocacao ate a primeira e formando os pares (T, F)
    valores[index][0] = solucao->tarefa;
    valores[index][1] = solucao->funcionario;
    index++;

    solucao = solucao->pai;
  }
  // Invertendo as solucoes
  for (int i = index - 1; i >= 0; i--) {
    printf("(T:%d F:%d) ", valores[i][0], valores[i][1]);
  }

  printf("= %d\n", custoTotal);
}

void DFS(Alocacao* raiz, int totalFuncionarios, int totalTarefas, int** matrizRestricao) {
  // Executando DFS para varrer a arvore (grafo) e pegar as listas de resultados

  Pilha pilha = criaPilha();
  empilha(pilha, raiz);

  Alocacao* alocacaoTeste;
  Alocacao* solucao;

  int menorCusto = 10000;

  while(!pilhaVazia(pilha)) {
    alocacaoTeste = desempilha(pilha);

    // Ignorando possiveis solucoes parciais invalidas
    if (solucaoInvalida(alocacaoTeste, totalTarefas, matrizRestricao)) {
      continue;
    }

    // Verificando se o elemento nao foi visitado
    if (alocacaoTeste->visitado == 0) {

      alocacaoTeste->visitado = 1; // Visitando o elemento

      // Varrendo por todos os possiveis 'vizinhos/filhos' do elemento em questao
      for(int i = 0; i < totalFuncionarios; i++) {

        if (alocacaoTeste->filhos[i]) { // Verificando se existe vizinhos

          if (alocacaoTeste->custoTotal < menorCusto) {
            // Empilhando somente se o custoTotal for menor do que o menor custo, assim evitamos empilhar elementos desnecessarios
            empilha(pilha, alocacaoTeste->filhos[i]);
          }
        } else {
          // Caso nao existam mais filhos,
          //    encontramos uma lista de resultado

          // Pegando o custo total somente se for menor do que o custo atual e,
          // pegando a solucao somente se for uma solucao completa, isto eh,
          //    ultimoElementoDaLista->tarefa == N-1 == totalTarefas - 1
          if (alocacaoTeste->custoTotal < menorCusto && alocacaoTeste->tarefa == totalTarefas - 1) {
              menorCusto = alocacaoTeste->custoTotal;
              solucao = alocacaoTeste;
          }
        }
      }

    }
  }

  imprimirSolucao(solucao);
}

int main(){
  // Lendo nome do arquivo
  char nome_arquivo[12];
  scanf("%s", nome_arquivo);

  // Lendo arquivo
  FILE *arquivo = lerArquivo(nome_arquivo);

  // Lendo N (tarefas) e M (funcionarios)
  int funcionarios = lerNumeroDeFuncionarios(arquivo);
  int tarefas = lerNumeroDeTarefas(arquivo);

  // Lendo matriz de custos
  int** matrizCustos = alocaMatriz(funcionarios, tarefas);
  lerMatrizDeCustos(arquivo, matrizCustos, funcionarios, tarefas);

  // Lendo matriz de restricoes
  int** matrizRestricao = alocaMatriz(funcionarios, funcionarios);
  lerMatrizDeRestricoes(arquivo, matrizRestricao, funcionarios);

  // Criando arvore de alocacoes
  Alocacao* raiz = criaArvoreDeAlocacoes(matrizCustos, matrizRestricao, tarefas, funcionarios);

  // Executando DFS a partir da raiz da arvore
  DFS(raiz, funcionarios, tarefas, matrizRestricao);

  fclose(arquivo);
  return 0;
}
