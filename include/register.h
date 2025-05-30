//
// Created by obraganca on 5/30/25.
//

#ifndef REGISTER_H
#define REGISTER_H

#define ITENSPAGINA 1000
#define MAXTABELA 1000


typedef struct{
    long long chave;
    long long dado1;
    char dado2[1000];
    char dado3[5000];
    long long esq;
    long long dir;
}Registro;



#endif //REGISTER_H
