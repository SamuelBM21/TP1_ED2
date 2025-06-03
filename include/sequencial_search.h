#ifndef SEQUENCIAL_SEARCH_C_H
#define SEQUENCIAL_SEARCH_C_H

int pesquisa (long long tab[], int tam, int itens_pagina, Registro* reg, FILE *arq);
int define_page_size(int total_registros);
int binary_search(Registro *pagina, int tamanho, long long chave, Registro *resultado);
#endif //SEQUENCIAL_SEARCH_C_H
