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
// #include    <assert.h> // unix

#define     NUM_THREADS                         4
#define     M_LINHA                             1000
#define     M_COLUNA                            1000
#define     MB_LINHA                            2
#define     MB_COLUNA                           2
#define     ld_mutex                            1 //manual

/* declaração da variável mutex */
pthread_mutex_t mutex;
pthread_mutex_t mb_mutex;

int         qtdMacrobloco = (M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA);
int         counter;
int         macroAtual = 0;
int         feito[(M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA)];
int** matriz;
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

int         serialethread();
int         ext_threads();
int         ehPrimo(int n);

void* bus_primo(void* param);
void        serial();
void        gerarMatriz();
void        gerarMacrobloco();
//void      printIndexMacroElemento();
void        printMacroblocos();
void        liberarMemoria();

int main(int argc, char** argv, char** envp)
{
    serialethread();
    return 0;
}

int serialethread()
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
    //printMacroblocos();
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
        //printf("\n");
        for (int l = 0; l < M_LINHA; l++)
        {
            for (int c = 0; c < M_COLUNA; c++)
            {
                matriz[l][c] = rand() % 31999;
                //printf("\t%d ", matriz[l][c]);
            }
            //printf("\n");
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
                printf("%d ", matriz[l][c]);
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

int ext_threads()
{
    pthread_t   threads[NUM_THREADS];
    int         thread_args[NUM_THREADS];
    int         i;
    int         result_code;

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        //printf("\n mutex init nao funcionou\n");
        return 1;
    }

    if (pthread_mutex_init(&mb_mutex, NULL) != 0)
    {
        //printf("\n mutex init nao funcionou\n");
        return 1;
    }

    for (i = 0; i < NUM_THREADS; i++)
    {
        //printf("\n no ext_threads: criando thread %d.\n", i);
        thread_args[i] = i;
        result_code = pthread_create(&threads[i], NULL, bus_primo, &thread_args[i]);
        //assert(!result_code);
    }

    // printf("\n no ext_threads: todas as threads foram criadas.\n");

    // aguardar cada thread terminar
    for (i = 0; i < NUM_THREADS; i++)
    {
        result_code = pthread_join(threads[i], NULL);

        //assert(!result_code);
        // printf("\n no ext_threads: a thread %d terminou.\n", i);
    }
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mb_mutex);
    // printf("\n ext_threads terminou.\n");
    return 0;
}

void* bus_primo(void* arguments)
{
    //int i;
    int     linhaInit;
    int     colunaInit;
    int     linhaFim;
    int     colunaFim;
    int     id_t_mb;
    int     index = *((int*)arguments);

    //int    sleep_time  =  1 + rand() % NUM_THREADS;

    // printf("\n### a thread %d Iniciou.\n", index);
    // printf("\nfuncao %d",index);
    //for (int id_t_mb = index; (id_t_mb + NUM_THREADS) <= (qtdMacrobloco + (NUM_THREADS - 1)); id_t_mb += NUM_THREADS)

  //int         feito[(M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA)];

    //printf("\n%d",&mb_mutex);
    //printf("\n%d",feito[id_t_mb]);
    for (int id_t_mb = index; (id_t_mb+1) <= (qtdMacrobloco); id_t_mb= id_t_mb+1)
    {
        //printf("\n%d", feito[id_t_mb]);
        if (feito[id_t_mb] == 0)
        {
            //printf("\n%d", &mb_mutex);
            //pthread_mutex_lock(&mb_mutex);
            //printf("\n%d", &mb_mutex);
                //printf("\n%d",mb_mutex);
            feito[id_t_mb] = 1;
            //pthread_mutex_unlock(&mb_mutex);
            //printf("\n%d",&mb_mutex);


            linhaInit = vetIndexMacro[id_t_mb].linhaInicial;
            colunaInit = vetIndexMacro[id_t_mb].colunaInicial;
            linhaFim = vetIndexMacro[id_t_mb].linhaFim;
            colunaFim = vetIndexMacro[id_t_mb].colunaFim;

            for (int linha = linhaInit; linha <= linhaFim; linha++)
            {
                for (int coluna = colunaInit; coluna <= colunaFim; coluna++)
                {
                    if (ehPrimo(matriz[linha][coluna]) == 1)
                    {
                        //pthread_mutex_lock(&mb_mutex);
                        pthread_mutex_lock(&mutex); //manual
                        //printf("\n%d",&mutex);
                        //if (mutex == 1)
                        //{
                            qtprimo++;
                        //}
                        pthread_mutex_unlock(&mutex); //manual
                        //pthread_mutex_unlock(&mb_mutex);
                    }
                    // printf("\n$$$ a thread %d: vai descansar por %d segundos.\n", index, sleep_time);
                }
            }




        }


        // printf("\n!!! a thread %d Terminou.\n", index);
    }
    //return(NULL);
    pthread_exit(NULL);
}

int ehPrimo(int n)
{
    /*
    if (n == 0)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }*/
    if (n == 1)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    /*
    if (n == 2)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if (n == 3)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if (n == 5)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if (n == 7)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if (n == 11)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if (n == 13)
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 1;
    }
    if ((n / 2) == round((n / 2)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    if ((n / 3) == round((n / 3)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    if ((n / 5) == round((n / 5)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    if ((n / 7) == round((n / 7)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    if ((n / 11) == round((n / 11)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    if ((n / 10) == round((n / 10)))
    {
        // printf("\nN\t %d \tnon est\t\tprimo", n);
        return 0;
    }
    */
    int raiz = sqrt(n);
    //Se o resto da divis�o for zero, o i � um divisor
    for (int i = 2; i <= raiz; i++)
    {
        if (n % i == 0)
        {
            //printf("\nN\t %d \tnon est\t\tprimo", n);
            return 0;
        }
    }
    //printf("\nP\t %d \test\t\t\tprimo", n);
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