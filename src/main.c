#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    return 0;
}


/*
Nome: bStarTree
Função: Criar a árvore b* com os valores do arquivo aberto, chamada da função de busca e impressão do resultado. 
Entrada: Número de registros, tipo do arquivo aberto e a chave a ser procurada.
Saída: --
*/

void bStarTree(int qtd, int situ, long long chave, char flag[]) {
    printf(">> Buscando na árvore B* por blocos...\n\n");

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

    int encontrou = 0;      // Flag que indica se a chave foi encontrada
    long totalLidos = 0;    // Contador total de registros lidos até o momento

    
    const int BLOCO = 10000;       // Máximo de registros inseridos numa única árvore temporária
    const int LEITURA_BLOCO = 1000; // Quantidade de registros lida por vez do disco

    // Buffer dinâmico para armazenar temporariamente os registros lidos
    Registro *buffer = malloc(LEITURA_BLOCO * sizeof(Registro));
    if (buffer == NULL) {
        printf("Falha na alocação de memória para o buffer\n");
        fclose(arqComum);
        return;
    }

    // Estrutura usada para pesquisa, inicializada apenas com a chave desejada
    Registro regPesquisa;
    memset(&regPesquisa, 0, sizeof(regPesquisa));
    regPesquisa.chave = chave;

    int registrosRestantes = qtd;  // Quantos registros ainda precisamos processar

    // Loop principal: constrói e testa uma árvore B* para cada bloco de até BLOCO registros
    while (!encontrou && registrosRestantes > 0) {
        // 1) Inicializa uma nova árvore B* vazia para este bloco
        ApontaPaginaStar arv;
        InicializaStar(&arv);

        int cont = 0;  // Quantos registros foram inseridos nesta árvore temporária

        // 2) Lê do arquivo até preencher o bloco ou esgotar os registros restantes
        while (cont < BLOCO && registrosRestantes > 0) {
            // Calcula quantos registros ler neste ciclo
            long quantidadeParaLer = (BLOCO - cont < LEITURA_BLOCO) ? (BLOCO - cont) : LEITURA_BLOCO;
            
            if (quantidadeParaLer > registrosRestantes) {
                quantidadeParaLer = registrosRestantes;
            }

            // Lê do disco para o buffer
            long lidos = fread(buffer, sizeof(Registro), quantidadeParaLer, arqComum);
            if (lidos == 0) {
                // Fim de arquivo ou erro de leitura
                break;
            }

            // Insere cada registro lido na árvore temporária
            for (long i = 0; i < lidos; i++) {
                InsereStar(buffer[i], &arv);
                cont++;
                totalLidos++;
                registrosRestantes--;
            }

            // Se leu menos que o solicitado, chegou ao final do arquivo
            if (lidos < quantidadeParaLer) {
                break;
            }
        }

        // Se não inseriu nenhum registro, acabou o arquivo ou não há mais dados
        if (cont == 0) {
            break;
        }

        // Informe a faixa de registros analisada no bloco atual
        printf("Buscando na árvore com registros %ld até %ld...\n",
               totalLidos - cont, totalLidos - 1);

        // 3) Executa a pesquisa na árvore do bloco
        if (PesquisaStar(&regPesquisa, arv)) {
            printf("Registro encontrado!\n");
            // Se a flag "-p" estiver ativa, exibe o conteúdo completo
            if (strcmp(flag, "-p") == 0) {
                printf("Chave: %lld\n", regPesquisa.chave);
                printf("Dado 1: %lld\n", regPesquisa.dado1);
                printf("Dado 2: %s\n", regPesquisa.dado2);
                printf("Dado 3: %s\n", regPesquisa.dado3);
            }
            encontrou = 1;
        }

        // 4) Libera os recursos da árvore antes de avançar para o próximo bloco
        LiberaStar(&arv);
    }

    // Se percorreu tudo sem encontrar, avisa o usuário
    if (!encontrou) {
        printf("Registro não encontrado após percorrer %ld registros.\n", totalLidos);
    }

    // Limpeza final: libera buffer e fecha arquivo
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
    FILE *arqComum;
    printf(">> Gerando arquivo de árvore binária...\n");

    switch (situ) {         // Qual a situação do arquivo
        case 1:             // Caso o arquivo desejado seja ordenado ascendentemente
            arqComum = fopen(ASCENDINGFILE, "rb");          // Abre o arquivo 
            if (arqComum == NULL) {                         // Se ocorreu algum erro na abertura
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        case 2:             // Caso o arquivo desejado seja ordenado descendentemente
            arqComum = fopen(DESCENDINGFILE, "rb");         // Abre o arquivo
            if (arqComum == NULL) {                         // Se ocorreu algum erro na abertura
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;

        case 3:             // Caso o arquivo desejado seja aleatório
            arqComum = fopen(RANDOMFILE, "rb");             // Abre o arquivo
            if (arqComum == NULL) {                         // Se ocorreu algum erro na abertura
                printf("Erro ao abrir o arquivo para leitura\n");
                return;
            }
            break;
        default:            // Caso a opção escolhida não seja possível
            printf("Opção de situacao de arquivo inválida.\n");
            return;
    }

    FILE *arqArvore = fopen(BINARYTREEFILE, "w+b");                     // Abre o arquivo da árvore binária para escrita e leitura
    if (arqArvore == NULL) {                                            // Se ocorreu falha na abertura
        printf("Erro ao abrir o arquivo da árvore binária\n");        
        fclose(arqComum);                                               // Fecha o arquivo
        return;
    }

    RegistroArvore registro;
    int registrosInseridos = 0;

    printf("Lendo registros do arquivo e inserindo na árvore binária...\n");

    while ((fread(&registro, sizeof(RegistroArvore), 1, arqComum) == 1) && registrosInseridos < qtd) {
        fseek(arqArvore, 0, SEEK_END);                      // Move para o final do arquivo da árvore para inserir o novo registro
        registro.dir=-1;                                    // Inicializa os ponteiros do novo registro como 0
        registro.esq=-1;

        fwrite(&registro, sizeof(RegistroArvore), 1, arqArvore);  // Escreve o registro na posição final 
        fflush(arqArvore);

        fseek(arqArvore, 0, SEEK_SET);                      // Volta o ponteiro do arquivo para o início

        if (insereArvBin(registro, arqArvore)) {            // Se conseguiu inserir na árvore
            registrosInseridos++;                           // Incrementa o número de registros inseridos
        }   

        if ((registrosInseridos) % 10000 == 0) {            // Mostra progresso a cada 10000 registros
            printf("Processados: %d registros\n", registrosInseridos );
        }
    }

    printf("Árvore binária gerada com sucesso!\n");
    printf("Registros inseridos: %d\n", registrosInseridos);

    printf("Procurando %lld na arvore binária\n", chave);
    if(searchTreeBinary(chave, &registro)){                          // Se a busca pela chave for bem sucedida
        printf("Registro encontrado!\n");
        if (strcmp(flag,"") != 0){
            printf("Chave: %lld\n", registro.chave);
            printf("Dado 1: %lld\n", registro.dado1);
            printf("Dado 2: %s\n", registro.dado2);
            printf("Dado 3: %s\n", registro.dado3);
        }
    }
    else{                                                          // Se a busca não for bem sucedida
        printf("Registro não encontrado\n");
    }
    fclose(arqComum);                       // Fecha o arquivo de registros
    fclose(arqArvore);                      // Fecha o arquivo com a árvore binária
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

    // Definições de tamanho: BLOCO = número máximo de registros por árvore,
    // LEITURA_BLOCO = quantos registros lemos de cada vez do disco
    const int BLOCO = 10000;
    const int LEITURA_BLOCO = 1000;

    // Buffer para leitura temporária de registros
    Registro *buffer = malloc(LEITURA_BLOCO * sizeof(Registro));
    if (!buffer) {
        printf("Falha na alocação de buffer\n");
        fclose(arqComum);
        return;
    }

    long totalLidos = 0;           // Contagem total de registros já lidos
    int registrosRestantes = qtd;  // Quantos ainda faltam processar
    int encontrou = 0;             // Flag: 1 se encontrar a chave

    // Prepara struct de busca apenas com a chave desejada
    Registro regPesquisa;
    regPesquisa.chave = chave;

    // Loop principal: enquanto não achar e tiver registros
    while (!encontrou && registrosRestantes > 0) {
        // 1) Inicializa uma nova árvore B vazia para este bloco
        ApontaPagina arv;
        Inicializa(&arv);

        int cont = 0;  // Quantos registros inseridos nesta árvore de bloco

        // 2) Preenche a árvore com até BLOCO registros, lendo
        //    em pedaços de LEITURA_BLOCO do arquivo
        while (cont < BLOCO && registrosRestantes > 0) {
            // Calcula quantos ler neste pedaço
            int paraLer = (BLOCO - cont < LEITURA_BLOCO) ? (BLOCO - cont) : LEITURA_BLOCO;
            if (paraLer > registrosRestantes)
                paraLer = registrosRestantes;

            // Tenta ler do arquivo
            long lidos = fread(buffer, sizeof(Registro), paraLer, arqComum);
            if (lidos <= 0) {
                // Fim de arquivo ou erro de leitura
                break;
            }

            // Insere cada registro lido na árvore
            for (int i = 0; i < lidos; i++) {
                Insere(buffer[i], &arv);
                cont++;
                totalLidos++;
                registrosRestantes--;
            }
        }

        // Se não leu nada, encerra (acabou o arquivo)
        if (cont == 0) {
            Libera(&arv);
            break;
        }

        // Exibe faixa de registros que compõem esta árvore
        printf("Buscando na árvore B com registros %ld até %ld...\n", totalLidos - cont, totalLidos - 1);

        // 3) Pesquisa na árvore deste bloco
        if (Pesquisa(&regPesquisa, arv)) {
            printf("Registro encontrado!\n");
            // Se a flag “-p” foi passada, imprime detalhes do registro
            if (strcmp(flag, "-p") == 0) {
                printf("Chave: %lld\n", regPesquisa.chave);
                printf("Dado 1: %lld\n", regPesquisa.dado1);
                printf("Dado 2: %s\n", regPesquisa.dado2);
                printf("Dado 3: %s\n", regPesquisa.dado3);
            }
            encontrou = 1;
        }

        // 4) Libera estrutura da árvore antes de passar ao próximo bloco
        Libera(&arv);
    }

    // Se percorreu tudo sem encontrar, avisa
    if (!encontrou) {
        printf("Registro não encontrado após percorrer %ld registros.\n", totalLidos);
    }

    // Limpeza final
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
    
    if (search(tabela, cont, itens_pagina, &y, arqComum)) {     // Se a busca for bem sucedida
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

    free(tabela);               // Libera a tabela
    fclose(arqComum);           // Fecha o arquivo
}