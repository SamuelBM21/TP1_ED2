#ifndef B_TREE_H
#define B_TREE_H

#define M 5
#define MM 2*M

#include "register.h"

typedef struct Pagina* ApontaPagina;

typedef struct Pagina{
    short n;
    Registro r[MM];
    ApontaPagina p[MM+1];
}Pagina;

void Inicializa(ApontaPagina *arvore);
void Libera(ApontaPagina *ap);
int Pesquisa(Registro *x, ApontaPagina ap);
void Imprime(ApontaPagina arvore);
void InsereNaPagina(ApontaPagina ap, Registro reg, ApontaPagina apDir);
void Ins(Registro reg, ApontaPagina ap, short *cresceu, Registro *regRetorno, ApontaPagina *apRetorno);
void Insere(Registro reg, ApontaPagina *ap);

#endif //B_TREE_H
