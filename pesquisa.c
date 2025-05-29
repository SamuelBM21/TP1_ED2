#include <stdio.h>

#define ITENSPAGINA 1000
#define MAXTABELA 1000

// definição de um item do arquivo de dados
typedef struct{
    int chave;
    long long dado1;
    char dado2[1000];
    char dado3[5000];
}Registro;

int pesquisa (int tab[], int tam, Registro* reg, FILE *arq) {
    Registro pagina[ITENSPAGINA];
    int i, quantitens;
    long desloc;
    // procura pela página onde o item pode se encontrar
    i = 0;
    while (i < tam && tab[i] <= reg->chave) i++;
    // caso a chave desejada seja menor que a 1a chave, o item
    // não existe no arquivo
    if (i == 0) return 0;
    else {
        // a ultima página pode não estar completa
        if (i < tam) quantitens = ITENSPAGINA;
        else {
            fseek (arq, 0, SEEK_END);
            quantitens = (ftell(arq)/sizeof(Registro))%ITENSPAGINA;
            if(quantitens == 0){
                quantitens = ITENSPAGINA;
            }
        }

        // lê a página desejada do arquivo
        desloc = (i-1)*ITENSPAGINA*sizeof(Registro);
        fseek (arq, desloc, SEEK_SET);
        fread (&pagina, sizeof(Registro), quantitens, arq);

        // pesquisa sequencial na página lida
        for (i=0; i < quantitens; i++)
            if (pagina[i].chave == reg->chave) {
                *reg = pagina[i]; 
                return 1;
            }
        return 0;
    }
} 

int main () {
    int tabela[MAXTABELA];
    FILE *arq; 
    Registro x[ITENSPAGINA];
    Registro y;
    int cont;

    // abre o arquivo de dados
    if ((arq = fopen("dados.bin","rb")) == NULL) {
        printf("Erro na abertura do arquivo\n"); 
        return 0;
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
    while (fread(&x, sizeof(Registro), ITENSPAGINA, arq) > 0) {
        printf("%d  ", cont);
        tabela[cont]= x[0].chave;
        cont++;
    }

    fseek(arq, 0, SEEK_SET);
    y.chave = 91299; // substitua pela chave que deseja procurar
    if (pesquisa(tabela, cont ,&y, arq)) {
        printf("Registro encontrado!\n");
        printf("Chave: %d\n", y.chave);
    } else {
        printf("Registro nao encontrado.\n");
    }

} 