#include <iostream>
#include <stdio.h>
using namespace std;

#define ITENSPAGINA 4
#define MAXTABELA 25

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
        cout << "Erro na abertura do arquivo\n"; 
        return 0;
    }
    // gera a tabela de índice das páginas
    cont = 0;
    while (fread(&x, sizeof(Registro), 4, arq) == ITENSPAGINA) {
        tabela[cont]= x[0].chave;
        cont++;
    }
 
    y.chave = 91299; // substitua pela chave que deseja procurar
    if (pesquisa(tabela, cont ,&y, arq)) {
        cout << "Registro encontrado!\n";
        cout << "Chave: " << y.chave << "\n";
    } else {
        cout << "Registro nao encontrado.\n";
    }

} 