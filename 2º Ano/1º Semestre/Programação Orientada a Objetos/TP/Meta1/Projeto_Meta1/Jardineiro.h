#ifndef JARDINEIRO_H
#define JARDINEIRO_H

class Jardineiro {
    bool dentro = false;
    int linha = 0, coluna = 0;

public:
    bool estaDentro() const { return dentro; }
    void entra(int l, int c) { dentro = true; linha = l; coluna = c; }
    void sai() { dentro = false; }
};

#endif
