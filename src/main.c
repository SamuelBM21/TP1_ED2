#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../include/b_tree.h"
#include "../include/register.h"
#include "../include/file_binary_tree.h"
#include "../include/generator.h"
#include "../include/sequential_search.h"
#include "../include/b_star_tree.h"

#define ASCENDINGFILE "dados1.bin"
#define DESCENDINGFILE "dados2.bin"
#define RANDOMFILE "dados3.bin"
#define BINARYTREEFILE "binarytree.bin"

void binaryTree(int, int, long long chave, char flag[]);
void bTree(int, int, long long chave, char flag[]);
void sequentialSearch(int, int , long long chave, char flag[]);
void bStarTree(int qtd, int situ, long long chave, char flag[]);

int main (int argc, char* argv[]) {

    clock_t inicio, fim; // variaveis para saber o tempo de execução total do código
    inicio = clock();

    if (argc != 5 && argc != 6){                    
        printf("Não foi passado o número mínimo de parâmetros. \n");
        return 0;
    }
    // Converte os argumentos para inteiros
    int metodo = atoi(argv[1]);    
    int quantidade = atoi(argv[2]);  

    if(quantidade > 1000000 ||quantidade < 0){
        printf("ERRO: A quantidade de Registros considerados deve ser positiva e menor que 1 milhão\n");
        return 0;
    }
    
    int situacao = atoi(argv[3]);
    long long chave = atoll(argv[4]);


    char flag[10] = "";
    if (argc == 6){
        strcpy(flag,argv[5]);
        if(strcmp(flag,"-p") != 0){
            printf("Esta flag '%s' não existe. \n", flag);
            return 0;
        }

    }
    
    // generateFile(1000000, RANDOMFILE,ORDER_RANDOM) ? printf("Arquivo aleatório gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    // generateFile(1000000, ASCENDINGFILE,ORDER_ASCENDING) ? printf("Arquivo ascendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    // generateFile(1000000, DESCENDINGFILE,ORDER_DESCENDING) ? printf("Arquivo descendente gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");
    // binaryTreeGenerator(1000000) ? printf("Arquivo da Arvore Binária gerado com sucesso\n") : printf("Falha ao gerar arquivo\n");

    switch (metodo) {                                           // Qual o método de pesquisa escolhido 
        case 1:
            sequentialSearch(quantidade,situacao, chave, flag);       // Busca sequencial
            break;
        case 2:
            binaryTree(quantidade,situacao, chave, flag);             // Árvore binária
            break;
        case 3:
            bTree(quantidade, situacao, chave, flag);                 // Árvore B
            break;
        case 4:

        bStarTree(quantidade, situacao, chave, flag);             // Árvore B*
            break;
        default:
            printf("Opção inválida. Tente novamente.\n");
    }

    fim = clock();

    printf("\nTempo de execução: %.6f\n\n",((double)(fim - inicio)) / CLOCKS_PER_SEC);

    return 0;
}


/*
Nome: bStarTree
Função: Criar a árvore b* com os valores do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void bStarTree(int qtd, int situ, long long chave, char flag[]) {
    printf(">> Criando a árvore B*...\n\n");

    // Ponteiro para o arquivo de registros de entrada
    FILE *arqComum;

    // Determina qual arquivo abrir, de acordo com a situação (ordenado ascendente,
    // descendente ou aleatório)
    const char *nomeArquivo;
    switch (situ) {
        case 1: nomeArquivo = ASCENDINGFILE;  break;  
        case 2: nomeArquivo = DESCENDINGFILE; break; 
        case 3: nomeArquivo = RANDOMFILE;    break; 
        default:
            printf("Opção de situação de arquivo inválida.\n");
            return;
    }

    // Abre o arquivo em modo binário para leitura
    arqComum = fopen(nomeArquivo, "rb");
    if (arqComum == NULL) {
        printf("Erro ao abrir o arquivo para leitura\n");
        return;
    }

    clock_t inicio_indice, fim_indice; // Variaveis para saber o tempo de criação dos indices
    inicio_indice = clock();

    const int LEITURA_BLOCO = 50; // Quantidade de registros lida por vez do disco

    // Buffer dinâmico para armazenar temporariamente os registros lidos
    Registro *buffer = malloc(LEITURA_BLOCO * sizeof(Registro));
    if (buffer == NULL) {
        printf("Falha na alocação de memória para o buffer\n");
        fclose(arqComum);
        return;
    }

    int registrosRestantes = qtd;  // Quantos registros ainda precisamos processar
    long totalLidos = 0;    // Contador total de registros lidos até o momento
    long comp_ins = 0;

    // 1) Inicializa uma nova árvore B* vazia
    ApontaPaginaStar arv;
    InicializaStar(&arv);

    // 2) Lê do arquivo até preencher o bloco ou esgotar os registros restantes
    while (registrosRestantes > 0) {
        // Calcula quantos registros ler neste ciclo
        long quantidadeParaLer = (registrosRestantes < LEITURA_BLOCO) ? registrosRestantes : LEITURA_BLOCO;
            
        // Lê do disco para o buffer
        long lidos = fread(buffer, sizeof(Registro), quantidadeParaLer, arqComum);
        if (lidos == 0) {
            // Fim de arquivo ou erro de leitura
            break;
        }

        // Insere cada registro lido na árvore temporária
        for (long i = 0; i < lidos; i++) {
            InsereStar(buffer[i], &arv, &comp_ins);
            totalLidos++;
            registrosRestantes--;
        }

        // Se leu menos que o solicitado, chegou ao final do arquivo
        if (lidos < quantidadeParaLer) {
            break;
        }
    }

    fim_indice = clock();

    printf("Arvore B* criada com sucesso!\n");

    printf("Numero de comparações na criação: %ld\n", comp_ins);
    printf("Tempo de criação do índice: %.6f\n",((double)(fim_indice - inicio_indice)) / CLOCKS_PER_SEC);
    printf("Número de transferências: %d\n", (int)ceil(qtd / (double)LEITURA_BLOCO));

    // Informando ao Usuario
    printf("\nBuscando na árvore B*\n");

    int comp = 0; // Usado para guardar o numero de comparações

    // Estrutura usada para pesquisa, inicializada apenas com a chave desejada
    Registro regPesquisa;
    memset(&regPesquisa, 0, sizeof(regPesquisa));
    regPesquisa.chave = chave;

    clock_t inicio, fim;
    inicio = clock();

    // 3) Executa a pesquisa na árvore do bloco
    if (PesquisaStar(&regPesquisa, arv, &comp)) {
        printf("Registro encontrado!\n");
        // Se a flag "-p" estiver ativa, exibe o conteúdo completo
        if (strcmp(flag, "-p") == 0) {
            printf("Chave: %lld\n", regPesquisa.chave);
            printf("Dado 1: %lld\n", regPesquisa.dado1);
            printf("Dado 2: %s\n", regPesquisa.dado2);
            printf("Dado 3: %s\n", regPesquisa.dado3);
        }
    } else {
        // Se percorreu tudo sem encontrar, avisa o usuário
        printf("Registro não encontrado após percorrer %ld registros.\n", totalLidos);
    }

    fim = clock();

    printf("\nNúmero de comparações: %d\n",comp);
    printf("Número de transferências: %d\n", 0);
    printf("Tempo de pesquisa: %.6lf\n",((double)(fim - inicio)) / CLOCKS_PER_SEC); 
     
    // Limpeza final: libera buffer, fecha arquivo e libera a arvore
    LiberaStar(&arv);
    free(buffer);
    fclose(arqComum);
}



/*
Nome: binaryTree
Função: Criar o arquivo com a árvore binária a partir dos registros do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void binaryTree(int qtd, int situ, long long chave, char flag[]) {
    
    // Verifica qual é a situação do arquivo que o usuario pede
    switch(situ){
        case 1: printf("Arvore Binaria só pode ser feita com o arquivo aleatório"); return;
        case 2: printf("Arvore Binaria só pode ser feita com o arquivo aleatório"); return;
        case 3: break;
        default: printf("Opção de situação de arquivo inválida.\n"); return;
    }
    
    // abre o arquivo da arvore binaria
    FILE *arqArvore = fopen(BINARYTREEFILE, "rb");
    if (arqArvore == NULL) {
        printf("Erro ao abrir o arquivo da árvore binária!\n");
        return;
    }


    RegistroArvore registroBusca;
    int comp = 0;  // Numero de comparações na busca
    int leitura = 0; // Numero de Leituras na busca

    clock_t inicio, fim;
    inicio = clock();

    printf("\nProcurando %lld na árvore binária...\n", chave);
    if (searchTreeBinary(chave, &registroBusca, &comp, arqArvore, qtd, &leitura)) {
        printf("Registro encontrado!\n");
        if (strcmp(flag, "") != 0) {
            printf("Chave: %lld\n", registroBusca.chave);
            printf("Dado 1: %lld\n", registroBusca.dado1);
            printf("Dado 2: %s\n", registroBusca.dado2);
            printf("Dado 3: %s\n", registroBusca.dado3);
        }
    } else {
        printf("Registro não encontrado.\n");
    }

    fim = clock();

    printf("\nNúmero de comparações: %d\n", comp);
    printf("Número de transferências: %d\n", leitura);
    printf("Tempo de criação do índice: %d\n",0);
    printf("Tempo de pesquisa: %.6lf\n",((double)(fim - inicio)) / CLOCKS_PER_SEC);
    
    fclose(arqArvore);
}


/*
Nome: bTree
Função: Criar a árvore b com os valores do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void bTree(int qtd, int situ, long long chave, char flag[]) {
    printf(">> Buscando em árvore B por blocos...\n\n");

    // Ponteiro para o arquivo de dados original
    FILE *arqComum;

    // Seleção do nome do arquivo conforme a situação (ordenado ascendente,
    // descendente ou aleatório)
    const char *nomeArquivo;
    switch (situ) {
        case 1: nomeArquivo = ASCENDINGFILE;  break;
        case 2: nomeArquivo = DESCENDINGFILE; break;
        case 3: nomeArquivo = RANDOMFILE;    break;
        default:
            printf("Opção de situação de arquivo inválida.\n");
            return;
    }

    // Abre o arquivo em modo binário para leitura
    arqComum = fopen(nomeArquivo, "rb");
    if (!arqComum) {
        printf("Erro ao abrir o arquivo para leitura\n");
        return;
    }

    clock_t inicio_indice, fim_indice; // Variaveis para saber o tempo de criação dos indices
    inicio_indice = clock();

    // LEITURA_BLOCO = quantos registros lemos de cada vez do disco
    const int LEITURA_BLOCO = 50;

    // Buffer para leitura temporária de registros
    Registro *buffer = malloc(LEITURA_BLOCO * sizeof(Registro));
    if (!buffer) {
        printf("Falha na alocação de buffer\n");
        fclose(arqComum);
        return;
    }

    long totalLidos = 0;           // Contagem total de registros já lidos
    int registrosRestantes = qtd;  // Quantos ainda faltam processar

    
    // 1) Inicializa uma nova árvore B vazia
    ApontaPagina arv;
    Inicializa(&arv);

    // 2) Preenche a árvore com até BLOCO registros, lendo
    //    em pedaços de LEITURA_BLOCO do arquivo
    while (registrosRestantes > 0) {
        // Calcula quantos ler neste pedaço
        int paraLer = (registrosRestantes < LEITURA_BLOCO) ? registrosRestantes : LEITURA_BLOCO;

        // Tenta ler do arquivo
        long lidos = fread(buffer, sizeof(Registro), paraLer, arqComum);
        if (lidos <= 0) {
            // Fim de arquivo ou erro de leitura
            break;
        }

        // Insere cada registro lido na árvore
        for (int i = 0; i < lidos; i++) {
            Insere(buffer[i], &arv);
            totalLidos++;
            registrosRestantes--;
        }
    }

    fim_indice = clock();

    // Exibe faixa de registros que compõem esta árvore
    printf("Buscando na árvore B \n");

    // Prepara struct de busca apenas com a chave desejada
    Registro regPesquisa;
    regPesquisa.chave = chave;

    int comp = 0;                   // Contador de Comparações

    clock_t inicio, fim;
    inicio = clock();

    // 3) Pesquisa na árvore
    if (Pesquisa(&regPesquisa, arv, &comp)) {
        printf("Registro encontrado!\n");
        // Se a flag “-p” foi passada, imprime detalhes do registro
        if (strcmp(flag, "-p") == 0) {
            printf("Chave: %lld\n", regPesquisa.chave);
            printf("Dado 1: %lld\n", regPesquisa.dado1);
            printf("Dado 2: %s\n", regPesquisa.dado2);
            printf("Dado 3: %s\n", regPesquisa.dado3);
        }
    } else {
        // Se percorreu tudo sem encontrar, avisa
        printf("Registro não encontrado após percorrer %ld registros.\n", totalLidos);
    }


    fim = clock();

    printf("\nNúmero de comparações: %d\n",comp);
    printf("Número de transferências: %d\n", (int)ceil(qtd / (double)LEITURA_BLOCO));
    printf("Tempo de criação do índice: %.6f\n",((double)(fim_indice - inicio_indice)) / CLOCKS_PER_SEC);
    printf("Tempo de pesquisa: %.6lf\n",((double)(fim - inicio)) / CLOCKS_PER_SEC);


    // Limpeza final
    Libera(&arv);
    free(buffer);
    fclose(arqComum);
}


/*
Nome: sequentialSearch
Função: Criar a tabela com os valores iniciais das páginas do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void sequentialSearch(int qtd, int situ, long long chave, char flag[]) {
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
            printf("Esse método só funciona para arquivos ordenados descendentemente.\n");
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
    
    int comp = 0;  // Contador de comparações
    
    clock_t inicio, fim;
    inicio = clock();

    if (search(tabela, cont, itens_pagina, &y, arqComum, &comp)) {     // Se a busca for bem sucedida
        printf("Registro encontrado!\n");
        if (strcmp(flag,"") != 0){
            printf("Chave: %lld\n", y.chave);
            printf("Dado 1: %lld\n", y.dado1);
            printf("Dado 2: %s\n", y.dado2);
            printf("Dado 3: %s\n", y.dado3);
        }
    } else {                                                    // Se não for bem sucedida
        printf("Registro não encontrado!\n");
    }

    fim = clock();

    printf("\nNúmero de comparações: %d\n",comp);
    printf("Número de transferências: %d\n", max_paginas);
    printf("Tempo de criação do índice: %d\n",0);
    printf("Tempo de pesquisa: %.6lf\n",((double)(fim - inicio)) / CLOCKS_PER_SEC);

    free(tabela);               // Libera a tabela
    fclose(arqComum);           // Fecha o arquivo
}