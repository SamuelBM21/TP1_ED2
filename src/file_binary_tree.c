#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_binary_tree.h"
#include "register.h"

int generateBinaryTreeFile(char fileName[]){
    FILE *arq = fopen(fileName,"wb");
    srand(time(NULL));

    if (!arq) {
        ferror(arq);
        return 0;
    }
    fclose(arq);
    return 1;
}

int insereArvBin(Registro inserido, FILE* arq){
    Registro* atual = (Registro*) malloc(sizeof(Registro));
    long long posicaoinserido = ftell(arq);
    fseek(arq, 0, SEEK_SET);

    if (posicaoinserido == 0){
        inserido.esq = inserido.dir = -1;
        fwrite(&inserido, sizeof(Registro), 1, arq);
        free(atual);
        return 1;
    }

    long long posicaoatual = 0;

    while (1){
        posicaoatual = ftell(arq);
        fread(atual, sizeof(Registro), 1, arq);
        if (inserido.chave > atual->chave){
            if (atual->dir == -1){
                inserido.dir = inserido.esq = -1;
                atual->dir = posicaoinserido;
                fseek(arq, posicaoatual, 0);
                fwrite(atual, sizeof(Registro), 1, arq);
                fseek(arq, posicaoinserido, SEEK_SET);
                fwrite(&inserido, sizeof(Registro), 1, arq);
                free(atual);
                return 1;
            }
            fseek(arq, atual->dir* sizeof(Registro), 0);
            continue;
        }
        if (inserido.chave < atual->chave){
            if (atual->esq == -1){
                inserido.dir = inserido.esq = -1;
                atual->esq = posicaoinserido;
                fseek(arq, posicaoatual, 0);
                fwrite(atual, sizeof(Registro), 1, arq);
                fseek(arq, posicaoinserido, SEEK_SET);
                fwrite(&inserido, sizeof(Registro), 1, arq);
                free(atual);
                exit(0);
                return 1;
            }
            fseek(arq, atual->esq * sizeof(Registro), 0);
            continue;
        }
        if (inserido.chave == atual->chave){
            free(atual);
            return 0;
        }
    }
    return 0;
}

int searchTreeBinary(long long chave, Registro *registro){
    FILE *a = fopen("binarytree.bin", "rb");
    if(a == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    Registro reg;
    while(fread(&reg, sizeof(Registro), 1, a) == 1){
        if(reg.chave == chave){
            *registro = reg;
            return 1;
        }else if(reg.chave < chave){
            fseek(a, reg.dir * sizeof(Registro), 0);
        }else{
            fseek(a, reg.esq * sizeof(Registro), 0);
        }
    }
    return 0;
}
