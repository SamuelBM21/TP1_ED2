#include "register.h"

#ifndef FILE_BINARY_TREE_H
#define FILE_BINARY_TREE_H

int generateBinaryTreeFile(char fileName[]);
int insereArvBin(Registro inserido, FILE* arq);
int searchTreeBinary(long long chave, Registro *registro);

#endif //FILE_BINARY_TREE_H
