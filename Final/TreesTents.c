#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TreesTents.h"
#include "generalStack.h"

#define BUFFER_SIZE 1024 * 16

#define DIR 2 // direita, esquerda, cima e baixo
#define ESQ -2
#define CIMA 1
#define BAIXO -1

void _free_matriz(char **Matriz, int L) {
  int i;

  if (Matriz == NULL)
    return;
  for (i = 0; i < L; i++) {
    if (Matriz[i] == NULL)
      break;
    free(Matriz[i]);
  }
  free(Matriz);
}

void handle_int(int res, int i, int *somaL, int *somaC, int *Lrests,
                int *Crests, int L, int C) {
  if (i < L) {
    Lrests[i] = res;
    *somaL += res;
  } else if (i - L < C) {
    Crests[i - L] = res;
    *somaC += res;
  }
}
// Descrição: Lẽ as hints do ficheiro e soma os vetores.
// Argumentos:
// Retorno: -2 se ocorrer erro, se não devolve o número de tendas ou -1 caso os
// vetores sejam coerentes.
int Fill_Hints_checkSums(FILE *fp, int L, int C, int *Lrests, int *Crests) {
  int i, n, res, somaL = 0, somaC = 0;
  int block_to_read, nbytes, bytes_skiped, digits_read, bytes_consumed;
  char *buffer, *pre_buffer, *end_ptr;

  n = (2 * L + 2 * C) / BUFFER_SIZE;
  block_to_read = BUFFER_SIZE;
  if (n == 0) {
    block_to_read = (2 * L + 2 * C);
    n = 1;
  }
  // The block is 16 extra chars before a buffer of size block_to_read, eith an
  // extra char in the end for '\0'.
  pre_buffer = (char *)malloc(16 + block_to_read + 1);
  if (pre_buffer == NULL)
    return -2;
  buffer = pre_buffer + 16;
  buffer[block_to_read] = '\0';

  i = 0;
  bytes_skiped = -1;
  digits_read = 0;
  while (1) {
    for (; n > 0; n--) {
      nbytes = fread(buffer, 1, block_to_read, fp);
      if (nbytes != block_to_read)
        return -2;
      bytes_consumed = 0;
      // Check if not first time
      if (digits_read != 0) {
        // Read the number that was supposed to be read last time
        res = strtol(buffer - digits_read, &end_ptr, 10);
        bytes_consumed = end_ptr - buffer;
        handle_int(res, i, &somaL, &somaC, Lrests, Crests, L, C);
        i++;
      }
      // Read buffer int by int
      for (; i < L + C; i++) {
        sscanf(buffer + bytes_consumed, " %n", &bytes_skiped);
        bytes_consumed += bytes_skiped;
        res = strtol(buffer + bytes_consumed, &end_ptr, 10);
        if (end_ptr == &buffer[block_to_read]) {
          digits_read = end_ptr - buffer - bytes_consumed;
          // Write part of number into pre_buffer
          memcpy(buffer - digits_read, buffer + bytes_consumed, digits_read);
          break;
        }
        bytes_consumed = end_ptr - buffer;
        handle_int(res, i, &somaL, &somaC, Lrests, Crests, L, C);
      }
    }
    // Se só restam 3 -> ler o resto diretamente
    if (L + C - i < 4)
      break;
    n = 2 * (L + C - i) / BUFFER_SIZE;
    block_to_read = BUFFER_SIZE;
    if (n == 0) {
      block_to_read = 2 * (L + C - i);
      n = 1;
    }
    buffer[block_to_read] = '\0';
  }
  if (i != L + C) {
    if (digits_read != 0) {
      // Read the number that was partially read last time
      if (fscanf(fp, "%[0-9]", buffer) != 1)
        buffer[0] = '\0';
      res = strtol(buffer - digits_read, &end_ptr, 10);
      handle_int(res, i, &somaL, &somaC, Lrests, Crests, L, C);
      i++;
    }
    // Read remaining ints one by one
    for (; i < L + C; i++) {
      if (fscanf(fp, " %d", &res) != 1)
        return -2;
      handle_int(res, i, &somaL, &somaC, Lrests, Crests, L, C);
    }
  }
  free(pre_buffer);
  if (somaL != somaC)
    return -1;
  return somaL;
}

// Descrição: Lê a matriz do ficheiro e conta as árvores.
// Argumentos:
// Retorno: O número de árvores se ler bem, -1 se chegar ao fim do ficheiro ou
// erro de alocação.
int Fill_Matriz_easy(FILE *fp, char **Matriz, int L, int C) {
  int i, j, arvores = 0;
  char format[32];
  sprintf(format, " %%%dc", C);

  for (i = 0; i < L; i++) {
    Matriz[i] = (char *)malloc(C * sizeof(char));
    if (Matriz[i] == NULL)
      return -1;
    if (fscanf(fp, format, Matriz[i]) != 1)
      return -1;
    for (j = 0; j < C; j++) {
      if (Matriz[i][j] == 'A')
        arvores++;
    }
  }
  return arvores;
}

/*int check_linha_coluna(int l0, int c0, char **Matriz, int L, int C,
                       int Lrests_l0, int Crests_c0) {
  int i, j, somaL = 0, somaC = 0;
  for (i = l0, j = 0; j < c0 - 1; j++) {
    if (Matriz[i][j] == 'T' || Matriz[i][j] == 't')
      somaL++;
  }
  for (j = c0 + 2; j < C; j++) {
    if (Matriz[i][j] == 'T' || Matriz[i][j] == 't')
      somaL++;
  }
  for (i = 0, j = c0; i < l0 - 1; i++) {
    if (Matriz[i][j] == 'T' || Matriz[i][j] == 't')
      somaC++;
  }
  for (i = l0 + 2; i < L; i++) {
    if (Matriz[i][j] == 'T' || Matriz[i][j] == 't')
      somaC++;
  }

  if (somaC >= Crests_c0 || somaL >= Lrests_l0)
    return 0;
  return 1;
}*/

void move_dir(int *l_out, int *c_out, char dir) {
  if (dir == CIMA) {
    *l_out += -2;
  } else if (dir == BAIXO) {
    *l_out += 2;
  } else if (dir == ESQ) {
    *c_out += -2;
  } else
    *c_out += 2;
}

// Descrição: Determina se esta tenda possui árvores adjacentes disponíveis.
// Altera a matriz. Argumentos: Linha e coluna da tenda. Retorno: 0 caso a tenda
// tenha árvore disponível, 1 caso contrário.
char isT_alone_iter(int l0, int c0, char **Matriz, int L, int C) {
  char from = 0;
  Stack *charStack;
  charStack = initStack(8, 1);
  while (1) {
    Matriz[l0][c0] = TMP_T;
    // Ver a direita
    if (c0 != C - 1) {
      if (Matriz[l0][c0 + 1] == 'A') {
        Matriz[l0][c0 + 1] = TMP_A;
        if (c0 == C - 2) {
          freeStack(charStack);
          return 0;
        }
        if (Matriz[l0][c0 + 2] != 'T') {
          freeStack(charStack);
          return 0;
        }
        push(charStack, &from);
        from = ESQ;
        move_dir(&l0, &c0, DIR);
        continue;
      }
    }
    // Ver a baixo
    if (l0 != L - 1) {
      if (Matriz[l0 + 1][c0] == 'A') {
        Matriz[l0 + 1][c0] = TMP_A;
        if (l0 == L - 2) {
          freeStack(charStack);
          return 0;
        }
        if (Matriz[l0 + 2][c0] != 'T') {
          freeStack(charStack);
          return 0;
        }
        push(charStack, &from);
        from = CIMA;
        move_dir(&l0, &c0, BAIXO);
        continue;
      }
    }
    // Ver a esquerda
    if (c0 != 0) {
      if (Matriz[l0][c0 - 1] == 'A') {
        Matriz[l0][c0 - 1] = TMP_A;
        if (c0 == 1) {
          freeStack(charStack);
          return 0;
        }
        if (Matriz[l0][c0 - 2] != 'T') {
          freeStack(charStack);
          return 0;
        }
        push(charStack, &from);
        from = DIR;
        move_dir(&l0, &c0, ESQ);
        continue;
      }
    }
    // Ver a cima
    if (l0 != 0) {
      if (Matriz[l0 - 1][c0] == 'A') {
        Matriz[l0 - 1][c0] = TMP_A;
        if (l0 == 1) {
          freeStack(charStack);
          return 0;
        }
        if (Matriz[l0 - 2][c0] != 'T') {
          freeStack(charStack);
          return 0;
        }

        push(charStack, &from);
        from = BAIXO;
        move_dir(&l0, &c0, CIMA);
        continue;
      }
    }
    if (from == 0) {
      freeStack(charStack);
      return 1;
    }
    move_dir(&l0, &c0, from);
    pop(charStack, &from);
  }
}
/* Descrição: 1 se c for uma tenda, 0 se não.
 * */
int isTent(char c) {
  return c == 't' || c == 'T' || c == NEW_T_UNPAIRED || c == NEW_T_PAIRED;
}

/* Descrição: 1 se c for uma tenda com par, 0 se não.
 * */
int isPairedTent(char c) {
  return c == 't' || c == NEW_T_PAIRED;
}
/* Descrição: 1 se c for uma nova tenda, 0 se não.
 * */
int isNewTent(char c) {
  return c == NEW_T_UNPAIRED || c == NEW_T_PAIRED;
}
void repair_matriz(char **Matriz, int L, int C) {
  int i, j;
  for (i = 0; i < L; i++) {
    for (j = 0; j < C; j++) {
      if (Matriz[i][j] == TMP_T)
        Matriz[i][j] = 'T';
      else if (Matriz[i][j] == TMP_A)
        Matriz[i][j] = 'A';
    }
  }
}

/* Descrição: Prepara matriz para escrita,
 * troca os char's de processamento para os seus valores finais.
 * */
void beautify_matriz(char **Matriz, int L, int C) {
  int i, j;
  for (i = 0; i < L; i++) {
    for (j = 0; j < C; j++) {
      if (Matriz[i][j] == 't')
        Matriz[i][j] = 'T';
      else if (Matriz[i][j] == 'a')
        Matriz[i][j] = 'A';
      else if (Matriz[i][j] != 'T' && Matriz[i][j] != 'A')
        Matriz[i][j] = '.';
    }
  }
}

/* atualiza os Opens around (x, y)*/
void add_around(int x, int y, int value, char **Matriz, int L, int C) {
  int j = 0;
  char c;
  if (y + 1 < C) {
    c = Matriz[x][y + 1];
    if (c >= '0' && c <= '8')
      Matriz[x][y + 1] += value;
  }
  if (x + 1 < L) {
    for (j = -1; j < 2; j++) {
      if (y + j >= 0 && y + j < C) {
        c = Matriz[x + 1][y + j];
        if (c >= '0' && c <= '8')
          Matriz[x + 1][y + j] += value;
      }
    }
  }
  return;
}

// Ferramenta para debug
void printMatriz(char **Matriz, int L, int C) {
  int i;
  char format[32];
  sprintf(format, " %%.%ds\n", C);
  for (i = 0; i < L; i++) {
    printf(format, Matriz[i]);
  }
  printf("\n");
}

/* Descrição: Verifica se pode haver tenda nas quatro direções,
 * coloca '0' nos que puderem
 * Argumentos: Posição (x,y) da árvore a ser verificada
 * */
void check_adj_for_opens(int x, int y, char **Matriz, int L, int C, int *Lrests,
                         int *Crests) {
  if (Crests[y] > 0) {
    if (x > 0) {
      if (Lrests[x - 1] > 0 && Matriz[x - 1][y] == '.' &&
          sem_tendas_adj(x - 1, y, Matriz, L, C)) {
        Matriz[x - 1][y] = '0';
      }
    }
    if (x < L - 1) {
      if (Lrests[x + 1] > 0 && Matriz[x + 1][y] == '.' &&
          sem_tendas_adj(x + 1, y, Matriz, L, C)) {
        Matriz[x + 1][y] = '0';
      }
    }
  }

  if (Lrests[x] > 0) {
    if (y > 0) {
      if (Crests[y - 1] > 0 && Matriz[x][y - 1] == '.' &&
          sem_tendas_adj(x, y - 1, Matriz, L, C)) {
        Matriz[x][y - 1] = '0';
      }
    }
    if (y < C - 1) {
      if (Crests[y + 1] > 0 && Matriz[x][y + 1] == '.' &&
          sem_tendas_adj(x, y + 1, Matriz, L, C)) {
        Matriz[x][y + 1] = '0';
      }
    }
  }
}
/* Descrição: Verifica tendas em todas posições adjacentes
 * Argumentos: Posição (x,y) do mapa
 * Retorno: Retorna 0 se existir pelo menos uma tenda na adjacência, 1 se não
 * existir nenhuma tenda
 * */
int sem_tendas_adj(int x, int y, char **Matriz, int L, int C) {
  /* vê tendas nas adjacentes */
  int i, j;

  for (i = -1; i <= 1; i++) {
    for (j = -1; j <= 1; j++) {
      if (x + i >= 0 && x + i < L && y + j >= 0 && y + j < C) {
        if (Matriz[x + i][y + j] == 'T' || Matriz[x + i][y + j] == 't')
          return 0;
      }
    }
  }
  return 1;
}
