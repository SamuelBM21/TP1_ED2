#include <stdio.h>
#include "../include/b_tree.h"
#include "../include/register.h"
#include "../include/file_binary_tree.h"
#include "../include/generator.h"
#include "../include/sequencial_search.h"

// Prototypes das funções (você pode implementá-las depois)
#define COMMONFILE "dados.bin"
#define BINARYTREEFILE "filebinarytree.bin"

void generateCommonFile();
void generateFileBinaryTree();
void generateBTree();
void sequencialSearch();

int main () {
    int opcao;

    do {
        printf("\n==== Gerenciador de Arquivos ====\n");
        printf("1. Gerar arquivo comum\n");
        printf("2. Gerar arquivo de árvore binária\n");
        printf("3. Gerar árvore B\n");
        printf("4. Busca sequencial em arquivo comum\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                generateCommonFile();
                break;
            case 2:
                generateFileBinaryTree();
                break;
            case 3:
                generateBTree();
                break;
            case 4:
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

void generateCommonFile() {
    printf(">> Gerando arquivo comum...\n");
    generateFile(1000000, COMMONFILE) ? printf("Arquivo gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
}

void generateFileBinaryTree() {
    printf(">> Gerando arquivo de árvore binária...\n");

    // Cria o arquivo da árvore binária
    if (!generateBinaryTreeFile(BINARYTREEFILE)) {
        printf("\nNão foi possível gerar o arquivo\n");
        return;
    }

    // Abre o arquivo comum para leitura
    FILE *arqComum = fopen(COMMONFILE, "rb");
    if (arqComum == NULL) {
        printf("Erro ao abrir o arquivo comum para leitura\n");
        return;
    }

    // Abre o arquivo da árvore binária para escrita
    FILE *arqArvore = fopen(BINARYTREEFILE, "wb");
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

        if (insereArvBin(registro, arqArvore)) {
            registrosInseridos++;
        } else {
            registrosDuplicados++;
        }

        // Mostra progresso a cada 100000 registros
        if ((registrosInseridos + registrosDuplicados) % 10 == 0) {
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

void generateBTree() {
    printf(">> Gerando arquivo de árvore B...\n\n");

    // Abre o arquivo da árvore binária para escrita
    FILE *arqComum = fopen(COMMONFILE, "rb");
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
    if ((arq = fopen(COMMONFILE,"rb")) == NULL) {
        printf("Erro na abertura do arquivo\n");
        free(tabela);
    }

    /*   long file_size = ftell(arq);
      long expected_size = 1000000 * sizeof(Registro);
      printf("Tamanho do arquivo: %ld bytes (esperado: %ld)\n", file_size, expected_size);
      if (file_size != expected_size) {
          printf("ERRO: Tamanho do arquivo incorreto!\n");
          return 1;
      } */

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