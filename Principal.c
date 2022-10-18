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
//Define para matriz
#define LINHA 10
#define COLUNA 10

//matriz de dois ponteiros, uma para linha e outro para coluna, REF p�gina 114 do livro 1 de C Giraldelli
int **matriz;
//vari�vel global
int qtdPrimos = 0;

void* ehPrimo(int numero);

int main(int argc, char* argv[]) {
    srand(time(NULL));
    //Aloca espa�o de mem�ria para as linhas
    matriz = calloc(LINHA, sizeof(int*));

    //Aloca espa�o de mem�ria para as coluas de cada linha
    for (int i = 0; i < COLUNA; i++)
        matriz[i] = calloc(COLUNA, sizeof(int));
   

    //Loop para adicionar os valores aleat�rios na matriz
    for (int l = 0; l < LINHA; l++) {
        for (int c = 0; c < COLUNA; c++)
            matriz[l][c] = rand() % 31999;
    }

    //Loop para imprimir a matriz
    for (int l = 0; l < LINHA; l++) {
        for (int c = 0; c < COLUNA; c++) {
            printf("%d ", matriz[l][c]);
            ehPrimo(matriz[l][c]);
        };
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

//Fun��es
void* ehPrimo(int numero) {
    int contador = 0;
    //Se o resto da divis�o for zero, o i � um divisor, assim o contador � incrementado
    for (int i = 1; i <= numero; i++) if (numero % i == 0) contador++;

    if (contador == 2) qtdPrimos++;
}