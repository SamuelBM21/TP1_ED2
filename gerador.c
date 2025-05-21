#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct{
    int chave;
    long long dado1;
    char dado2[1000];
    char dado3[5000];
}Registro;

int main(){
    FILE *arq = fopen("dados.bin","wb");
    srand(time(NULL));

    if (!arq) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }
    
    int numLinhas = 1000;
    Registro reg;

    for(int i = 0; i < numLinhas ; i++){
        reg.chave = i;
        reg.dado1 = rand() * (long long)rand();
        strcpy(reg.dado2, " ");
        strcpy(reg.dado3, " ");
        fwrite(&reg, sizeof(Registro), 1, arq);
    }

    fclose(arq);
    return 0;
}