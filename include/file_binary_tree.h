#include "register.h"

#ifndef FILE_BINARY_TREE_H
#define FILE_BINARY_TREE_H

int generateBinaryTreeFile(char fileName[]);
int insereArvBin(RegistroArvore inserido, FILE* arq, long *leitura, long *comp);
int searchTreeBinary(long long chave, RegistroArvore *registro, int *comp, FILE *a, long long quantidade, int *leitura);
#endif //FILE_BINARY_TREE_H
