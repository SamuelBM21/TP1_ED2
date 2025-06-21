#ifndef B_STAR_TREE_H
#define B_STAR_TREE_H

#define M 5
#define MM 2*M

#include "register.h"

typedef enum {Interna, Externa} TipoIntExt;
typedef struct PaginaStar* ApontaPaginaStar;

typedef struct PaginaStar {
    TipoIntExt Pt;
    union {
        struct {
            int ni;
            Registro ri[MM];
            ApontaPaginaStar pi[MM + 1];
        } Int;
        struct {
            int ne;
            Registro re[MM];
        } Ext;
    } Union;
} PaginaStar;



void InicializaStar(ApontaPaginaStar *arvore);
void LiberaStar(ApontaPaginaStar *ap);
int PesquisaStar(Registro *x, ApontaPaginaStar ap);
//void ImprimeStar(ApontaPaginaStar arvore);
void InsereStar(Registro reg, ApontaPaginaStar *ap);


#endif //B_STAR_TREE_H
