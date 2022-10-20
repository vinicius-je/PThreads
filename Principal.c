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

#include <time.h>
#include <math.h>
//define para matriz
#define LINHA 3
#define COLUNA 3
//define para macrobloco
#define MBLINHA 2
#define MBCOLUNA 2

//matriz de dois ponteiros, uma para linha e outro para coluna, REF página 114 do livro 1 de C Giraldelli
int **matriz;
//variável global
int qtdPrimos = 0;

int ehPrimo(int numero);
void* gerarMatriz();

int main(int argc, char* argv[]) {
    gerarMatriz();

    //Loop para imprimir a matriz
    for (int l = 0; l < LINHA; l++) {
        for (int c = 0; c < COLUNA; c++) {
            printf("%d ", matriz[l][c]);
            if (ehPrimo(matriz[l][c])) qtdPrimos++;
        }
        printf("\n");
    }

    printf("QTD PRIMOS %d", qtdPrimos);

    //Libera as linhas da matriz
    for (int i = 0; i < LINHA; i++) 
        free(matriz[i]);
    //Libera a matriz de ponteiro
    free(matriz);
	
	return 0;
}

//Funções
int ehPrimo(int numero) {
    if (numero == 1) return 0;
    int raiz = sqrt(numero);
    //Se o resto da divisão for zero, o i é um divisor
    for (int i = 2; i <= raiz; i++) {
        if (numero % i == 0) return 0;
    }

    return 1;
}

void* gerarMatriz() {
    srand(2000);
    //Aloca espaço de memória para as linhas
    matriz = calloc(LINHA, sizeof(int*));

    //Aloca espaço de memória para as coluas de cada linha
    for (int i = 0; i < COLUNA; i++)
        matriz[i] = calloc(COLUNA, sizeof(int));


    //Loop para adicionar os valores aleatórios na matriz
    for (int l = 0; l < LINHA; l++) {
        for (int c = 0; c < COLUNA; c++)
            matriz[l][c] = rand() % 31999;
    }

    /* 
    //Libera as linhas da matriz
    for (int i = 0; i < LINHA; i++)
        free(matriz[i]);
    //Libera a matriz de ponteiro
    free(matriz);
    */
}