#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//  DIMENSÕES DA MATRIZ
#define M_LINHA 9
#define M_COLUNA 9

//  DIMENSÕES DO MACROBLOCO
#define MB_LINHA 3
#define MB_COLUNA 3

// QUANTIDADE DE MACROBLOCO 
int qtdMacrobloco = (M_LINHA * M_COLUNA) / (MB_LINHA * MB_COLUNA);

// STRUCT PARA CONTROLAR OS MACROBLOCOS [sessão critica]
typedef struct{
    int linhaInicial;
    int colunaInicial;
    int linhaFim;
    int colunaFim;
}IndexMacrobloco;

//  VETOR DE STRUCT PARA CONTROLAR OS MACROBLOCOS [sessão critica]
IndexMacrobloco *vetIndexMacro;

//  CONTROLA O PRÓXIMO MACROBLOCO QUE SERÁ ACESSADO PELA THREAD [sessão critica]
int macroAtual = 0;

//  matriz de dois ponteiros, uma para linha e outro para coluna, REF página 114 do livro 1 de C Giraldelli [sessão critica(talvez)]
int **matriz;

void gerarMatriz();
void gerarMacrobloco();

// funções de auxilio [DELETAR ao final do projeto]
void printIndexMacroElemento();
void printMacroblocos();

int main(){
    gerarMatriz();
    printf("\nqtd macrobloco: %d\n", qtdMacrobloco);
    gerarMacrobloco();

    //DELETAR AO FINAL DO TRABALHO
    //printIndexMacroElemento();
    printMacroblocos();

    return 0;
}

void gerarMatriz(){
    //  Semente: força o mesmo conjunto de números
    srand(2000);

    //  Aloca espaço de memória para as linhas
    matriz = calloc(M_LINHA, sizeof(int*));

    //  Aloca espaço de memória para as coluas de cada linha
    for(int i = 0; i < M_COLUNA; i++) matriz[i] = calloc(M_COLUNA, sizeof(int));

    //  Loop para adicionar os valores aleatórios na matriz
    for(int l = 0; l < M_LINHA; l++)
        for(int c = 0; c < M_COLUNA; c++) 
            matriz[l][c] = rand() % 31999;  

    //  Loop para imprimir a matriz
    for(int l = 0; l < M_LINHA; l++){
        for(int c = 0; c < M_COLUNA; c++) printf("\t%d ", matriz[l][c]);
        printf("\n");
    }  
}

void gerarMacrobloco(){
    //  Quantidade de macrobloco por linha e coluna da matriz
    int macroblocoPorLinha = M_LINHA / MB_LINHA;
    int macroblocoPorColuna = M_COLUNA / MB_COLUNA;
    //  Index do proximo macrobloco
    int proxMacroLinha = 0;
    int proxMacroColuna = 0;
    // Aloca espaço de memória para o vetor de struct
    vetIndexMacro = calloc(qtdMacrobloco, sizeof(IndexMacrobloco));
    
    int indexVet = 0;
    for(int i = 0; i < macroblocoPorLinha; i++){
        for(int j = 0; j < macroblocoPorColuna; j++){
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

// DELETAR AO FINAL DO TRABALHO
void printIndexMacroElemento(){
    //apenas para printar cada coordenada de cada macrobloco e seus valores
    for(int i = 0; i < qtdMacrobloco; i++){
        printf("MACROBLOCO: %d\n\n", i + 1);

        printf("linha init: %d\n", vetIndexMacro[i].linhaInicial);
        printf("coluna init: %d\n", vetIndexMacro[i].colunaInicial);
        printf("%d\n", matriz[vetIndexMacro[i].linhaInicial][vetIndexMacro[i].colunaInicial]);

        printf("linha fim: %d\n", vetIndexMacro[i].linhaFim);
        printf("coluna fim: %d\n", vetIndexMacro[i].colunaFim);
        printf("%d\n\n", matriz[vetIndexMacro[i].linhaFim][vetIndexMacro[i].colunaFim]);
    }
}

void printMacroblocos(){
    int linhaInit;
    int colunaInit;
    int linhaFim;
    int colunaFim;
    for(int i = 0; i < qtdMacrobloco; i++){
        // APENAS ORGANIZAÇÃO   
        linhaInit = vetIndexMacro[i].linhaInicial;
        colunaInit = vetIndexMacro[i].colunaInicial;
        linhaFim = vetIndexMacro[i].linhaFim;
        colunaFim = vetIndexMacro[i].colunaFim;


        printf("MACROBLOCO %d\n\n", i + 1);
        for(int linha = linhaInit; linha <= linhaFim; linha++){
            for(int coluna = colunaInit; coluna <= colunaFim ; coluna++){
                printf("%d ", matriz[linha][coluna]);
            }
            printf("\n\n");
        }
    }
}