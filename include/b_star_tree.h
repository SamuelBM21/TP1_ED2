#ifndef B_STAR_TREE_H
#define B_STAR_TREE_H



#include "register.h"

typedef enum { Interna, Externa } TipoIntExt;
typedef struct PaginaStar* ApontaPaginaStar;

typedef struct PaginaStar {
    TipoIntExt Pt;
    union {
        struct {
            int ni;
            long long ri[MM];           // Vetor de chaves (mantendo o nome ri)
            ApontaPaginaStar pi[MM + 1]; // Ponteiros para filhos
        } Int;
        struct {
            int ne;
            Registro re[MM];            // Registros nas folhas
        } Ext;
    } Union;
} PaginaStar;

// Funções principais
void InicializaStar(ApontaPaginaStar *arvore);
void LiberaStar(ApontaPaginaStar *ap);
int PesquisaStar(Registro *x, ApontaPaginaStar ap, int *comp);
void InsereStar(Registro reg, ApontaPaginaStar *ap);

#endif
