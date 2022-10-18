/* Declaração das bibliotecas internas*/
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "main.h"
#include "func.h"

#define LIMIT	100
#define LINHA	10
#define COLUNA	10

int qtdPrimos = 0;                                  // variável global
int** matriz;

int ehPrimo(int numero)
{
    int i;
    int contador;
    contador = 0;

    for (int i = 1; i <= numero; i++)               // Se o resto da divisão for zero, o i é um divisor,                       
    {                                               // assim o contador é incrementado
        if (numero % i == 0)
        {
            contador++;
        }
    }
    if (contador == 2)
    {
        qtdPrimos++;
    }
    return (contador == 2) ? 1 : 0;
}

void gerarMatriz()
{
    srand(time(NULL));
    matriz = calloc(LINHA, sizeof(int*));           // Aloca espaço de memória para as linhas

    for (int i = 0; i < COLUNA; i++)
    {
        matriz[i] = calloc(COLUNA, sizeof(int));    // Aloca espaço de memória para as coluas de cada linha
    }
    for (int l = 0; l < LINHA; l++)                 // Loop para adicionar os valores aleatórios na matriz
    {
        for (int c = 0; c < COLUNA; c++)
        {
            matriz[l][c] = rand() % 31999;
        }
    }
    for (int l = 0; l < LINHA; l++)                 // Loop para imprimir a matriz
    {
        for (int c = 0; c < COLUNA; c++)
        {
            printf("\t%d ", matriz[l][c]);
            ehPrimo(matriz[l][c]);
        }
        printf("\n");
    }
    for (int i = 0; i < LINHA; i++)
    {
        free(matriz[i]);                            // Libera as linhas da matriz
    }
    free(matriz);                                   // Libera a matriz de ponteiro

    printf("QTD PRIMOS %d", qtdPrimos);
    return 0;
}
