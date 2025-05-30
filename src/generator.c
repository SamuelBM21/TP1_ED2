#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "register.h"

int generateFile(long long numLines, char nameFile[]){
    FILE *arq = fopen(nameFile,"wb");
    srand(time(NULL));

    if (!arq) {
        printf("Erro ao abrir arquivos.\n");
        return 0;
    }

    Registro reg;

    for(long long i = 0; i < numLines ; i++){
        reg.chave = i;
        reg.dado1 = rand() * (long long)rand();
        strcpy(reg.dado2, " ");
        strcpy(reg.dado3, " ");
        fwrite(&reg, sizeof(Registro), 1, arq);
    }

    fclose(arq);
    return 1;
}