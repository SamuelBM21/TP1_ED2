#ifndef GENERATOR_H
#define GENERATOR_H

#define ORDER_ASCENDING 0
#define ORDER_DESCENDING 1
#define ORDER_RANDOM 2

int generateFile(long long numLines, const char *nameFile, int mode);
void numeroPorExtenso(int numero, char *saida);
void shuffle(long long *array, long long n);

#endif
