#ifndef SEQUENTIAL_SEARCH_H
#define SEQUENTIAL_SEARCH_H

#include "register.h"
#include "stdio.h"

int search(long long tab[], int tam, int itens_pagina, Registro* reg, FILE *arq);
int define_page_size(int total_registros);
int binary_search(Registro *pagina, int tamanho, long long chave, Registro *resultado);
#endif //SEQUENCIAL_SEARCH_H
