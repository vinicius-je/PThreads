
#pragma     comment(lib, "pthreadVC2.lib")
#define     HAVE_STRUCT_TIMESPEC
#pragma     once
#define     _CRT_SECURE_NO_WARNINGS 1
#define     _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma     comment(lib, "pthreadVC2.lib")
#define     HAVE_STRUCT_TIMESPEC

#include    <math.h>
#include    <pthread.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
//#include  <assert.h>

#define     NUM_THREADS     4
#define     M_LINHA         10000
#define     M_COLUNA        10000
#define     MB_LINHA        500
#define     MB_COLUNA       500

pthread_mutex_t mutex; /* declaração da variável mutex */

int     qtdMacrobloco = (M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA);
int     counter;
int     macroAtual = 0;
int**   matriz;
int     qtprimo;
double  tempo;

//  struct para armazenar o index de cada macrobloco [sessão critica]

typedef struct
{
    int     linhaInicial;
    int     colunaInicial;
    int     linhaFim;
    int     colunaFim;
}
IndexMacrobloco;

typedef struct
{
    int     fromidx;
    int     length;
}
thread_arg, *ptr_thread_arg;

pthread_t threads[NUM_THREADS];
thread_arg arguments[NUM_THREADS];

IndexMacrobloco* vetIndexMacro;

int     ehPrimo(int n);
void    serial();
void    ext_threads();
void    gerarMatriz();
void    gerarMacrobloco();
void    printIndexMacroElemento();
void    printMacroblocos();
void*   bus_primo(void* param);
void    liberarMemoria();

int main(int argc, char **argv, char **envp)
{
    qtprimo = 0;
    clock_t inicio;
    clock_t fim;
    printf("\nqtd macrobloco: %d\n", qtdMacrobloco);
    gerarMatriz();
    gerarMacrobloco();

    inicio = clock();
    serial();
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de execucao serial: %f segundos\n", tempo);

    inicio = clock();
    ext_threads();
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Tempo de execucao multithread: %f segundos\n", tempo);
    printf("qnt primos: %d\n", qtprimo);

    liberarMemoria();
    return 0;
}

void gerarMatriz()
{
    //  Semente: força o mesmo conjunto de números gerados
    srand(2000);

    //  Aloca espaço de memória para as linhas
    matriz = calloc(M_LINHA, sizeof(int*));

    //  Aloca espaço de memória para as coluas de cada linha
    for (int i = 0; i < M_COLUNA; i++)
    {
        matriz[i] = calloc(M_COLUNA, sizeof(int));
    }
    //  Loop para adicionar os valores aleatórios na matriz
    for (int l = 0; l < M_LINHA; l++)
    {
        for (int c = 0; c < M_COLUNA; c++)
        {
            matriz[l][c] = rand() % 31999;
        }
    }
    //  Loop para imprimir a matriz
    /*for (int l = 0; l < M_LINHA; l++) {
      for (int c = 0; c < M_COLUNA; c++)
        printf("\t%d ", matriz[l][c]);
      printf("\n");
    }*/
}

void gerarMacrobloco()
{
    //  Quantidade de macrobloco por linha e coluna da matriz
    int macroblocoPorLinha = M_LINHA / MB_LINHA;
    int macroblocoPorColuna = M_COLUNA / MB_COLUNA;
    //  Index do proximo macrobloco
    int proxMacroLinha = 0;
    int proxMacroColuna = 0;
    //  Aloca espaço de memória para o vetor de struct
    vetIndexMacro = calloc(qtdMacrobloco, sizeof(IndexMacrobloco));

    int indexVet = 0;
    for (int i = 0; i < macroblocoPorLinha; i++)
    {
        for (int j = 0; j < macroblocoPorColuna; j++)
        {
            //  Armazena cada index no vetor de struct
            vetIndexMacro[indexVet].linhaInicial = proxMacroLinha;
            vetIndexMacro[indexVet].colunaInicial = proxMacroColuna;
            vetIndexMacro[indexVet].linhaFim = proxMacroLinha + MB_LINHA - 1;
            vetIndexMacro[indexVet].colunaFim = proxMacroColuna + MB_COLUNA - 1;
            indexVet++;

            proxMacroColuna += MB_COLUNA;
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
        linhaInit = vetIndexMacro[i].linhaInicial;
        colunaInit = vetIndexMacro[i].colunaInicial;
        linhaFim = vetIndexMacro[i].linhaFim;
        colunaFim = vetIndexMacro[i].colunaFim;

        printf("X MACROBLOCO %d\n\n", i + 1);
        for (int linha = linhaInit; linha <= linhaFim; linha++)
        {
            for (int coluna = colunaInit; coluna <= colunaFim; coluna++)
            {
                printf("%d ", matriz[linha][coluna]);
            }
            printf("\n\n");
        }
    }
}

void serial()
{
    for (int i = 0; i < M_LINHA; i++)
    {
        for (int j = 0; j < M_LINHA; j++)
        {
            if (ehPrimo(matriz[i][j]))
            {
                qtprimo++;
            }
        }
    }
}

void ext_threads()
{
    pthread_t   threads         [NUM_THREADS];
    int         thread_args     [NUM_THREADS];
    int         i;
    int         result_code;

    for (i = 0; i < NUM_THREADS; i++)
    {
        // printf("\n no ext_threads: criando thread %d.\n", i);
        thread_args[i] = i;
        result_code = pthread_create(&threads[i], NULL, bus_primo, &thread_args[i]);
        // assert(!result_code);
    }

    // printf("\n no ext_threads: todas as threads foram criadas.\n");

    // aguardar cada thread terminar
    for (i = 0; i < NUM_THREADS; i++)
    {
        result_code = pthread_join(threads[i], NULL);
        // assert(!result_code);
        // printf("\n no ext_threads: a thread %d terminou.\n", i);
    }

    // printf("\n ext_threads terminou.\n");
}

void* bus_primo(void* arguments)
{
    int i;
    int linhaInit;
    int colunaInit;
    int linhaFim;
    int colunaFim;
    int index;
    int id_t_mb;
    
    index = *((int*)arguments);
    // int    sleep_time  =  1 + rand() % NUM_THREADS;

    // int        id_t_mb = index;
    // NUM_THREADS
    // qtdMacrobloco

    // printf("\n### a thread %d Iniciou.\n", index);
    // printf("\nfuncao %d",index);

    for (
        int id_t_mb = index;
        (id_t_mb + NUM_THREADS) <= qtdMacrobloco + (NUM_THREADS - 1);
        id_t_mb += NUM_THREADS
        )
    {

        if ((id_t_mb + NUM_THREADS) <= qtdMacrobloco + (NUM_THREADS - 1))
        {
            // printf("\nfuncao %d",id_t_mb)

            linhaInit = vetIndexMacro[id_t_mb].linhaInicial;
            colunaInit = vetIndexMacro[id_t_mb].colunaInicial;
            linhaFim = vetIndexMacro[id_t_mb].linhaFim;
            colunaFim = vetIndexMacro[id_t_mb].colunaFim;
            
            pthread_mutex_init(&mutex, NULL);
            pthread_mutex_lock(&mutex);
            
            for (int linha = linhaInit; linha <= linhaFim; linha++)
            {
                for (int coluna = colunaInit; coluna <= colunaFim; coluna++)
                {
                    // printf("%d ", matriz[linha][coluna]);
                    
                    if (ehPrimo(matriz[linha][coluna]) == 1)
                    {
                        qtprimo++;
                    }
                    
                    pthread_mutex_unlock(&mutex);
                    // printf("\n$$$ a thread %d: vai descansar por %d segundos.\n",
                    // index, sleep_time);
                }
            }
            // printf("\n!!! a thread %d Terminou.\n", index);
        }
    }
    return NULL;
    pthread_exit(0);
}

int ehPrimo(int n)
{
    /*
    int i;
    int cont;
    cont = 0;

    for (int i = 1; i <= n;
        i++) // Se o resto da divisão for zero, o i é um divisor,
    {         // assim o contador é incrementado
        if (n % i == 0) {
            cont++;
        }
    }
    if (cont == 2) {
        // qtprimo++;
        // printf("\nP\t %d \test\t\t\tprimo", n);
    }
    else {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
    }
    return (cont == 2) ? 1 : 0;
    */

    if (n == 1)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    int raiz = sqrt(n);
    //Se o resto da divis�o for zero, o i � um divisor
    for (int i = 2; i <= raiz; i++)
    {
        if (n % i == 0)
        {
            // printf("\nN\t %d \tnon est\t\tprimo", n);
            return 0;
        }
    }
    // printf("\nP\t %d \test\t\t\tprimo", n);
    return 1;
}

void liberarMemoria()
{
    free(vetIndexMacro);

    for (int i = 0; i < M_LINHA; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}
