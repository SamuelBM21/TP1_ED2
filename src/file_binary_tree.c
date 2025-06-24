#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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

int insereArvBin(RegistroArvore inserido, FILE* arq, long *leitura, long *comp) {
    fseek(arq, 0, SEEK_END);
    long long posicaoinserido = ftell(arq) / sizeof(RegistroArvore);

    // Se o arquivo está vazio (árvore vazia)
    if (posicaoinserido == 0) {
        inserido.esq = inserido.dir = -1;
        fwrite(&inserido, sizeof(RegistroArvore), 1, arq);
        return 1;
    }

    long long posAtual = 0;
    RegistroArvore atual;

    while (1) {
        fseek(arq, posAtual * sizeof(RegistroArvore), SEEK_SET);
        fread(&atual, sizeof(RegistroArvore), 1, arq);
        (*leitura)++;

        (*comp)++;
        if (inserido.chave < atual.chave) {
            if (atual.esq == -1) {
                inserido.esq = inserido.dir = -1;
                atual.esq = posicaoinserido;

                // Atualiza o pai
                fseek(arq, posAtual * sizeof(RegistroArvore), SEEK_SET);
                fwrite(&atual, sizeof(RegistroArvore), 1, arq);

                // Insere o novo nó
                fseek(arq, 0, SEEK_END);
                fwrite(&inserido, sizeof(RegistroArvore), 1, arq);

                return 1;
            } else {
                posAtual = atual.esq;
            }
        } else{
            (*comp)++; // Comparação: inserido.chave > atual.chave
            if (inserido.chave > atual.chave) {
                if (atual.dir == -1) {
                    inserido.esq = inserido.dir = -1;
                    atual.dir = posicaoinserido;

                    // Atualiza o pai
                    fseek(arq, posAtual * sizeof(RegistroArvore), SEEK_SET);
                    fwrite(&atual, sizeof(RegistroArvore), 1, arq);

                    // Insere o novo nó
                    fseek(arq, 0, SEEK_END);
                    fwrite(&inserido, sizeof(RegistroArvore), 1, arq);

                    return 1;
                } else {
                    posAtual = atual.dir;
                }
            } else {
                (*comp)++;
                // Chave já existe, não insere
                return 0;
            }
        }
    }
}


/*
Nome: searchTreeBinary
Função: Procurar por um registro na árvore binária. 
Entrada: Chave a ser procurada e um ponteiro para registro.
Saída: 1 se a chave estiver na árvore e 0 se a chave não tiver na árvore
*/
int searchTreeBinary(long long chave, RegistroArvore *registro, int *comp, FILE *a, long long quantidade, int *leitura) {
    long long pos = 0; // Começa na raiz (posição 0)
    RegistroArvore reg; // Variável que carrega o registro atual durante a busca

    // Loop de navegação pela árvore
    while (pos < quantidade) { // Enquanto a posição estiver dentro do limite considerado
        
        // Posiciona no arquivo no registro atual
        fseek(a, pos * sizeof(RegistroArvore), SEEK_SET);
        if (fread(&reg, sizeof(RegistroArvore), 1, a) != 1) {
            (*leitura)++;
            break; // Leitura inválida
        }
        (*leitura)++;
        (*comp)++; // Incrementa o número de comparações realizadas

        if (reg.chave == chave) {
            *registro = reg; // Copia o registro encontrado para o ponteiro passado
            return 1; // Achou
        } else if (chave < reg.chave) {  // Decide se vai para a subárvore da esquerda
            if (reg.esq == -1 || reg.esq >= quantidade) break; // Se não tem filho à esquerda ou se ele está fora do limite de quantidade
            pos = reg.esq;
        } else {  // Decide se vai para a subárvore da direita
            if (reg.dir == -1 || reg.dir >= quantidade) break; // Se não tem filho à direita ou se ele está fora do limite de quantidade
            pos = reg.dir;
        }
    }

    return 0; // Não encontrado
}