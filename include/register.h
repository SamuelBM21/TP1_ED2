#ifndef REGISTER_H
#define REGISTER_H


typedef struct{
    long long chave;
    long long dado1;
    char dado2[1000];
    char dado3[5000];
    long long esq;
    long long dir;
}RegistroArvore;

typedef struct{
    long long chave;
    long long dado1;
    char dado2[1000];
    char dado3[5000];
}Registro;



#endif //REGISTER_H
