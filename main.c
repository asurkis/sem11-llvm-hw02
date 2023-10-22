#include "sim.h"

int BUF[2][SIM_X_SIZE * SIM_Y_SIZE];
int *board = BUF[0];
int *board_next = BUF[1];

static int calcNeighbors(int row, int col) {
        int result = 0;
        for (int d_row = -1; d_row <= 1; ++d_row) {
                for (int d_col = -1; d_col <= 1; ++d_col) {
                        int rr = row + d_row;
                        int cc = col + d_col;
                        int fits_v = 0 <= rr && rr < SIM_Y_SIZE;
                        int fits_h = 0 <= cc && cc < SIM_X_SIZE;
                        if (fits_v && fits_h) {
                                result += board[rr * SIM_X_SIZE + cc];
                        }
                }
        }
        return result;
}

static void fillNextState() {
        for (int row = 0; row < SIM_Y_SIZE; ++row) {
                for (int col = 0; col < SIM_X_SIZE; ++col) {
                        int neighbors = calcNeighbors(row, col);
                        int idx = row * SIM_X_SIZE + col;
                        if (board[idx]) {
                                // Self is also counted
                                int is3or4 = neighbors == 3 || neighbors == 4;
                                board_next[idx] = is3or4 ? 1 : 0;
                        } else {
                                board_next[idx] = neighbors == 3 ? 1 : 0;
                        }
                        simSetPixel(col, row, board_next[idx] ? 0 : 0xFFFFFF);
                }
        }
}

static void initBoard() {
        // Add a glider
        board[1] = 1;
        board[2 + SIM_X_SIZE] = 1;
        board[0 + 2 * SIM_X_SIZE] = 1;
        board[1 + 2 * SIM_X_SIZE] = 1;
        board[2 + 2 * SIM_X_SIZE] = 1;
}

int main(int _argc, char *_argv[]) {
        simBegin();
        initBoard();
        while (simShouldContinue()) {
                fillNextState();
                int *tmp = board;
                board = board_next;
                board_next = tmp;
                simFlush();
        }
        simEnd();
        return 0;
}
