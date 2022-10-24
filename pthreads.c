
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC
#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#include  <pthread.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <time.h>
#include  <math.h>
//#include  <assert.h>

#define              NUM_THREADS    4
#define              M_LINHA        10
#define              M_COLUNA       10
#define              MB_LINHA       2
#define              MB_COLUNA      2

pthread_mutex_t      mutex; /* declaração da variável mutex */

int                  qtdMacrobloco = (M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA);
int                  counter;
int                  macroAtual = 0;
int                  **matriz;
int                  qtprimo;

// STRUCT PARA CONTROLAR OS MACROBLOCOS [sessão critica]

typedef struct
{
    int    linhaInicial;
    int    colunaInicial;
    int    linhaFim;
    int    colunaFim;
}
IndexMacrobloco;

typedef struct
{
    int  fromidx;
    int  length;
}
thread_arg, *ptr_thread_arg;

pthread_t          threads    [NUM_THREADS];
thread_arg         arguments  [NUM_THREADS];

IndexMacrobloco    *vetIndexMacro;

int    vef_primo                (int numero);
void   ext_threads              ();
void   gerarMatriz              ();
void   gerarMacrobloco          ();
void   printIndexMacroElemento  ();
void   printMacroblocos         ();
void   *bus_primo               (void *param);

int main (int argc, char *argv[], char *envp[])
{
    qtprimo = 0;
    gerarMatriz();
    printf("\nqtd macrobloco: %d\n", qtdMacrobloco);
    gerarMacrobloco();
    //printMacroblocos();
    ext_threads();
    printf("\n qnt primos: %d\n", qtprimo);
    return 0;
}

void gerarMatriz()
{
    //  Semente: força o mesmo conjunto de números
    srand(2000);

    //  Aloca espaço de memória para as linhas
    matriz = calloc(M_LINHA, sizeof(int*));

    //  Aloca espaço de memória para as coluas de cada linha
    for(int i = 0; i < M_COLUNA; i++)
    {
      matriz[i] = calloc(M_COLUNA, sizeof(int));
    }
    //  Loop para adicionar os valores aleatórios na matriz
    for(int l = 0; l < M_LINHA; l++)
    {
        for(int c = 0; c < M_COLUNA; c++)
        {
            matriz[l][c] = rand() % 31999;  
        }
    }
    //  Loop para imprimir a matriz
    for(int l = 0; l < M_LINHA; l++)
    {
        for(int c = 0; c < M_COLUNA; c++) printf("\t%d ", matriz[l][c]);
        printf("\n");
    }  
}

void gerarMacrobloco()
{
    //  Quantidade de macrobloco por linha e coluna da matriz
    int macroblocoPorLinha = M_LINHA / MB_LINHA;
    int macroblocoPorColuna = M_COLUNA / MB_COLUNA;
    //  Index do proximo macrobloco
    int proxMacroLinha = 0;
    int proxMacroColuna = 0;
    // Aloca espaço de memória para o vetor de struct
    vetIndexMacro = calloc(qtdMacrobloco, sizeof(IndexMacrobloco));
    
    int indexVet = 0;
    for(int i = 0; i < macroblocoPorLinha; i++)
    {
        for(int j = 0; j < macroblocoPorColuna; j++)
        {
            // Armazena cada index no vetor de struct
            vetIndexMacro[indexVet].linhaInicial = proxMacroLinha;
            vetIndexMacro[indexVet].colunaInicial = proxMacroColuna;
            vetIndexMacro[indexVet].linhaFim = proxMacroLinha + MB_LINHA - 1;
            vetIndexMacro[indexVet].colunaFim = proxMacroColuna + MB_COLUNA - 1;
            indexVet++;

            proxMacroColuna += MB_COLUNA;

            /* REMOVER AO FINAL DO TRABALHO [SERVER PARA IMPRIMIR O INDEX DE CADA MACROBLOCO]
            printf("\nprimeiro: %d", matriz[proxMacroLinha][proxMacroColuna]);
            printf("\nultimo: %d", matriz[proxMacroLinha + MB_LINHA - 1][proxMacroColuna + MB_COLUNA - 1]);
            printf("\n");
            */

        }
        proxMacroLinha += MB_LINHA;
        proxMacroColuna = 0;
    }
}

void printMacroblocos()
{
    int linhaInit;
    int colunaInit;
    int linhaFim;
    int colunaFim;
  
    for (int i = 0; i < qtdMacrobloco; i++)
    {
        // APENAS ORGANIZAÇÃO   
        linhaInit   = vetIndexMacro[i].linhaInicial;
        colunaInit  = vetIndexMacro[i].colunaInicial;
        linhaFim    = vetIndexMacro[i].linhaFim;
        colunaFim   = vetIndexMacro[i].colunaFim;

        printf("X MACROBLOCO %d\n\n", i + 1);
        for (int linha = linhaInit; linha <= linhaFim; linha++)
        {
            for(int coluna = colunaInit; coluna <= colunaFim ; coluna++)
            {
                printf("%d ", matriz[linha][coluna]);
            }
            printf("\n\n");
        }
    }
}





void ext_threads()
{
  pthread_t  threads      [NUM_THREADS];
  int        thread_args  [NUM_THREADS];
  int        i;
  int        result_code;

  for (i = 0; i < NUM_THREADS; i++)
  {
    //printf("\n no ext_threads: criando thread %d.\n", i);
    thread_args[i] = i;
    result_code = pthread_create(&threads[i], NULL, bus_primo, &thread_args[i]);
    //assert(!result_code);
  }

  //printf("\n no ext_threads: todas as threads foram criadas.\n");

  //aguardar cada thread terminar
  for (i = 0; i < NUM_THREADS; i++)
  {
    result_code = pthread_join(threads[i], NULL);
    //assert(!result_code);
    //printf("\n no ext_threads: a thread %d terminou.\n", i);
  }
  
  //printf("\n ext_threads terminou.\n");
  return 0;
}

void *bus_primo(void *arguments)
{
  int i; 
  int linhaInit;
  int colunaInit;
  int linhaFim;
  int colunaFim;
  int index       =  *((int *)arguments);
  int sleep_time  =  1 + rand() % NUM_THREADS;
  
  //int        id_t_mb = index;
  //NUM_THREADS
  //qtdMacrobloco
  
  //printf("\n### a thread %d Iniciou.\n", index);

  //printf("\nfuncao %d",index);
  for (
        int id_t_mb = index;
        (id_t_mb + NUM_THREADS) <= qtdMacrobloco + (NUM_THREADS - 1);
        id_t_mb += NUM_THREADS
      )
  {
    
    if (
         (id_t_mb + NUM_THREADS) <= qtdMacrobloco + (NUM_THREADS - 1)
       )
    {
      //printf("\nfuncao %d",id_t_mb);
      
      pthread_mutex_init(&mutex, NULL);
      pthread_mutex_lock(&mutex);
      
      linhaInit   = vetIndexMacro[id_t_mb].linhaInicial;
      colunaInit  = vetIndexMacro[id_t_mb].colunaInicial;
      linhaFim    = vetIndexMacro[id_t_mb].linhaFim;
      colunaFim   = vetIndexMacro[id_t_mb].colunaFim;
  
      for (int linha = linhaInit; linha <= linhaFim; linha++)
      {
        for (int coluna = colunaInit; coluna <= colunaFim ; coluna++)
        {
          //printf("%d ", matriz[linha][coluna]);  
          if (vef_primo(matriz[linha][coluna]) == 1)
          {
            qtprimo++;
          }
          pthread_mutex_unlock(&mutex);
          //printf("\n$$$ a thread %d: vai descansar por %d segundos.\n", index, sleep_time);
        }
      }
      //printf("\n!!! a thread %d Terminou.\n", index);
    }
  }
  return NULL;
  pthread_exit(0);
}

int vef_primo(int nmb)
{
    int i;
    int cont;
    cont = 0;

    for (int i = 1; i <= nmb; i++)               // Se o resto da divisão for zero, o i é um divisor,               
    {                                               // assim o contador é incrementado
        if (nmb % i == 0)
        {
            cont++;
        }
    }
    if (cont == 2)
    {
        //qtprimo++;
      //printf("\nP\t %d \test\t\t\tprimo", nmb);
    }
    else
    {
      //printf("\nN\t %d \tnon est\t\tprimo", nmb);
    }
    return (cont == 2) ? 1 : 0;
}
