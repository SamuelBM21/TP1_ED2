#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "register.h"

void shuffle(long long *array, long long n) {
    for (long long i = n - 1; i > 0; i--) {
        long long j = rand() % (i + 1);
        long long temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int generateFile(long long numLines, const char *nameFile, int mode) {
    FILE *arq = fopen(nameFile, "wb");
    if (!arq) {
        perror("Erro ao abrir arquivo");
        return 0;
    }

    srand(time(NULL));

    Registro reg;
    long long *chaves = malloc(numLines * sizeof(long long));
    if (!chaves) {
        perror("Erro ao alocar memória para chaves");
        fclose(arq);
        return 0;
    }

    // Preenche o vetor de chaves
    for (long long i = 0; i < numLines; i++) {
        chaves[i] = i;
    }

    // Modifica as chaves conforme o modo escolhido
    if (mode == ORDER_DESCENDING) {
        for (long long i = 0; i < numLines; i++) {
            chaves[i] = numLines - i - 1;
        }
    } else if (mode == ORDER_RANDOM) {
        shuffle(chaves, numLines);
    }

    for (long long i = 0; i < numLines; i++) {
        reg.chave = chaves[i];
        reg.dado1 = rand() * (long long)rand();
        strcpy(reg.dado2, " ");
        strcpy(reg.dado3, " ");
        fwrite(&reg, sizeof(Registro), 1, arq);
    }

    free(chaves);
    fclose(arq);
    return 1;
}
