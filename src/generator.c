#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "register.h"

/*
Nome: shuffle
Função: Embaralhar vetor de chaves. 
Entrada: Vetor de chaves ordenado e o número de chaves.
Saída: --
*/

void shuffle(long long *array, long long n) {
    for (long long i = n - 1; i > 0; i--) {
        long long j = rand() % (i + 1);
        long long temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

/*
Nome: generateFile
Função: Gerar o arquivo binário de acordo com o tipo escolhido. 
Entrada: Número de linhas, nome do arquivo e tipo do arquivo.
Saída: 1 caso foi gerado de forma correta e 0 caso não tenha sido possível gerar.
*/

int generateFile(long long numLines, const char *nameFile, int mode) {
    FILE *arq = fopen(nameFile, "wb");              // Abre o arquivo
    if (!arq) {                                     // Se não for possível abrir
        printf("Erro ao abrir arquivo");
        return 0;
    }

    srand(time(NULL));                              // Define a semente para as funções da biblioteca rand

    Registro reg;
    long long *chaves = malloc(numLines * sizeof(long long));               // Aloca o vetor de chaves
    if (!chaves) {                                                          // Se não foi possível alocar
        printf("Erro ao alocar memória para chaves");
        fclose(arq);                                                        // Fecha o arquivo
        return 0;
    }

    for (long long i = 0; i < numLines; i++) {                              // Preenche o vetor de chaves
        chaves[i] = i;
    }

    if (mode == ORDER_DESCENDING) {                                         // Modifica as chaves conforme o modo escolhido
        for (long long i = 0; i < numLines; i++) {                          // Inverte o vetor de chaves
            chaves[i] = numLines - i - 1;
        }
    } else if (mode == ORDER_RANDOM) {
        shuffle(chaves, numLines);
    }

    for (long long i = 0; i < numLines; i++) {                              // Preenche o restante dos dados de cada registro
        reg.chave = chaves[i];
        reg.dado1 = rand() * (long long)rand();
        strcpy(reg.dado2, " ");
        strcpy(reg.dado3, " ");
        fwrite(&reg, sizeof(Registro), 1, arq);
    }

    free(chaves);                                                           // Libera o vetor com as chaves
    fclose(arq);                                                            // Fecha o arquivo
    return 1;
}
