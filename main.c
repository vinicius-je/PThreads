/*
  %%%  Instituicao:     IFES
  %%%  Curso:           BSI
  %%%  Disciplina:      SO
  %%%  Tipo:            Trabalho 1
  %%%  Ano:             2022
  %%%  versao           001.002
*/

/* Declaração das bibliotecas internas*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC
#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#define NUM_THREADS 5

/* Declaração das bibliotecas externas*/
#include "main.h"
#include "func.h"

int main(int argc, char** argv)
{
    gerarMatriz();
    return 0;
}
