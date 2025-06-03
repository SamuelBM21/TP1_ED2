#include <stdio.h>
#include <stdlib.h>
#include "register.h"
#include "sequencial_search.h"

int define_page_size(int total_registros) {
    if (total_registros <= 100)
        return 10;
    else if (total_registros <= 1000)
        return 50;
    else if (total_registros <= 10000)
        return 100;
    else if (total_registros <= 100000)
        return 500;
    else
        return 1000;
}

int binary_search(Registro *pagina, int tamanho, long long chave, Registro *resultado) {
    int esquerda = 0, direita = tamanho - 1;

    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;

        if (pagina[meio].chave == chave) {
            *resultado = pagina[meio];
            return 1; // Encontrado
        } else if (pagina[meio].chave < chave) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }

    return 0; // Não encontrado
}

int pesquisa (long long tab[], int tam, int itens_pagina,  Registro* reg, FILE *arq) {
    
    // Aloca dinamicamente o array de páginas
    Registro* pagina = (Registro*)malloc(itens_pagina * sizeof(Registro));
    if (pagina == NULL) {
        printf("Erro na alocação de memória para página\n");
        exit(1);
    }

    int i, quantitens;
    long desloc;
    // procura pela página onde o item pode se encontrar
    i = 0;
    while (i < tam && tab[i] <= reg->chave) i++;
    // caso a chave desejada seja menor que a 1a chave, o item
    // não existe no arquivo
    if (i == 0) {
        free(pagina);
        return 0;
    }
    else {
        // a ultima página pode não estar completa
        if (i < tam) quantitens = itens_pagina;
        else {
            fseek (arq, 0, SEEK_END);
            quantitens = (ftell(arq)/sizeof(Registro))% itens_pagina;
            if(quantitens == 0){
                quantitens = itens_pagina;
            }
        }

        // lê a página desejada do arquivo
        desloc = (i-1)*itens_pagina*sizeof(Registro);
        fseek (arq, desloc, SEEK_SET);
        fread (pagina, sizeof(Registro), quantitens, arq);

        // pesquisa sequencial na página lida
        // pesquisa binária na página lida
        int encontrado = binary_search(pagina, quantitens, reg->chave, reg);
        free(pagina);
        return encontrado;
    }
}

