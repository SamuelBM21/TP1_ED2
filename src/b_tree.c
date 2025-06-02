#include <stdio.h>
#include <stdlib.h>
#include "../include/b_tree.h"

void Libera(TipoApontador *ap) {
    if (*ap == NULL) return;

    for (int i = 0; i <= (*ap)->n; i++) {
        Libera(&((*ap)->p[i]));
    }

    free(*ap);
    *ap = NULL;
}

void Inicializa(TipoApontador *arvore){
    *arvore=NULL;
}

int Pesquisa(TipoRegistro *x, TipoApontador ap){
    long i =1;
    if(ap == NULL) return 0;

    while( i< ap->n && x->chave >ap->r[i-1].chave) i++;

    if(x->chave == ap->r[i-1].chave){
        *x = ap->r[i-1];
        return 1;
    }

    if(x->chave < ap->r[i-1].chave) return Pesquisa(x, ap->p[i-1]);
    else return Pesquisa(x, ap->p[i]);
}

void Imprime(TipoApontador arvore){
    int i =0;
    if(arvore==NULL) return;
    while (i<=arvore->n){
        Imprime(arvore->p[i]);
        if(i!=arvore->n) printf("%ld ", arvore->r[i].chave);
        i++;
    }
}

void InsereNaPagina(TipoApontador ap, TipoRegistro reg, TipoApontador apDir){
    int k= ap->n;
    short naoAchouPosicao = (k>0);

    while(naoAchouPosicao){
        if(reg.chave>=ap->r[k-1].chave){
            naoAchouPosicao=0;
            break;
        }

        ap->r[k]=ap->r[k-1];
        ap->p[k+1] = ap->p[k];
        k--;
        if(k<1) naoAchouPosicao=0;
    }

    ap->r[k]=reg;
    ap->p[k+1]=apDir;
    ap->n++;
}

void Ins(TipoRegistro reg, TipoApontador ap, short *cresceu, TipoRegistro *regRetorno, TipoApontador *apRetorno){
    long i=1, j;
    TipoApontador apTemp;
    if(ap == NULL){
        *cresceu = 1;
        *regRetorno=reg;
        *apRetorno=NULL;
        return;
    }

    while(i<ap->n && reg.chave>ap->r[i-1].chave) i++;
    if(reg.chave==ap->r[i-1].chave) {
        *cresceu=0;
        return;
    }

    if(reg.chave<ap->r[i-1].chave) i--;
    Ins(reg, ap->p[i], cresceu, regRetorno, apRetorno);
    if(!*cresceu) return;
    if(ap->n < MM){
        InsereNaPagina(ap, *regRetorno, *apRetorno);
        *cresceu=0;
        return;
    }

    apTemp=(TipoApontador)malloc(sizeof(TipoPagina));
    apTemp->n=0;
    apTemp->p[0]=NULL;

    if(i<M+1){
        InsereNaPagina(apTemp, ap->r[MM-1], ap->p[MM]);
        ap->n--;
        InsereNaPagina(ap, *regRetorno, *apRetorno);
    }else {
        InsereNaPagina(apTemp, *regRetorno, *apRetorno);
    }
    for(j=M+2; j<=MM; j++) {
        InsereNaPagina(apTemp, ap->r[j - 1], ap->p[j]);
    }
    ap->n=M;
    apTemp->p[0]=ap->p[M+1];
    *regRetorno=ap->r[M];
    *apRetorno=apTemp;
}

void Insere(TipoRegistro reg, TipoApontador *ap){
    short cresceu;
    TipoRegistro regRetorno;
    TipoPagina *apRetorno, *apTemp;

    Ins(reg, *ap, &cresceu, &regRetorno, &apRetorno);
    if(cresceu){
        apTemp = (TipoPagina*)malloc(sizeof(TipoPagina));
        apTemp->n=1;
        apTemp->r[0]=regRetorno;
        apTemp->p[1]=apRetorno;
        apTemp->p[0]=*ap;
        *ap = apTemp;
    }
}