#include <stdio.h>
#include <stdlib.h>
#include "register.h"
#include "sequential_search.h"

/*
Nome: define_page_size
Função: Definir o número de registros por página e consequentemente o número de páginas da tabela (tamanho).
Entrada: Número de registros total.
Saída: Número de registros por página.
*/

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

/*
Nome: binary_search
Função: A partir do vetor de registros (página), fazer uma busca binária, que é mais eficiente que a sequencial,
        para encontrar o registro com a chave procurada.
Entrada: Vetor de registros, tamanho desse vetor, valor da chave e endereço para retorno do registro se for encontrado.
Saída: 1 se o registro for encontrado e 0 caso contrario.
*/

int binary_search(Registro *pagina, int tamanho, long long chave, Registro *resultado, int *comp) {
    int esquerda = 0, direita = tamanho - 1;

    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;

        if (pagina[meio].chave == chave) {
            *resultado = pagina[meio];
            (*comp)++;
            return 1;           // Encontrado
        } else if (pagina[meio].chave < chave) {
            esquerda = meio + 1;
            (*comp)++;
        } else {
            direita = meio - 1;
            (*comp)++;
        }
    }

    return 0;           // Não encontrado
}

/*
Nome: search
Função: Realizar a busca, definindo em qual página a busca deve ser feita e chamando as funções para encontrar o registro procurado.
Entrada: Tabela dos valores iniciais de cada pagina, tamanho dessa tabela, número de itens de cada página, 
         endereço do registro procurado e ponteiro do arquivo.
Saída: 1 se o registro for encontrado e 0 caso contrário.
*/

int search(long long tab[], int tam, int itens_pagina,  Registro* reg, FILE *arq, int *comp) {
    
    Registro* pagina = (Registro*)malloc(itens_pagina * sizeof(Registro));   // Aloca dinamicamente o array de registros
    if (pagina == NULL) {
        printf("Erro na alocação de memória para página\n");
        exit(1);
    }

    int i, quantitens;
    long desloc;
    i = 0;
    while (i < tam && tab[i] <= reg->chave){  // Procura pela página onde o item pode se encontrar
        i++;           
        (*comp)++;
    }
    (*comp)++; //Pegando a comparação de quando não entra no while


    if (i == 0) {                       // Caso a chave desejada seja menor que a 1a chave, o item não existe no arquivo
        free(pagina);                   // Libera o vetor de registros
        return 0;
    }
    else {
        if (i < tam) quantitens = itens_pagina;          // Se não for a última página, ela estará completa  
        else {                                           // Já a ultima página pode não estar completa
            fseek (arq, 0, SEEK_END);
            quantitens = (ftell(arq)/sizeof(Registro))% itens_pagina;       // Calcula o número de registros na última pagina
            if(quantitens == 0){                                            // Se o resto foi 0, significa que a página está cheia
                quantitens = itens_pagina;
            }
        }

        desloc = (i-1)*itens_pagina*sizeof(Registro);                       // Define a posição da página desejada no arquivo
        fseek (arq, desloc, SEEK_SET);
        fread (pagina, sizeof(Registro), quantitens, arq);                  // Lê a página desejada do arquivo

        int encontrado = binary_search(pagina, quantitens, reg->chave, reg, comp);        // Pesquisa binária na página lida
        free(pagina);                                                               // Libera a memória
        return encontrado;
    }
}

