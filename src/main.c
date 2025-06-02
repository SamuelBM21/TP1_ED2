#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Para ftruncate e fileno
#include <sys/types.h>  // Para ftruncate
#include "../include/b_tree.h"
#include "../include/register.h"
#include "../include/file_binary_tree.h"
#include "../include/generator.h"
#include "../include/sequencial_search.h"

// Prototypes das funções (você pode implementá-las depois)
#define ASCENDINGFILE "dados1.bin"
#define DESCENDINGFILE "dados2.bin"
#define RANDOMFILE "dados3.bin"
#define BINARYTREEFILE "binarytree.bin"

void binaryTree();
void bTree();
void sequencialSearch();

int main () {
    int opcao;
    //generateFile(1000000, DESCENDINGFILE,ORDER_RANDOM) ? printf("Arquivo aleatório gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    //generateFile(1000000, ASCENDINGFILE,ORDER_ASCENDING) ? printf("Arquivo ascendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    //generateFile(1000000, RANDOMFILE,ORDER_DESCENDING) ? printf("Arquivo descendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    do {
        printf("\n==== Gerenciador de Arquivos ====\n");
        printf("1. Árvore binária\n");
        printf("2. Árvore B\n");
        printf("3. Busca sequencial\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                binaryTree();
                break;
            case 2:
                bTree();
                break;
            case 3:
                sequencialSearch();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);
    return 0;
}


void binaryTree() {
    printf(">> Gerando arquivo de árvore binária...\n");

    // Abre o arquivo comum para leitura
    FILE *arqComum = fopen(RANDOMFILE, "rb");
    if (arqComum == NULL) {
        printf("Erro ao abrir o arquivo comum para leitura\n");
        return;
    }

    // Abre o arquivo da árvore binária para escrita
    FILE *arqArvore = fopen(BINARYTREEFILE, "r+b");
    if (arqArvore == NULL) {
        printf("Erro ao abrir o arquivo da árvore binária\n");
        fclose(arqComum);
        return;
    }

    Registro registro;
    int registrosInseridos = 0;
    int registrosDuplicados = 0;

    printf("Lendo registros do arquivo comum e inserindo na árvore binária...\n");

    // Lê cada registro do arquivo comum e insere na árvore binária
    while (fread(&registro, sizeof(Registro), 1, arqComum) == 1) {
        // Move para o final do arquivo da árvore para inserir o novo registro
        fseek(arqArvore, 0, SEEK_END);
        registro.dir=-1;
        registro.esq=-1;
        long insertPos = ftell(arqArvore);

        // Escreve o registro na posição final (isso garante espaço)
        fwrite(&registro, sizeof(Registro), 1, arqArvore);
        fflush(arqArvore);

        fseek(arqArvore, 0, SEEK_SET);

        if (insereArvBin(registro, arqArvore)) {
            registrosInseridos++;
        } else {
            registrosDuplicados++;
            ftruncate(fileno(arqArvore), insertPos);
        }

        // Mostra progresso a cada 100000 registros
        if ((registrosInseridos + registrosDuplicados) % 10000 == 0) {
            printf("Processados: %d registros\n", registrosInseridos + registrosDuplicados);
        }
    }

    printf("Árvore binária gerada com sucesso!\n");
    printf("Registros inseridos: %d\n", registrosInseridos);
    if (registrosDuplicados > 0) {
        printf("Registros duplicados ignorados: %d\n", registrosDuplicados);
    }

    fclose(arqComum);
    fclose(arqArvore);
}

void bTree() {
    printf(">> Gerando arquivo de árvore B...\n\n");

    // Abre o arquivo da árvore binária para escrita
    FILE *arqComum = fopen(RANDOMFILE, "rb");
    if (arqComum == NULL) {
        printf("Erro ao abrir o arquivo comum para leitura\n");
        return;
    }

    Registro registro;
    TipoApontador arv;
    Inicializa(&arv);

    // Lê cada registro do arquivo comum e insere na árvore binária

    while (fread(&registro, sizeof(Registro), 1, arqComum) == 1) {
        TipoRegistro x;
        x.chave=registro.chave;

        Insere(x, &arv);
    }

    long long chavesTeste[] = {91299, 123456, 999999, 1, 500000, 750000};

    for (int i = 0; i < 6; i++) {
        TipoRegistro regPesquisa;
        regPesquisa.chave = chavesTeste[i];

        printf("Pesquisando chave %lld: ", chavesTeste[i]);

        if (Pesquisa(&regPesquisa, arv)) {
            printf("ENCONTRADA!\n");
        } else {
            printf("NÃO ENCONTRADA\n");
        }
    }
    Libera(&arv);
    fclose(arqComum);
}

void sequencialSearch() {
    printf(">> Executando busca sequencial...\n");

    long long *tabela = (long long *) malloc(MAXTABELA * sizeof(long long));
    if (tabela == NULL) {
        printf("Erro na alocação de memória para tabela\n");
        exit(1);
    }
    FILE *arq;
    Registro x[ITENSPAGINA];
    Registro y;
    int cont;

    // abre o arquivo de dados
    if ((arq = fopen(ASCENDINGFILE,"rb")) == NULL) {
        printf("Erro na abertura do arquivo\n");
        free(tabela);
    }

    // gera a tabela de índice das páginas
    cont = 0;
    while (fread(x, sizeof(Registro), ITENSPAGINA, arq) > 0) {
        tabela[cont] = x[0].chave;
        cont++;
    }

    fseek(arq, 0, SEEK_SET);
    y.chave = 91299; // substitua pela chave que deseja procurar
    if (pesquisa(tabela, cont, &y, arq)) {
        printf("Registro encontrado!\n");
        printf("Chave: %lld\n", y.chave);
    } else {
        printf("Registro nao encontrado.\n");
    }

    free(tabela);
    fclose(arq);
}