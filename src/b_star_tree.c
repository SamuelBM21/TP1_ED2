#include <stdio.h>
#include <stdlib.h>
#include "../include/b_star_tree.h"

// Inicializa a árvore B* como vazia (ponteiro NULL)
void InicializaStar(ApontaPaginaStar *arvore) {
    *arvore = NULL;
}

// Libera recursivamente toda a memória ocupada pela árvore
void LiberaStar(ApontaPaginaStar *ap) {
    if ((*ap) == NULL) return; // Caso base: árvore vazia

    if ((*ap)->Pt == Interna) { // Se for nó interno, libera recursivamente os filhos
        for (int i = 0; i <= (*ap)->Union.Int.ni; i++) {
            LiberaStar(&((*ap)->Union.Int.pi[i]));
        }
    }

    free(*ap); // Libera o próprio nó
    *ap = NULL; // Evita ponteiro pendente
}

// Função de pesquisa na árvore B*
int PesquisaStar(Registro *x, ApontaPaginaStar ap, int *comp) {
    int i;
    if (ap == NULL) return 0; // Árvore vazia, chave não encontrada

    if (ap->Pt == Interna) { // Se nó interno, desce na árvore
        i = 1;
        // Encontra o índice do ponteiro filho apropriado
        while (i < ap->Union.Int.ni && x->chave > ap->Union.Int.ri[i - 1]){ 
            i++;
            (*comp)++;
        }
        (*comp)++;
        if (x->chave < ap->Union.Int.ri[i - 1])
            return PesquisaStar(x, ap->Union.Int.pi[i - 1], comp); // Vai para a esquerda
        else
            return PesquisaStar(x, ap->Union.Int.pi[i], comp);     // Vai para a direita
    }

    // Nó folha: busca sequencial ordenada
    i = 1;
    while (i < ap->Union.Ext.ne && x->chave > ap->Union.Ext.re[i - 1].chave){
        i++;
        (*comp)++;
    }

    (*comp)++;
    if (x->chave == ap->Union.Ext.re[i - 1].chave) { // Encontrou
        *x = ap->Union.Ext.re[i - 1]; // Copia os dados encontrados
        return 1;
    } else {
        return 0; // Não encontrado
    }
}

// Insere um registro diretamente em uma folha específica (sem tratar splits)
void insere_na_folha_especifica(ApontaPaginaStar folha, Registro reg, long *comp) {
    int i = folha->Union.Ext.ne;
    // Desloca registros maiores para frente
    while (i > 0 && reg.chave < folha->Union.Ext.re[i - 1].chave) {
        (*comp)++; // contando o numero de comparações
        folha->Union.Ext.re[i] = folha->Union.Ext.re[i - 1];
        i--;
    }
    if(i>0) (*comp)++;

    folha->Union.Ext.re[i] = reg; // Insere na posição correta
    folha->Union.Ext.ne++;        // Incrementa o número de registros na folha
}

// Insere uma chave e um ponteiro no nó interno (sem tratar splits)
static void insere_no_no_interno(ApontaPaginaStar ap, long long chave, ApontaPaginaStar filho_dir, long *comp) {
    int i = ap->Union.Int.ni;
    // Desloca chaves maiores para frente
    while (i > 0 && chave < ap->Union.Int.ri[i - 1]) {
        (*comp)++;
        ap->Union.Int.ri[i] = ap->Union.Int.ri[i - 1];
        ap->Union.Int.pi[i + 1] = ap->Union.Int.pi[i];
        i--;
    }
    if (i > 0) (*comp)++;

    // Insere chave e ponteiro na posição correta
    ap->Union.Int.ri[i] = chave;
    ap->Union.Int.pi[i + 1] = filho_dir;
    ap->Union.Int.ni++; // Incrementa o número de chaves no nó interno
}

// Função recursiva que gerencia a inserção na árvore
static void insere_recursivo(Registro reg, ApontaPaginaStar ap, short *cresceu, long long *chave_retorno, ApontaPaginaStar *ap_retorno, long *comp) {
    if (ap->Pt == Externa) { // Caso base: nó folha
        if (ap->Union.Ext.ne < MM) {
            // Caso haja espaço na folha, insere diretamente
            insere_na_folha_especifica(ap, reg, comp);
            *cresceu = 0;
            return;
        }

        // Split da folha (folha cheia)
        ApontaPaginaStar nova_folha = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        nova_folha->Pt = Externa;

        Registro temp[MM + 1]; // Vetor temporário para acomodar os registros existentes + novo
        for (int i = 0; i < MM; i++) temp[i] = ap->Union.Ext.re[i];

        // Insere o novo registro na posição correta no vetor temporário
        int j = MM;
        while (j > 0 && reg.chave < temp[j - 1].chave) {
            (*comp)++;
            temp[j] = temp[j - 1];
            j--;
        }
        if (j > 0) (*comp)++; // comparação que quebrou o while

        temp[j] = reg;

        // Divide os registros entre a folha atual e a nova folha
        ap->Union.Ext.ne = (MM + 1) / 2;
        nova_folha->Union.Ext.ne = (MM + 1) - ap->Union.Ext.ne;

        for (int i = 0; i < ap->Union.Ext.ne; i++) 
            ap->Union.Ext.re[i] = temp[i];

        for (int i = 0; i < nova_folha->Union.Ext.ne; i++) 
            nova_folha->Union.Ext.re[i] = temp[i + ap->Union.Ext.ne];

        // Retorna a menor chave da nova folha (para promover no nó pai)
        *chave_retorno = nova_folha->Union.Ext.re[0].chave;
        *ap_retorno = nova_folha;
        *cresceu = 1;
        return;
    }

    // Caso seja nó interno
    int i = ap->Union.Int.ni - 1;
    // Encontra o filho apropriado para descer
    while (i >= 0 && reg.chave < ap->Union.Int.ri[i]){
        i--;
        (*comp)++;
    }
    if (i >= 0) (*comp)++;
    i++;

    // Chamada recursiva no filho
    insere_recursivo(reg, ap->Union.Int.pi[i], cresceu, chave_retorno, ap_retorno, comp);

    if (!(*cresceu)) return; // Se o filho não cresceu, encerra

    if (ap->Union.Int.ni < MM) {
        // Se há espaço no nó interno, insere chave e ponteiro do filho
        insere_no_no_interno(ap, *chave_retorno, *ap_retorno, comp);
        *cresceu = 0;
        return;
    }

    // Split do nó interno
    ApontaPaginaStar novo_no = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
    novo_no->Pt = Interna;

    long long temp_ri[MM + 1];            // Vetor temporário para chaves
    ApontaPaginaStar temp_pi[MM + 2];     // Vetor temporário para ponteiros

    // Copia os dados atuais para os vetores temporários
    for (int j = 0; j < MM; j++) temp_ri[j] = ap->Union.Int.ri[j];
    for (int j = 0; j <= MM; j++) temp_pi[j] = ap->Union.Int.pi[j];

    // Insere a nova chave e ponteiro na posição correta
    int j = MM;
    while (j > 0 && *chave_retorno < temp_ri[j - 1]) {
        (*comp)++;
        temp_ri[j] = temp_ri[j - 1];
        temp_pi[j + 1] = temp_pi[j];
        j--;
    }
    if (j > 0) (*comp)++;

    temp_ri[j] = *chave_retorno;
    temp_pi[j + 1] = *ap_retorno;

    // Calcula ponto de divisão (meio)
    int meio = (MM + 1) / 2;
    ap->Union.Int.ni = meio;
    novo_no->Union.Int.ni = MM - meio;

    // Distribui chaves e ponteiros entre nó atual e novo nó
    for (int k = 0; k < meio; k++) {
        ap->Union.Int.ri[k] = temp_ri[k];
        ap->Union.Int.pi[k] = temp_pi[k];
    }
    ap->Union.Int.pi[meio] = temp_pi[meio];

    for (int k = 0; k < novo_no->Union.Int.ni; k++) {
        novo_no->Union.Int.ri[k] = temp_ri[k + meio + 1];
        novo_no->Union.Int.pi[k] = temp_pi[k + meio + 1];
    }
    novo_no->Union.Int.pi[novo_no->Union.Int.ni] = temp_pi[MM + 1];

    // Promove a chave do meio para o nível acima
    *chave_retorno = temp_ri[meio];
    *ap_retorno = novo_no;
    *cresceu = 1;
}

// Função principal de inserção pública na árvore
void InsereStar(Registro reg, ApontaPaginaStar *ap, long *comp) {
    short cresceu;
    long long chave_retorno;
    ApontaPaginaStar ap_retorno;

    // Se a árvore está vazia, cria uma nova folha e insere o registro
    if (*ap == NULL) {
        *ap = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        (*ap)->Pt = Externa;
        (*ap)->Union.Ext.ne = 1;
        (*ap)->Union.Ext.re[0] = reg;
        return;
    }

    // Chamada recursiva para inserir
    insere_recursivo(reg, *ap, &cresceu, &chave_retorno, &ap_retorno, comp);

    if (cresceu) {
        // Se a raiz cresceu, cria uma nova raiz
        ApontaPaginaStar nova_raiz = (ApontaPaginaStar)malloc(sizeof(PaginaStar));
        nova_raiz->Pt = Interna;
        nova_raiz->Union.Int.ni = 1;
        nova_raiz->Union.Int.ri[0] = chave_retorno;
        nova_raiz->Union.Int.pi[0] = *ap;
        nova_raiz->Union.Int.pi[1] = ap_retorno;
        *ap = nova_raiz; // Atualiza ponteiro da árvore para nova raiz
    }
}
