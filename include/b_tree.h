//
// Created by obraganca on 5/30/25.
//

#ifndef B_TREE_H
#define B_TREE_H

#define M 5
#define MM 2*M
typedef long TipoChave;

typedef struct TipoRegistro{
    TipoChave chave;
} TipoRegistro;

typedef struct TipoPagina* TipoApontador;

typedef struct TipoPagina{
    short n;
    TipoRegistro r[MM];
    TipoApontador p[MM+1];
}TipoPagina;

void Inicializa(TipoApontador *arvore);
int Pesquisa(TipoRegistro *x, TipoApontador ap);
void Imprime(TipoApontador arvore);
void InsereNaPagina(TipoApontador ap, TipoRegistro reg, TipoApontador apDir);
void Ins(TipoRegistro reg, TipoApontador ap, short *cresceu, TipoRegistro *regRetorno, TipoApontador *apRetorno);
void Insere(TipoRegistro reg, TipoApontador *ap);

#endif //B_TREE_H
