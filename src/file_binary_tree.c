#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_binary_tree.h"
#include "register.h"

/*
Nome: generateBinaryTreeFile
Função: Gerar o arquivo de árvore binária
Entrada: String com o nome do arquivo
Saída: 1 para sucesso e 0 para erro
*/

int generateBinaryTreeFile(char fileName[]){
    FILE *arq = fopen(fileName,"wb");                          // Abre o arquivo
    srand(time(NULL));

    if (!arq) {                                                // Trata o caso de erro
        ferror(arq);
        return 0;
    }
    fclose(arq);
    return 1;
}


/*
Nome: insereArvBin
Função: Inserir os arquivos na árvore binária
Entrada: Registro para ser inserido e arquivo onde vai ser inserido
Saída: 1 para sucesso e 0 para erro
*/

int insereArvBin(RegistroArvore inserido, FILE* arq){
    RegistroArvore* atual = (RegistroArvore*) malloc(sizeof(RegistroArvore));             // Aloca um registro
    long long posicaoinserido = ftell(arq);                             // Posição no arquivo onde o registro será inserido
    fseek(arq, 0, SEEK_SET);                                            // Volta o ponteiro pro início do arquivo

    if (posicaoinserido == 0){                                          // Caso seja o primeiro registro
        inserido.esq = inserido.dir = -1;                               // Inicia os apontadores do registro
        fwrite(&inserido, sizeof(RegistroArvore), 1, arq);                    // Escreve o registro no arquivo
        free(atual);
        return 1;                                                       // Retorna 1
    }

    long long posicaoatual = 0;

    while (1){
        posicaoatual = ftell(arq);                                      // Posição atual do ponteiro do pai no arquivo
        fread(atual, sizeof(RegistroArvore), 1, arq);                         
        if (inserido.chave > atual->chave){                             // Se o registro que vai ser inserido é maior que o atual
            if (atual->dir == -1){                                      // Caso o nó a direita esteja vazio
                inserido.dir = inserido.esq = -1;                       // Inicia os apontadores do nó que vai ser inserido
                atual->dir = posicaoinserido;                           // O atual aponta pra posição onde o novo registro será inserido 
                fseek(arq, posicaoatual, 0);                            // Volta o ponteiro pra posição atual inicial
                fwrite(atual, sizeof(RegistroArvore), 1, arq);                // Reescreve o pai na sua mesma posição com os apontadores atualizados
                fseek(arq, posicaoinserido, SEEK_SET);                  // Volta o ponteiro pra posição onde o novo registro será inserido
                fwrite(&inserido, sizeof(RegistroArvore), 1, arq);            // Escreve o registro no arquivo
                free(atual);                                            // Libera a memória do atual
                return 1;
            }
            fseek(arq, atual->dir* sizeof(RegistroArvore), 0);                // Move o ponteiro pra direita
            continue;
        }
        if (inserido.chave < atual->chave){                             // Se o registro que vai ser inserido é menor que o atual
            if (atual->esq == -1){                                      // Caso o nó a esquerda esteja vazio  
                inserido.dir = inserido.esq = -1;                       // Inicia os apontadores do nó que vai ser inserido
                atual->esq = posicaoinserido;                           // O atual aponta pra posição onde o novo registro será inserido
                fseek(arq, posicaoatual, 0);                            // Volta o ponteiro pra posição atual inicial
                fwrite(atual, sizeof(RegistroArvore), 1, arq);                // Reescreve o pai na sua mesma posição com os apontadores atualizados
                fseek(arq, posicaoinserido, SEEK_SET);                  // Volta o ponteiro pra posição onde o novo registro será inserido
                fwrite(&inserido, sizeof(RegistroArvore), 1, arq);            // Escreve o registro no arquivo
                free(atual);                                            // Libera a memória do atual
                return 1;
            }
            fseek(arq, atual->esq * sizeof(RegistroArvore), 0);               // Move o ponteiro pra esquerda
            continue;
        }
        if (inserido.chave == atual->chave){                            // Se for igual não insere
            free(atual);
            return 0;                                                   // Retorna 0 pois não inseriu
        }
    }
    return 0;                                                           // Retorna 0 pois não inseriu 
}

/*
Nome: searchTreeBinary
Função: Procurar por um registro na árvore binária. 
Entrada: Chave a ser procurada e um ponteiro para registro.
Saída: 1 se a chave estiver na árvore e 0 se a chave não tiver na árvore
*/
int searchTreeBinary(long long chave, RegistroArvore *registro){      
    FILE *a = fopen("binarytree.bin", "rb");                // Abre o arquivo da árvore binária
    if(a == NULL){                                          // Se ocorrer erro na abertura
        printf("Erro ao abrir o arquivo!\n");
        return 0;
    }
    RegistroArvore reg;                                           // Registro temporário para comparar
    while(fread(&reg, sizeof(RegistroArvore), 1, a) == 1){        // Enquanto houver registros para ler
        if(reg.chave == chave){                             // Se encotrou a chave retorna o registro por referência fecha o arquivo e retorna 1
            *registro = reg;
            fclose(a);
            return 1;
        }else if(reg.chave < chave){                        // Se a chave for maior que a chave do registro devemos ir pra direita na árvore binária
            fseek(a, reg.dir * sizeof(RegistroArvore), 0);
        }else{                                              // Se a chave for menor que a chave do registro devemos ir pra esquerda na árvore binária
            fseek(a, reg.esq * sizeof(RegistroArvore), 0);
        }
    }
    fclose(a);
    return 0;                                               // Se chegou aqui retorna 0 pois o registro procurado não está na árvore
}
