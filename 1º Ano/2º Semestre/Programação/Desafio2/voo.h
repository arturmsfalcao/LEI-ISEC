
#ifndef P2526_DESAFIO2
#define P2526_DESAFIO2

typedef struct a data;
typedef struct b voo;

struct a{
    int dia, mes;
};

struct b{
    char codigo[10];        // Identificador alfanumerico unico
    char origem[50];        // Cidade origem
    char destino[50];       // Cidade destino
    data partida;           // Data da partida (dia e mes)
    int lugares;            // Lugares livres
};

void printVoo(voo a);

void printTab(voo a[], int tam);

int desafio2(voo tab[], int tam, char *or, char *dest, data lInf, data lSup);


#endif //P2526_DESAFIO2