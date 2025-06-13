#include <stdio.h>
#include <stdlib.h>
#include "../include/b_star_tree.h"

void InicializaStar(ApontaPaginaStar *arvore){
    *arvore = NULL;
}

void LiberaStar(ApontaPaginaStar *ap){
    if ((*ap) == NULL) return;

    // Se for um nó interno, precisa liberar os filhos primeiro
    if ((*ap)->Pt == Interna) {
        // Percorre todos os ponteiros dos filhos
        for (int i = 0; i <= (*ap)->Union.Int.ni; i++) {
            LiberaStar(&((*ap)->Union.Int.pi[i]));
        }
    }
    
    // Libera a memória do nó atual, seja ele interno ou folha
    free(*ap);
    *ap = NULL;
}

int PesquisaStar(Registro *x, ApontaPaginaStar ap){
    int i;
    ApontaPaginaStar Pag;
    Pag = ap;

    if(ap->Pt == Interna){
        i = 1;
        while(i < Pag->Union.Int.ni && x->chave > Pag->Union.Int.ri[i-1].chave) i++;
        if(x->chave < Pag->Union.Int.ri[i-1].chave)
            PesquisaStar(x, Pag->Union.Int.pi[i-1]);
        else
            PesquisaStar(x, Pag->Union.Int.pi[i]);
        return 1;
    }
    i = 1;
    while(i < Pag->Union.Ext.ne && x->chave > Pag->Union.Ext.re[i-1].chave) i++;
    if(x->chave == Pag->Union.Ext.re[i-1].chave){
        *x = Pag->Union.Ext.re[i-1];
        return 1;
    }
    else
        printf("O Registro não está presente na arvore\n");
    return 0;

}



void insere_na_folha_especifica(ApontaPaginaStar folha, Registro reg) {
    int i = folha->Union.Ext.ne;
    // Desloca registros para a direita para abrir espaço
    while(i > 0 && reg.chave < folha->Union.Ext.re[i-1].chave) {
        folha->Union.Ext.re[i] = folha->Union.Ext.re[i-1];
        i--;
    }
    // Insere o novo registro na posição correta
    folha->Union.Ext.re[i] = reg;
    folha->Union.Ext.ne++; // Incrementa o contador de registros da folha
}

void InsereStar(Registro reg, ApontaPaginaStar *ap) {
    short cresceu;
    Registro reg_retorno;
    ApontaPaginaStar ap_retorno;

    if (*ap == NULL) {
        *ap = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        (*ap)->Pt = Externa;
        (*ap)->Union.Ext.ne = 1;
        (*ap)->Union.Ext.re[0] = reg;
        return;
    }

    insere_recursivo(reg, *ap, &cresceu, &reg_retorno, &ap_retorno);

    if (cresceu) {
        ApontaPaginaStar nova_raiz = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        nova_raiz->Pt = Interna;
        nova_raiz->Union.Int.ni = 1;
        nova_raiz->Union.Int.ri[0] = reg_retorno;
        nova_raiz->Union.Int.pi[0] = *ap;
        nova_raiz->Union.Int.pi[1] = ap_retorno;
        *ap = nova_raiz;
    }
}



static void insere_recursivo(Registro reg, ApontaPaginaStar ap, short *cresceu, Registro *reg_retorno, ApontaPaginaStar *ap_retorno) {
    if (ap->Pt == Externa) {
        // Caso base: folha
        if (ap->Union.Ext.ne < MM) {
            insere_na_folha_especifica(ap, reg);
            *cresceu = 0;
            return;
        }

        // SPLIT da folha
        ApontaPaginaStar nova_folha = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        nova_folha->Pt = Externa;

        Registro temp[MM + 1];
        int i = 0;
        while (i < MM) {
            temp[i] = ap->Union.Ext.re[i];
            i++;
        }

        // Inserção ordenada com novo registro
        int j = MM;
        while (j > 0 && reg.chave < temp[j - 1].chave) {
            temp[j] = temp[j - 1];
            j--;
        }
        temp[j] = reg;

        // Divide os registros
        ap->Union.Ext.ne = (MM + 1) / 2;
        nova_folha->Union.Ext.ne = (MM + 1) - ap->Union.Ext.ne;

        for (i = 0; i < ap->Union.Ext.ne; i++)
            ap->Union.Ext.re[i] = temp[i];

        for (i = 0; i < nova_folha->Union.Ext.ne; i++)
            nova_folha->Union.Ext.re[i] = temp[i + ap->Union.Ext.ne];

        *reg_retorno = nova_folha->Union.Ext.re[0]; // Primeira chave da nova folha
        *ap_retorno = nova_folha;
        *cresceu = 1;
        return;
    }

    // Nó interno: desce recursivamente
    int i = ap->Union.Int.ni - 1;
    while (i >= 0 && reg.chave < ap->Union.Int.ri[i].chave) i--;
    i++;

    insere_recursivo(reg, ap->Union.Int.pi[i], cresceu, reg_retorno, ap_retorno);

    if (!(*cresceu)) return;

    // Tem espaço no nó interno
    if (ap->Union.Int.ni < MM) {
        insere_no_no_interno(ap, *reg_retorno, *ap_retorno);
        *cresceu = 0;
        return;
    }

    // SPLIT do nó interno
    ApontaPaginaStar novo_no = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
    novo_no->Pt = Interna;

    Registro temp_reg[MM + 1];
    ApontaPaginaStar temp_ptr[MM + 2];

    for (int j = 0; j < MM; j++) temp_reg[j] = ap->Union.Int.ri[j];
    for (int j = 0; j <= MM; j++) temp_ptr[j] = ap->Union.Int.pi[j];

    // Inserção ordenada
    int j = MM;
    while (j > 0 && reg_retorno->chave < temp_reg[j - 1].chave) {
        temp_reg[j] = temp_reg[j - 1];
        temp_ptr[j + 1] = temp_ptr[j];
        j--;
    }
    temp_reg[j] = *reg_retorno;
    temp_ptr[j + 1] = *ap_retorno;

    int meio = (MM + 1) / 2;
    ap->Union.Int.ni = meio;
    novo_no->Union.Int.ni = MM - meio;

    for (int k = 0; k < meio; k++) {
        ap->Union.Int.ri[k] = temp_reg[k];
        ap->Union.Int.pi[k] = temp_ptr[k];
    }
    ap->Union.Int.pi[meio] = temp_ptr[meio];

    for (int k = 0; k < novo_no->Union.Int.ni; k++) {
        novo_no->Union.Int.ri[k] = temp_reg[k + meio + 1];
        novo_no->Union.Int.pi[k] = temp_ptr[k + meio + 1];
    }
    novo_no->Union.Int.pi[novo_no->Union.Int.ni] = temp_ptr[MM + 1];

    *reg_retorno = temp_reg[meio];
    *ap_retorno = novo_no;
    *cresceu = 1;
}

static void insere_no_no_interno(ApontaPaginaStar ap, Registro reg, ApontaPaginaStar filho_dir) {
    int i = ap->Union.Int.ni;
    while (i > 0 && reg.chave < ap->Union.Int.ri[i - 1].chave) {
        ap->Union.Int.ri[i] = ap->Union.Int.ri[i - 1];
        ap->Union.Int.pi[i + 1] = ap->Union.Int.pi[i];
        i--;
    }
    ap->Union.Int.ri[i] = reg;
    ap->Union.Int.pi[i + 1] = filho_dir;
    ap->Union.Int.ni++;
}
