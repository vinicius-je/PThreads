#pragma     comment(lib,"pthreadVC2.lib")
#define     HAVE_STRUCT_TIMESPEC
#pragma     once
#define     _CRT_SECURE_NO_WARNINGS             1
#define     _WINSOCK_DEPRECATED_NO_WARNINGS     1
#pragma     comment(lib,"pthreadVC2.lib")
#define     HAVE_STRUCT_TIMESPEC

#include    <math.h>
#include    <pthread.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>

#define     NUM_THREADS                         4
#define     M_LINHA                             10000
#define     M_COLUNA                            10000
#define     MB_LINHA                            2
#define     MB_COLUNA                           2
#define     ld_mutex                            1 //manual

/* declaração da variável mutex */
pthread_mutex_t mutex;

int         qtdMacrobloco = (M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA);
int         counter;
int         macroAtual = 0;
int**       matriz;
int         qtprimo;
double      tempo;

/*  struct para armazenar o index de cada macrobloco [sessão critica] */
typedef struct
{
    int     linhaInicial;
    int     colunaInicial;
    int     linhaFim;
    int     colunaFim;
}
IndexMacrobloco;

IndexMacrobloco* vetIndexMacro;

int         ext_threads();
int         ehPrimo(int n);
void*       bus_primo(void* param);
void        serial();
void        gerarMatriz();
void        gerarMacrobloco();
void        liberarMemoria();

int main(int argc, char** argv, char** envp)
{
    clock_t inicio;
    clock_t fim;
    printf("---------------------------------------------------------------\n");
    printf("\tMatriz\n");
    printf("\tLinha x Coluna:                          %d x %d\n", M_LINHA, M_COLUNA);
    printf("\tQuantidade de elementos da matriz:       %d\n", (M_LINHA * M_COLUNA));
    printf("---------------------------------------------------------------\n");
    printf("\tMacrobloco\n");
    printf("\tLinha x Coluna:                          %d x %d\n", MB_LINHA, MB_COLUNA);
    printf("\tQuantidade de elementos do macrobloco:   %d\n", (MB_LINHA * MB_COLUNA));
    printf("---------------------------------------------------------------\n");
    printf("\tQuantidade de macroblocos:               %d\n", qtdMacrobloco);
    printf("---------------------------------------------------------------\n");
    printf("\tThreads\n");
    printf("\tNumero de threads:                       %d\n", NUM_THREADS);
    printf("\tMutex? (1-sim, 0-nao):                   %d\n", ld_mutex);
    gerarMatriz();
    gerarMacrobloco();
    printf("---------------------------------------------------------------\n");

    qtprimo = 0;
    inicio = clock();
    serial();
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("\tExecucao Serial\n");
    printf("\tTempo de execucao serial:                %f segundos\n", tempo);
    printf("\tQuantidade primos:                       %d\n", qtprimo);
    printf("\tQuantidade nao primos:                   %d\n", ((M_LINHA * M_COLUNA) - qtprimo));

    qtprimo = 0;
    inicio = clock();
    ext_threads();
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("---------------------------------------------------------------\n");
    printf("\tExecucao Multithread\n");
    printf("\tTempo de execucao multithread:           %f segundos\n", tempo);
    printf("\tQuantidade primos:                       %d\n", qtprimo);
    printf("\tQuantidade nao primos:                   %d\n", ((M_LINHA * M_COLUNA) - qtprimo));
    liberarMemoria();
    printf("---------------------------------------------------------------\n");

    return 0;
}

void gerarMatriz()
{
    if (M_LINHA == M_COLUNA)
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
    }
    if (M_LINHA != M_COLUNA)
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

int ext_threads()
{
    pthread_t   threads[NUM_THREADS];
    int         thread_args[NUM_THREADS];
    int         i;
    int         result_code;

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        return 1;
    }

    for (i = 0; i < NUM_THREADS; i++)
    {
        thread_args[i] = i;
        result_code = pthread_create(&threads[i], NULL, bus_primo, &thread_args[i]);
    }
    
    for (i = 0; i < NUM_THREADS; i++)
    {
        result_code = pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* bus_primo(void* arguments)
{
    int     linhaInit;
    int     colunaInit;
    int     linhaFim;
    int     colunaFim;
    int     id_t_mb;
    int     index = *((int*)arguments);

    for (int id_t_mb = index; (id_t_mb + NUM_THREADS) <= (qtdMacrobloco + (NUM_THREADS - 1)); id_t_mb += NUM_THREADS)
    {
        linhaInit   = vetIndexMacro[id_t_mb].linhaInicial;
        colunaInit  = vetIndexMacro[id_t_mb].colunaInicial;
        linhaFim    = vetIndexMacro[id_t_mb].linhaFim;
        colunaFim   = vetIndexMacro[id_t_mb].colunaFim;

        for (int linha = linhaInit; linha <= linhaFim; linha++)
        {
            for (int coluna = colunaInit; coluna <= colunaFim; coluna++)
            {
                if (ehPrimo(matriz[linha][coluna]) == 1)
                {
                    pthread_mutex_lock(&mutex); //manual
                    qtprimo++;
                    pthread_mutex_unlock(&mutex); //manual
                }
            }
        }
    }
    pthread_exit(NULL);
}

int ehPrimo(int n)
{
    if (n == 1)
    {
        return 0;
    }
    int raiz = sqrt(n);
    for (int i = 2; i <= raiz; i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
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
