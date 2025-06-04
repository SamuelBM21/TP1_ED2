#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>  // Para ftruncate e fileno
//#include <sys/types.h>  // Para ftruncate
#include "../include/b_tree.h"
#include "../include/register.h"
#include "../include/file_binary_tree.h"
#include "../include/generator.h"
#include "../include/sequential_search.h"

// Prototypes das funções (você pode implementá-las depois)
#define ASCENDINGFILE "dados1.bin"
#define DESCENDINGFILE "dados2.bin"
#define RANDOMFILE "dados3.bin"
#define BINARYTREEFILE "binarytree.bin"

void binaryTree(int, int, long long chave);
void bTree(int, int, long long chave);
void sequentialSearch(int, int , long long chave);

int main (int argc, char* argv[]) {
    if (argc != 5 && argc != 6)
        printf("Não foi passado o número mínimo de parâmetros. \n");
    int metodo = atoi(argv[1]);  // converte para int
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    long long chave = atoi(argv[4]);
    //generateFile(1000000, DESCENDINGFILE,ORDER_RANDOM) ? printf("Arquivo aleatório gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    //generateFile(1000000, ASCENDINGFILE,ORDER_ASCENDING) ? printf("Arquivo ascendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    //generateFile(1000000, RANDOMFILE,ORDER_DESCENDING) ? printf("Arquivo descendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    switch (metodo) {
        case 1:
            sequentialSearch(quantidade,situacao, chave);
            break;
        case 2:
            binaryTree(quantidade,situacao, chave);
            break;
        case 3:
            bTree(quantidade, situacao, chave);
            break;
        default:
            printf("Opção inválida. Tente novamente.\n");
    }
    return 0;
}


void binaryTree(int qtd, int situ, long long chave) {
    FILE *arqComum;
    printf(">> Gerando arquivo de árvore binária...\n");
    switch (situ) {
        case 1:
            arqComum = fopen(ASCENDINGFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        case 2:
            arqComum = fopen(DESCENDINGFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
        break;
        case 3:
            arqComum = fopen(RANDOMFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        default:
            printf("Opção de situacao de arquivo inválida.\n");
            return;
    }

    // Abre o arquivo da árvore binária para escrita
    FILE *arqArvore = fopen(BINARYTREEFILE, "w+b");
    if (arqArvore == NULL) {
        printf("Erro ao abrir o arquivo da árvore binária\n");
        fclose(arqComum);
        return;
    }

    Registro registro;
    int registrosInseridos = 0;
    int registrosDuplicados = 0;

    printf("Lendo registros do arquivo e inserindo na árvore binária...\n");

    // Lê cada registro do arquivo comum e insere na árvore binária
    while ((fread(&registro, sizeof(Registro), 1, arqComum) == 1) && registrosInseridos < qtd) {
        // Move para o final do arquivo da árvore para inserir o novo registro
        fseek(arqArvore, 0, SEEK_END);
        registro.dir=-1;
        registro.esq=-1;
        //long insertPos = ftell(arqArvore);

        // Escreve o registro na posição final (isso garante espaço)
        fwrite(&registro, sizeof(Registro), 1, arqArvore);
        fflush(arqArvore);

        fseek(arqArvore, 0, SEEK_SET);

        if (insereArvBin(registro, arqArvore)) {
            registrosInseridos++;
        } else {
            registrosDuplicados++;
            //ftruncate(fileno(arqArvore), insertPos);
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
    
    printf("Procurando %lld na arvore binária\n", chave);
    if(searchTreeBinary(chave, &registro))
        printf("Registro encontrado: %lld\n", registro.chave);
    else   
        printf("Registro não encontrado\n");
    fclose(arqComum);
    fclose(arqArvore);
}

void bTree(int qtd, int situ, long long chave) {
    printf(">> Gerando arquivo de árvore B...\n\n");
    FILE *arqComum;
    // Abre o arquivo da árvore binária para escrita
    switch (situ) {
        case 1:
            arqComum = fopen(ASCENDINGFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        case 2:
            arqComum = fopen(DESCENDINGFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
        break;
        case 3:
            arqComum = fopen(RANDOMFILE, "rb");
            if (arqComum == NULL) {
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        default:
            printf("Opção de situacao de arquivo inválida.\n");
            return;
    }

    int cont = 0;
    Registro registro;
    TipoApontador arv;
    Inicializa(&arv);
    
    // Lê cada registro do arquivo comum e insere na árvore binária

    while ((fread(&registro, sizeof(Registro), 1, arqComum) == 1) && cont < qtd) {
        TipoRegistro x;
        x.chave=registro.chave;

        Insere(x, &arv);
        cont++;
    }

    TipoRegistro regPesquisa;
    regPesquisa.chave = chave;

    printf("Pesquisando chave %lld: ", chave);

    if (Pesquisa(&regPesquisa, arv)) {
        printf("ENCONTRADA!\n");
    } else {
        printf("NÃO ENCONTRADA\n");
    }

    Libera(&arv);
    fclose(arqComum);
}

/*
Nome: sequentialSearch
Função: Criar a tabela com os valores iniciais das páginas do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void sequentialSearch(int qtd, int situ, long long chave) {
    printf(">> Executando busca sequencial...\n");

    int itens_pagina = define_page_size(qtd);                       // Define o número de itens por página
    int max_paginas = (qtd + itens_pagina - 1) / itens_pagina;      // Define o número de páginas a partir do valor calculado anteriormente

    long long *tabela = (long long *) malloc(max_paginas* sizeof(long long));   // Aloca a memória para a tabela

    if (tabela == NULL) {                                                       // Se ocorreu uma falha na alocação
        printf("Erro na alocação de memória para tabela\n");
        exit(1);
    }

    FILE *arqComum;
    Registro x[itens_pagina];
    Registro y;
    int cont = 0;

    switch (situ) {         // Qual a situação do arquivo
        case 1:             // Caso o arquivo desejado seja ordenado ascendentemente
            arqComum = fopen(ASCENDINGFILE, "rb");          // Abre o arquivo 
            if (arqComum == NULL) {                         // Se ocorreu algum erro na abertura
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            while ((fread(x, sizeof(Registro), itens_pagina, arqComum) > 0) && cont < max_paginas) {
                tabela[cont] = x[0].chave;                  // Preenche a tabela com os valores iniciais de cada página
                cont++;                                     // Incrementa o contador de páginas
            }
        
            break;
        case 2:             // Caso o arquivo desejado seja ordenado descendentemente
            printf("Esse método só funciona para arquivos ordenados ascendentemente.\n");
            free(tabela);   // Libera a tabela
            return;
        case 3:             // Caso o arquivo desejado seja aleatório
            printf("Esse método só funciona para arquivos ordenados.\n");
            free(tabela);   // Libera a tabela
            return;
        default:            // Caso a opção escolhida não seja possível
            printf("Opção de situacao de arquivo inválida.\n");
            free(tabela);   // Libera a tabela
            return;
    }

    fseek(arqComum, 0, SEEK_SET);       // Coloca o apontador do arquivo no início
    y.chave = chave;                    // Inicializa um registro com a chave a ser procurada
    
    if (search(tabela, cont, itens_pagina, &y, arqComum)) {     // Se a busca for bem sucedida
        printf("Registro encontrado!\n");
        printf("Chave: %lld\n", y.chave);
    } else {                                                    // Se não for bem sucedida
        printf("Registro nao encontrado.\n");
    }

    free(tabela);               // Libera a tabela
    fclose(arqComum);           // Fecha o arquivo
}