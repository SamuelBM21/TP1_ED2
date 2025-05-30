#include <stdio.h>
#include <stdlib.h>
#include "register.h"
#include "sequencial_search.h"

int pesquisa (long long tab[], int tam, Registro* reg, FILE *arq) {
    // Aloca dinamicamente o array de páginas
    Registro* pagina = (Registro*)malloc(ITENSPAGINA * sizeof(Registro));
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
        if (i < tam) quantitens = ITENSPAGINA;
        else {
            fseek (arq, 0, SEEK_END);
            quantitens = (ftell(arq)/sizeof(Registro))%ITENSPAGINA;
            if(quantitens == 0){
                quantitens = ITENSPAGINA;
            }
        }

        // lê a página desejada do arquivo
        desloc = (i-1)*ITENSPAGINA*sizeof(Registro);
        fseek (arq, desloc, SEEK_SET);
        fread (pagina, sizeof(Registro), quantitens, arq);

        // pesquisa sequencial na página lida
        for (i=0; i < quantitens; i++)
            if (pagina[i].chave == reg->chave) {
                *reg = pagina[i];
                free(pagina);
                return 1;
            }

        free(pagina);
        return 0;
    }
}

