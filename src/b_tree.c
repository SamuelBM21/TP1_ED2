#include <stdio.h>
#include <stdlib.h>
#include "../include/b_tree.h"

/*
Nome: Libera
Função: Liberar a memória alocada para a árvore. 
Entrada: Vetor de apontadores de página.
Saída: --
*/

void Libera(ApontaPagina *ap) {
    if (*ap == NULL) return;                // Se a página for nula, retorna porque chegou na folha

    for (int i = 0; i <= (*ap)->n; i++) {   // Chama recursivamente a função de liberar para cada filho
        Libera(&((*ap)->p[i]));
    }

    free(*ap);                              // Libera o Apontador
    *ap = NULL;
}

/*
Nome: Inicializa
Função: Inicializar a árvore. 
Entrada: Apontador para a página raiz.
Saída: --
*/

void Inicializa(ApontaPagina *arvore){
    *arvore=NULL;
}

/*
Nome: Pesquisa
Função: Procurar por um registro na árvore B. 
Entrada: Endereço do registro com a chave a ser procurada e apontador para a página.
Saída: --
*/

int Pesquisa(Registro *x, ApontaPagina ap, int *comp){
    long i =1;
    if(ap == NULL) return 0;                                // Se chegou em uma página folha

    while( i< ap->n && x->chave > ap->r[i-1].chave){   // Loop para encontrar qual filho fazer a verificação 
        i++;  
        (*comp)++;  
    }

    (*comp)++;
    if(x->chave == ap->r[i-1].chave){                       // Se a chave do registro for igual à procurada
        *x = ap->r[i-1];                                    // Modifica o registro, substituindo-o pelo encontrado   
        return 1;
    }

    if(x->chave < ap->r[i-1].chave) return Pesquisa(x, ap->p[i-1], comp);         // Se a chave for menor, chama a função recursivamente para o filho da esquerda de i
    else return Pesquisa(x, ap->p[i], comp);                                      // Se não, chama para a direita
}

/*
Nome: Imprime
Função: Imprime toda a árvore B.
Entrada: Apontador para a página.
Saída: --
*/

void Imprime(ApontaPagina arvore){
    int i =0;
    if(arvore==NULL) return;
    while (i<=arvore->n){
        Imprime(arvore->p[i]);                                      // Chama a função recursivamente para cada página
        if(i!=arvore->n) printf("%lld ", arvore->r[i].chave);
        i++;
    }
}

/*
Nome: InsereNaPagina
Função: Inserir um registro e um ponteiro à direita na posição correta em uma página.
Entrada: Apontador para a página, registro a ser inserido e apontador para a página da direita da inicial.
Saída: --
*/

void InsereNaPagina(ApontaPagina ap, Registro reg, ApontaPagina apDir){
    int k = ap->n;                              // Começa no último índice ocupado
    short naoAchouPosicao = (k>0);              // Se a página tiver algum registro

    while(naoAchouPosicao){
        if(reg.chave>=ap->r[k-1].chave){        // Se a posição correta do registro foi encontrada
            naoAchouPosicao=0;
            break;                              // Sai do loop
        }

        ap->r[k]=ap->r[k-1];                    // Desloca o registro para a direita
        ap->p[k+1] = ap->p[k];                  // Desloca o ponteiro correspondente
        k--;
        if(k<1) naoAchouPosicao=0;              
    }

    ap->r[k]=reg;                               // Insere o novo registro
    ap->p[k+1]=apDir;                           // Adiciona o ponteiro à direita
    ap->n++;                                    // Incrementa o número de registros na página
}

/*
Nome: Ins  
Função: Insere recursivamente um registro na árvore B. Se necessário, divide a página e propaga o registro do meio para cima.  
Entrada:  Registro a ser inserido, apontador para a raiz ou subárvore onde será feita a inserção, ponteiro para uma variável "booleana"
          (short) que indica se a árvore cresceu após a inserção,ponteiro para o registro que deve subir para o nível superior em caso de
          divisão da página e ponteiro para a nova página criada à direita em caso de divisão.  
Saída: --  
*/

void Ins(Registro reg, ApontaPagina ap, short *cresceu, Registro *regRetorno, ApontaPagina *apRetorno){
    long i=1, j;
    ApontaPagina apTemp;
    if(ap == NULL){                     // Caso base, árvore vazia ou nó folha
        *cresceu = 1;                   // A árvore cresceu
        *regRetorno=reg;                // Registro para subir
        *apRetorno=NULL;                // Sem filho à direita
        return;
    }

    while(i<ap->n && reg.chave>ap->r[i-1].chave) i++;       // Busca a posição correta para inserção

    if(reg.chave==ap->r[i-1].chave) {                       // Registro já existe, não insere
        *cresceu=0;
        return;
    }

    if(reg.chave<ap->r[i-1].chave) i--;                     // Corrige índice

    Ins(reg, ap->p[i], cresceu, regRetorno, apRetorno);     // Inserção recursiva no filho correspondente

    if(!*cresceu) return;                                   // Se não cresceu, não faz nada

    if(ap->n < MM){                                         // Se ainda há espaço na página
        InsereNaPagina(ap, *regRetorno, *apRetorno);
        *cresceu=0;
        return;
    }

    apTemp=(ApontaPagina)malloc(sizeof(Pagina));            // Já que não há espaço na página, cria uma nova e a inicia com 0
    apTemp->n=0;
    apTemp->p[0]=NULL;

    if(i<M+1){                                              // Se o novo registro for entrar na página original
        InsereNaPagina(apTemp, ap->r[MM-1], ap->p[MM]);     // Último vai pra nova página
        ap->n--;                                            // Remove o último da original
        InsereNaPagina(ap, *regRetorno, *apRetorno);        // Insere o novo registro
    }else {
        InsereNaPagina(apTemp, *regRetorno, *apRetorno);    // Vai direto na nova página
    }

    for(j=M+2; j<=MM; j++) {
        InsereNaPagina(apTemp, ap->r[j - 1], ap->p[j]);     // Move metade dos registros da página original para a nova
    }

    ap->n=M;                        // Reduz número de registros na original
    apTemp->p[0]=ap->p[M+1];        // Atualiza ponteiro inicial da nova

    *regRetorno=ap->r[M];           // Registro do meio sobe
    *apRetorno=apTemp;              // Retorna a nova página
}

/*
Nome: Insere  
Função: Função principal para inserção de um registro em uma árvore B. Chama a função recursiva Ins e, se necessário, cria uma nova raiz.  
Entrada: Registro a ser inserido, apontador para a raiz da árvore (passado por referência, pois pode ser alterado).  
Saída: --  
*/

void Insere(Registro reg, ApontaPagina *ap){
    short cresceu;
    Registro regRetorno;
    Pagina *apRetorno, *apTemp;

    Ins(reg, *ap, &cresceu, &regRetorno, &apRetorno);       // Chama inserção recursiva

    if(cresceu){                                            // Se a árvore cresceu na raiz
        apTemp = (Pagina*)malloc(sizeof(Pagina));           
        apTemp->n=1;
        apTemp->r[0]=regRetorno;                            // Registro do meio vira nova raiz
        apTemp->p[1]=apRetorno;                             // Filho direito
        apTemp->p[0]=*ap;                                   // Filho esquerdo (raiz antiga)
        *ap = apTemp;                                       // Atualiza ponteiro da raiz
    }
}