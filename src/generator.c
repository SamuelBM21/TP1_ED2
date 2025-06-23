#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "register.h"
#include "file_binary_tree.h"

void numeroPorExtenso(int numero, char *saida) {
    const char *unidades[] = {"", "um", "dois", "três", "quatro", "cinco", "seis", "sete", "oito", "nove"};
    const char *dezenas1[] = {"dez", "onze", "doze", "treze", "quatorze", "quinze", "dezesseis", "dezessete", "dezoito", "dezenove"};
    const char *dezenas[] = {"", "", "vinte", "trinta", "quarenta", "cinquenta", "sessenta", "setenta", "oitenta", "noventa"};
    const char *centenas[] = {"", "cento", "duzentos", "trezentos", "quatrocentos", "quinhentos",
                              "seiscentos", "setecentos", "oitocentos", "novecentos"};

    saida[0] = '\0';

    if (numero < 0 || numero > 1000000) {
        strcpy(saida, "fora do intervalo");
        return;
    }

    if (numero == 0) {
        strcpy(saida, "zero");
        return;
    }

    if (numero == 1000000) {
        strcpy(saida, "um milhão");
        return;
    }

    int milhar = numero / 1000;
    int resto = numero % 1000;

    char temp[256] = "";

    // Milhar
    if (milhar > 0) {
        if (milhar == 1) {
            strcat(temp, "mil");
        } else {
            char parteMil[100];
            numeroPorExtenso(milhar, parteMil);
            strcat(temp, parteMil);
            strcat(temp, " mil");
        }
        if (resto > 0) strcat(temp, " e ");
    }

    // Centenas, dezenas e unidades
    if (resto > 0) {
        if (resto == 100) {
            strcat(temp, "cem");
        } else {
            int c = resto / 100;
            int d = (resto % 100) / 10;
            int u = resto % 10;

            if (c > 0) {
                strcat(temp, centenas[c]);
                if (d > 0 || u > 0) strcat(temp, " e ");
            }

            if (d == 1) {
                strcat(temp, dezenas1[u]);
            } else {
                if (d > 1) {
                    strcat(temp, dezenas[d]);
                    if (u > 0) strcat(temp, " e ");
                }
                if (d != 1 && u > 0) {
                    strcat(temp, unidades[u]);
                }
            }
        }
    }

    strcpy(saida, temp);
}

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

#define BUFFER_SIZE 50   // Definindo um tamanho de buffer para que possa ser escrito no arquivo

int generateFile(long long numLines, const char *nameFile, int mode) {
    FILE *arq = fopen(nameFile, "wb");
    if (!arq) {
        printf("Erro ao abrir arquivo\n");
        return 0;
    }

    srand(time(NULL));

    long long *chaves = malloc(numLines * sizeof(long long));
    if (!chaves) {
        printf("Erro ao alocar memória para chaves\n");
        fclose(arq);
        return 0;
    }

    // Preenchendo as chaves
    for (long long i = 0; i < numLines; i++) {
        chaves[i] = i;
    }

    // Ordenação conforme o modo
    if (mode == ORDER_DESCENDING) {
        for (long long i = 0; i < numLines; i++) {
            chaves[i] = numLines - i - 1;
        }
    } else if (mode == ORDER_RANDOM) {
        shuffle(chaves, numLines);
    }

    // Aloca buffer de registros
    Registro buffer[BUFFER_SIZE];

    long long total = 0;
    while (total < numLines) {
        int quantidade = (numLines - total >= BUFFER_SIZE) ? BUFFER_SIZE : (int)(numLines - total);

        memset(buffer, 0, sizeof(buffer));  // ZERA O BUFFER COMPLETO

        for (int i = 0; i < quantidade; i++) {
            buffer[i].chave = chaves[total + i];
            buffer[i].dado1 = (long long)rand() * rand();
            snprintf(buffer[i].dado2, sizeof(buffer[i].dado2), "%lld", buffer[i].chave);
            numeroPorExtenso((int)buffer[i].chave, buffer[i].dado3);
        }

        fwrite(buffer, sizeof(Registro), quantidade, arq);
        
        total += quantidade;
    }

    free(chaves);
    fclose(arq);
    return 1;
}


int binaryTreeGenerator(int qtd) {
    FILE *arqComum;

    
    arqComum = fopen("dados3.bin", "rb");
    if (arqComum == NULL) {
        printf("Erro ao abrir o arquivo de dados.\n");
        return 0;
    }

    FILE *arqArvore = fopen("binarytree.bin", "w+b");
    if (arqArvore == NULL) {
        printf("Erro ao abrir o arquivo da árvore binária.\n");
        fclose(arqComum);
        return 0;
    }

    printf(">> Gerando arquivo de árvore binária...\n");

    Registro bufferRegistros[BUFFER_SIZE];

    int totalInseridos = 0;
    int leitura = 0;

    while (totalInseridos < qtd) {
        int restante = qtd - totalInseridos;
        int quantidade = (restante > BUFFER_SIZE) ? BUFFER_SIZE : restante;

        size_t lidos = fread(bufferRegistros, sizeof(Registro), quantidade, arqComum);
        if (lidos == 0) break;

        leitura++;

        for (size_t i = 0; i < lidos; i++) {
            RegistroArvore regArv;
            regArv.chave = bufferRegistros[i].chave;
            regArv.dado1 = bufferRegistros[i].dado1;
            strcpy(regArv.dado2, bufferRegistros[i].dado2);
            strcpy(regArv.dado3, bufferRegistros[i].dado3);
            regArv.esq = -1;
            regArv.dir = -1;

            if (insereArvBin(regArv, arqArvore)) {
                totalInseridos++;
            }

            if (totalInseridos % 10000 == 0) {
                printf("Processados: %d registros\n", totalInseridos);
            }
        }
    }

    printf("Árvore binária gerada com sucesso!\n");
    printf("Registros inseridos: %d\n", totalInseridos);

    fclose(arqComum);
    fclose(arqArvore);

    return 1;
}