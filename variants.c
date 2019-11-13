#include <stdio.h>
#include <stdlib.h>

#include "charStack.h"

#define CHECK_CANTO if(fscanf(fp, " %c", &c) != 1) exit(0); if(c == 'T') return 1
#define CHECK_LADO if(fscanf(fp, " %c", &c) != 1) exit(0); if(c == 'T') return 1; if(c == 'A') sem_arvores = 0
#define CHECK_CENTRO if(fscanf(fp, " %c", &c) != 1) exit(0); if(c == 'A') return 1

#define DIR 1 // direita, esquerda, cima e baixo
#define ESQ -1
#define CIMA 2
#define BAIXO -2

char **Matriz;
int L;
int C;
FILE *fp;
/*unsigned char* stack;
long int massi= 0;*/

void _free_matriz() {
    int i;
    for(i=0; i<L; i++) {
        free(Matriz[i]);
    }
    free(Matriz);
}

void InitSolver(FILE *fpointer, int Linhas, int Colunas) {
    fp = fpointer;
    L = Linhas;
    C = Colunas;
}


// Descrição: Determina se pode existir uma tenda nas coordenadas fornecidas, de acordo com os critérios de B,
//            lendo diretamente do ficheiro.
// Argumentos: Ficheiro com o problema apontando para depois de "B l0 c0", linha e coluna.
// Retorno: 1 caso seja detetada a impossibilidade de existir uma tenda, 0 caso contrário.
int SolveBfromFile(int l0, int c0) {
    int i, j;
    int l0_tents, c0_tents;
    int somaC = 1, somaL = 1;
    char sem_arvores = 1;
    char c;

    if(l0 < 0 || l0 >= L || c0 < 0 || c0 >= C) {
        // Limpar os números antes de saír
        do {
            c = fgetc(fp);
        } while(c != 'A' && c != 'T' && c != '.');
        return -1;
    }

    // Ler vetor tendas por linha
    for(i=0; i<l0; i++) if(fscanf(fp, " %*d") != 0) exit(0);
    if(fscanf(fp, " %d", &l0_tents) != 1) exit(0);
    for(i=l0+1; i<L; i++) if(fscanf(fp, " %*d") != 0) exit(0);

    // Ler vetor tendas por coluna
    for(i=0; i<c0; i++) if(fscanf(fp, " %*d") != 0) exit(0);
    if(fscanf(fp, " %d", &c0_tents) != 1) exit(0);
    for(i=c0+1; i<C; i++) if(fscanf(fp, " %*d") != 0) exit(0);

    if(l0_tents == 0 || c0_tents == 0) return 1;

    // Ler matriz

    // Ler linhas antes dos adjacentes de (l0, c0)
    for(i=0; i < l0 - 1; i++) {
        for(j=0; j<c0; j++) if(fscanf(fp, " %*c") != 0) exit(0);
        if(fscanf(fp, " %c", &c) != 1) exit(0);
        if(c == 'T')
            if(++somaC > c0_tents) return 1;
        for(j=c0+1; j<C; j++) if(fscanf(fp, " %*c") != 0) exit(0);
    }
    if(l0 != 0) {
        // Ler l0 - 1
        if(c0 != 0) {
            for(j = 0; j < c0 - 1; j++) if(fscanf(fp, " %*c") != 0) exit(0);
            CHECK_CANTO;
        }
        CHECK_LADO;
        if(c0 != C - 1)  {
            CHECK_CANTO;
            for(j =c0 + 2; j < C; j++) if(fscanf(fp, " %*c") != 0) exit(0);
        }
    }

    // Ler l0
    if(c0 != 0) {
        for(j = 0; j < c0 - 1; j++) {
            if(fscanf(fp, " %c", &c) != 1) exit(0);
            if(c == 'T')
                if(++somaL > l0_tents) return 1;
        }
        CHECK_LADO;
    }
    CHECK_CENTRO;
    if(c0 != C - 1)  {
        CHECK_LADO;
        for(j =c0 + 2; j < C; j++) {
            if(fscanf(fp, " %c", &c) != 1) exit(0);
            if(c == 'T')
                if(++somaL > l0_tents) return 1;
        }
    }

    if(l0 != L - 1) {
        // Ler l0 + 1
        if(c0 != 0) {
            for(j = 0; j < c0 - 1; j++) if(fscanf(fp, " %*c") != 0) exit(0);
            CHECK_CANTO;
        }
        CHECK_LADO;

        if(sem_arvores == 1) return 1;
        if(c0 != C - 1)  {
            CHECK_CANTO;
            for(j =c0 + 2; j < C; j++) if(fscanf(fp, " %*c") != 0) exit(0);
        }
    }

    if(sem_arvores == 1) return 1;

    // Ler linhas depois dos adjacentes de (l0, c0)
    for(i=l0+2; i < L; i++) {
        for(j=0; j<c0; j++) if(fscanf(fp, " %*c") != 0) exit(0);
        if(fscanf(fp, " %c", &c) != 1) exit(0);
        if(c == 'T')
            if(++somaC > c0_tents) return 1;
        for(j=c0+1; j<C; j++) if(fscanf(fp, " %*c") != 0) exit(0);
    }
    return 0;
}

// Descrição: Determina se o problema é admissível, segundo os critérios de A, lendo diretamente do ficheiro.
// Argumentos: Ficheiro com o problema apontando para depois de 'A', número de linhas e número de colunas.
// Retorno: 0 caso seja detetada a inadmissibilidade do problema, 1 caso contrário.
int SolveAfromFile() {
    int i, j;
    int somaL = 0, somaC = 0;
    int arvores = 0;
    char c;

    for(i=0; i<L; i++) {
        if(fscanf(fp, " %d", &j) != 1) exit(0);
        somaL += j;
    }

    for(i=0; i<C; i++) {
        if(fscanf(fp, "%d", &j) != 1) exit(0);
        somaC += j;
    }
    /* Verifica admissiblidade do jogo */
    if(somaC != somaL) return 0;

    for ( i = 0; i < L; i++) {
        for ( j = 0; j < C; j++) {
            if(fscanf(fp, " %c", &c) != 1) exit(0);
            if(c=='A') arvores++;
        }
    }

    /* Verifica admissiblidade do jogo */
    if(somaL > arvores) return 0;

    return 1;

}

// Descrição: Determina se esta tenda possui árvores adjacentes disponíveis.
// Argumentos: Linha e coluna da tenda.
// Retorno: 1 caso tenda não tenha árvore disponível, N caso ocorra um erro na alocação de memória.
char isT_alone(unsigned char l0, unsigned char c0) {
    /*if(stack-&c0 > massi) {
        massi = stack - &c0;
        printf("stack at %li\n", massi);
    }*/
    Matriz[l0][c0] = '.';
    // Ver a direita
    if(c0!=C-1) {
        if(Matriz[l0][c0+1] == 'A') {
            Matriz[l0][c0+1] = '.';
            if(c0==C-2)
                return 0;
            if(Matriz[l0][c0+2] != 'T')
                return 0;
            if(isT_alone(l0, c0+2) == 0)
                return 0;
        }
    }
    // Ver em baixo
    if(l0!=L-1) {
        if(Matriz[l0+1][c0] == 'A') {
            Matriz[l0+1][c0] = '.';
            if(l0==L-2)
                return 0;
            if(Matriz[l0+2][c0] != 'T')
                return 0;
            if(isT_alone(l0+2, c0) == 0)
                return 0;
        }
    }
    // Ver a esquerda
    if(c0!=0) {
        if(Matriz[l0][c0-1] == 'A') {
            Matriz[l0][c0-1] = '.';
            if(c0==1)
                return 0;
            if(Matriz[l0][c0-2] != 'T')
                return 0;
            if(isT_alone(l0, c0-2) == 0)
                return 0;
        }
    }
    // Ver em cima
    if(l0!=0) {
        if(Matriz[l0-1][c0] == 'A') {
            Matriz[l0-1][c0] = '.';
            if(l0==1)
                return 0;
            if(Matriz[l0-2][c0] != 'T')
                return 0;
            if(isT_alone(l0-2, c0) == 0)
                return 0;
        }
    }

    return 1;
}

void move_dir(int* l_out, int* c_out, unsigned char dir){
    if(dir== CIMA){
        *l_out += -2;
    } else if( dir == BAIXO) {
        *l_out += 2;
    } else if( dir == ESQ) {
        *c_out += -2;
    } else if( dir == DIR) {
        *c_out += 2;
    }
}


// Descrição: Determina se esta tenda possui árvores adjacentes disponíveis.
// Argumentos: Linha e coluna da tenda.
// Retorno: 0 caso a tenda tenha árvore disponível, 1 caso contrário.
char isT_alone_iter(int l0, int c0) {
    char from = 0;

    initStack();
    while(1) {
        Matriz[l0][c0] = '.';
        // Ver a direita
        if(c0!=C-1) {
            if(Matriz[l0][c0+1] == 'A') {
                Matriz[l0][c0+1] = '.';
                if(c0==C-2) {
                    freeStack();
                    return 0;
                }
                if(Matriz[l0][c0+2] != 'T') {
                    freeStack();
                    return 0;
                }
                push(from);
                from = ESQ;
                continue;
            }
        }
        // Ver a baixo
        if(l0!=L-1) {
            if(Matriz[l0+1][c0] == 'A') {
                Matriz[l0+1][c0] = '.';
                if(l0==L-2) {
                    freeStack();
                    return 0;
                }
                if(Matriz[l0+2][c0] != 'T')  {
                    freeStack();
                    return 0;
                }
                push(from);
                from = CIMA;
                continue;
            }
        }
        // Ver a esquerda
        if(c0!=0) {
            if(Matriz[l0][c0-1] == 'A') {
                Matriz[l0][c0-1] = '.';
                if(c0==1) {
                    freeStack();
                    return 0;
                }
                if(Matriz[l0][c0-2] != 'T') {
                    freeStack();
                    return 0;
                }
                push(from);
                from = DIR;
                continue;
            }
        }
        // Ver a cima
        if(l0!=0) {
            if(Matriz[l0-1][c0] == 'A') {
                Matriz[l0-1][c0] = '.';
                if(l0==1) {
                    freeStack();
                    return 0;
                }
                if(Matriz[l0-2][c0] != 'T') {
                    freeStack();
                    return 0;
                }

                push(from);
                from = BAIXO;
                continue;
            }
        }
        if(from == 0) return 1;
        move_dir(&l0, &c0, from);
        from = pop();
    }
}


// Descrição: Cria uma matriz para o mapa e determina se há tendas ilegais, no que toca a somatórios por
//            linhas/colunas e adjacência entre tendas, lendo diretamente do ficheiro.
// Argumentos: Ficheiro com o problema apontando para o início da matriz, número de linhas, número de colunas
//             vetores de tendas, por linhas e por colunas, e endereço onde escrever o endereço da matriz criada.
// Retorno: 1 caso exista alguma tenda ilegal, 0 caso não exista, -1 caso ocorra um erro na alocação de memória.
int VerSomasAdjacentes_fromFile(int *Ltents, int *Ctents) {
    int i, j;

    Matriz = (char**) malloc(L*sizeof(char*));
    if(Matriz == NULL) return -1;

    // linha 0
    Matriz[0] = (char*) malloc(C*sizeof(char));
    if(Matriz[0] == NULL) return -1;
    // célula (0, 0)
    if(fscanf(fp, " %c", &Matriz[0][0]) != 1) exit(0);
    if(Matriz[0][0] == 'T') {
        if(--Ltents[0] < 0) return 1;
        if(--Ctents[0] < 0) return 1;
    }
    // Resto da linha 0
    for ( j = 1; j < C; j++) {
        if(fscanf(fp, " %c", &Matriz[0][j]) != 1) exit(0);
        if(Matriz[0][j] == 'T') {
            // Ver tendas adjacentes na parte já lida da matriz.
            if(Matriz[0][j-1] == 'T')
                return 1;
            if(--Ltents[0] < 0) return 1;
            if(--Ctents[j] < 0) return 1;
        }
    }

    // Resto das linhas
    for ( i = 1; i < L; i++) {
        Matriz[i] = (char*) malloc(C*sizeof(char));
        if(Matriz[i] == NULL) return -1;
        // célula (i, 0)
        if(fscanf(fp, " %c", &Matriz[i][0]) != 1) exit(0);
        if(Matriz[i][0] == 'T') {
            // Ver tendas adjacentes na parte já lida da matriz.
            if( Matriz[i-1][0] == 'T' || Matriz[i-1][1] == 'T')
                return 1;
            if(--Ltents[i] < 0) return 1;
            if(--Ctents[0] < 0) return 1;
        }
        // Resto da linha i excepto última
        for ( j = 1; j < C - 1; j++) {
            if(fscanf(fp, " %c", &Matriz[i][j]) != 1) exit(0);

            if(Matriz[i][j] == 'T') {
                // Ver tendas adjacentes na parte já lida da matriz.
                if(Matriz[i-1][j-1] == 'T' || Matriz[i-1][j] == 'T' ||
                        Matriz[i-1][j+1] == 'T' || Matriz[i][j-1] == 'T')
                    return 1;
                if(--Ltents[i] < 0) return 1;
                if(--Ctents[j] < 0) return 1;
            }
        }
        // célula (i, C-1)
        if(fscanf(fp, " %c", &Matriz[i][C-1]) != 1) exit(0);
        if(Matriz[i][C-1] == 'T') {
            // Ver tendas adjacentes na parte já lida da matriz.
            if(Matriz[i-1][C-2] == 'T' || Matriz[i-1][C-1] == 'T' ||
                    Matriz[i][C-2] == 'T')
                return 1;
            if(--Ltents[i] < 0) return 1;
            if(--Ctents[C-1] < 0) return 1;
        }
    }
    return 0;
}

// Descrição: Determina se há tendas ilegais, lendo diretamente do ficheiro.
// Argumentos: Ficheiro com o problema apontando para depois de 'C', número de linhas e número de colunas.
// Retorno: 1 caso exista alguma tenda ilegal, 0 caso não exista, -1 caso ocorra um erro na alocação de memória.
int SolveCfromFile() {
    int i, j;
    int * Ltents, *Ctents;
    int res;



    Ltents = (int*) malloc(L*sizeof(int));
    if(Ltents == NULL) return -1;

    Ctents = (int*) malloc(C*sizeof(int));
    if(Ctents == NULL) {
        free(Ltents);
        return -1;
    }

    for(i=0; i<L; i++) {
        if(fscanf(fp, " %d", &Ltents[i]) != 1) exit(0);
    }
    for(i=0; i<C; i++) {
        if(fscanf(fp, " %d", &Ctents[i]) != 1) exit(0);
    }

    /* Carregar mapa e avaliar somas e tendas adjacentes */
    res = VerSomasAdjacentes_fromFile(Ltents, Ctents);
    free(Ltents);
    free(Ctents);

    if(res != 0) {
        _free_matriz();
        return res;
    }

    for(i=0; i<L; i++) {
        for(j=0; j<C; j++) {
            if(Matriz[i][j] == 'T') {
                //stack = (unsigned char*) &res;
                if(isT_alone_iter(i, j) == 1) {
                    _free_matriz();
                    return 1;
                }
            }
        }
    }
    _free_matriz();
    return 0;
}


