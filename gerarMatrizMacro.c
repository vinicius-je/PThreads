#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//  MATRIZ
#define LINHA 9
#define COLUNA 9

// MACROBLOCO
#define MB_LINHA 3
#define MB_COLUNA 3
//  matriz de dois ponteiros, uma para linha e outro para coluna, REF página 114 do livro 1 de C Giraldelli
int **matriz;

void gerarMatriz();
void gerarMacrobloco();

int main(){
    gerarMatriz();
    printf("\n\n");
    gerarMacrobloco();
    //gerarMatriz();
    return 0;
}

void gerarMatriz(){
    srand(2000);
    //Aloca espaço de memória para as linhas
    matriz = calloc(LINHA, sizeof(int*));

    //Aloca espaço de memória para as coluas de cada linha
    for(int i = 0; i < COLUNA; i++) matriz[i] = calloc(COLUNA, sizeof(int));

    //Loop para adicionar os valores aleatórios na matriz
    for(int l = 0; l < LINHA; l++)
        for(int c = 0; c < COLUNA; c++) matriz[l][c] = rand() % 31999;  

    //Loop para imprimir a matriz
    for(int l = 0; l < LINHA; l++){
        for(int c = 0; c < COLUNA; c++) printf("\t%d ", matriz[l][c]);
        printf("\n");
    }  
    // matriz++;
    // printf("\nprimeiro elemento da segunda linha: %d", matriz[0][0]);

    // matriz[0] += 4;
    // printf("\nsegundo elemento da segunda coluna da segunda linha: %d", matriz[0][0]);
 
    //Libera as linhas da matriz
    // for(int i; i < LINHA; i++){
    //     free(matriz[i]);
    // }
    //Libera a matriz de ponteiro
    //free(matriz);
}

void gerarMacrobloco(){
    int cont = 0;
    for(int macroPorLinha = 0; macroPorLinha < MB_LINHA; macroPorLinha++){
        for(int macroPorColuna = 0; macroPorColuna < MB_COLUNA; macroPorColuna++){
            cont++;
            printf("\tMACROBLOCO: %d\n", cont);
            for(int i = 0; i < MB_LINHA; i++){
                for(int j = 0; j < MB_COLUNA; j++){
                    printf("\t%d", matriz[i][j]);
                }
                printf("\n");
                // Seguir para 1º coluna do próximo macrobloco
            }
            printf("\n");
            for(int l = 0; l < 3; l++)
                matriz[l] += MB_COLUNA;
        }

        for(int l = 0; l < 3; l++){
            // Retorna para a coluna do index 0 da matriz mãe
            matriz[l] -= MB_COLUNA * 2;
        }
        // Avança 3 linhas para baixo
        matriz += 3;
    }
}
