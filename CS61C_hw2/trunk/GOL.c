#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define 	ALIVE	1
#define		DEAD	0

char *inputFile;
char baseFileName[100];
int *bornRule, *dieRule, *currBoard, *nextBoard;
int maxGens, bornRules, dieRules, boardRows, boardCols;

/* ======================
 * Board Data Abstraction */

//int position(int row, int col) { return (row * boardCols) + col;}

int getVal(int *board, int row, int col) {
	row = (row + boardRows)%boardRows;
	col = (col + boardCols)%boardCols;
	return board[(row*boardCols) + col];
	//if (row > -1 && col > -1) return board[position(row%boardRows,col%boardCols)];
	//if (row < 0 && col > -1) return board[position(boardRows + row,col%boardCols)];
	//if (row > -1 && col < 0) return board[position(row%boardRows,boardCols + col)];
	//if (row < 0 && col < 0)	return board[position(boardRows + row,boardCols + col)];
	return -1;
}

int* makeBoard(int rows, int cols) {
	int* board = (int *) malloc(rows*cols*sizeof(int));
	if (board == NULL) {
		printf("COULD NOT FIND MEMORY!");
		return 0;
	}
	return board;

	}

int setVal(int *board, int row, int col, int val) {
	//if (!(val == DEAD || val == ALIVE)) return 1;
	board[(row*boardCols) + col] = val;
	return 0;
}

int countNeighbors(int *board, int row, int col) {
	int i;
	i = getVal(board, row+1, col);
	i += getVal(board, row-1, col);
	i += getVal(board, row, col+1);
	i += getVal(board, row, col-1);
	i += getVal(board, row+1, col+1);
	i += getVal(board, row+1, col-1);
	i += getVal(board, row-1, col+1);
	i += getVal(board, row-1, col-1);
	return i;
}

/* Board Data Abstraction End
 * ========================== */

void writeFile(char* filenameRoot, int genNum, int* board) {
    char buff[1000];
    FILE* file;
    sprintf(buff, "%s%d.pbm", filenameRoot, genNum);
    file = fopen(buff, "w");
    fprintf(file, "P1 %d %d ## Generation %d of Conway's Game of Life by cs61c-ca\n", boardCols, boardRows, genNum);

    int r, c;
	for(r=0; r<boardRows; r++) {
		c = 0;
		fprintf(file, "%d", getVal(board,r,c));
		for(c=1; c<boardCols; c++)
			fprintf(file, " %d", getVal(board,r,c));
		fprintf(file, "\n");
	}

    fclose(file);
}

int readFile(char* fileName) {
    FILE * file = fopen(fileName, "r");
    if (file == NULL) return 1;
    fscanf(file, "P1 %d %d\n", &boardCols, &boardRows);
	currBoard = makeBoard(boardRows, boardCols);
	nextBoard = makeBoard(boardRows, boardCols);

	int r=0,c=0,val=-1;
	while ((val = fgetc(file)) != EOF) {
		if (val == '\n') {
			c = 0;
			r++;
		} else if (val == ' ' || val == '\r') {
			;
		} else if (val == '0' || val == '1') {
			if (setVal(currBoard, r, c, val - '0')) {
				fclose(file);
				return 1;
			}
			c++;
		} else {
			;
		}

	}
    fclose(file);
    return 0;
}

/* Reads the command line arguments into global variables. */
int readArgs(char** argv) {
	int n;
	inputFile = argv[1];
	bornRules = strlen(argv[2]);
	dieRules = strlen(argv[3]);
	bornRule = (int *) malloc(bornRules*sizeof(int));
	dieRule = (int *) malloc(dieRules*sizeof(int));
	maxGens = atoi(argv[4]);

	for (n = 0; n < strlen(argv[1])-4; n++)
		baseFileName[n] = argv[1][n];
	baseFileName[n] = '\0';

	for (n = 0; n < strlen(argv[2]); n++)
		*(bornRule + n) = argv[2][n] - '0';

	for (n = 0; n < strlen(argv[3]); n++)
		*(dieRule + n) = argv[3][n] - '0';

	return 0;
}

int contains(int val, int* array, int arrayLen) {
	for (int count = 0; count < arrayLen; count++) if (array[count] == val) return 1;
	return 0;
}
/* Only looks at current board, only updates next board, swaps at end. */
int simulate() {
	int count, r, c, i;
	int* temp;
	for (count = 0; count <= maxGens; count++) {
		writeFile(baseFileName, count, currBoard);
		for (r = 0; r < boardRows; r++) {
			for (c = 0; c < boardCols; c++) {
				i = countNeighbors(currBoard, r, c);
				if (getVal(currBoard, r, c) == ALIVE) {
					if (contains(i, dieRule, dieRules))
						setVal(nextBoard, r, c, DEAD);
					else
						setVal(nextBoard, r, c, ALIVE);
				} else {
					if (contains(i, bornRule, bornRules))
						setVal(nextBoard, r, c, ALIVE);
					else
						setVal(nextBoard, r, c, DEAD);
				};
			}

		}
		temp = nextBoard;
		nextBoard = currBoard;
		currBoard = temp;
	}
	return 0;
}

/* Our Main Program */
int main(int argc, char** argv) {
	if (argc != 5 || readArgs(argv)) goto error;
	if (readFile(inputFile)) goto error;
	if (simulate()) goto error;
    return 0;

	error:	//User Input Error Case:
	puts("  Usage: GOL <file> <bornRule> <dieRule> <max generations>\n    This program simulates Conway's Game of Life, a cellular automaton devised\n    by British Mathematician John Horton Conway in 1970. The rules for this\n    zero-player game are the standard rules as described in the Wikipedia article\n    about this game: http://en.wikipedia.org/wiki/Conway%27s_game_of_life. The\n    only difference is births and deaths are determined by the two strings of\n    digits passed as arguments.\n\n    For example, if 34 is passed as <bornRule>, a cell will be born if and\n    only if it has 3 or 4 living neighbors. The rule works similarly for deaths.\n    The standard rules would be represented as <bornRule> = 3 and <dieRule> =\n    0145678.\n\n    This program will take as input a plain PBM file of the format:\n\n      P1 <columns> <rows>\n      <bits for each row indicating a living cell (1) or dead cell (0)>\n\n    and output one PBM file (with identical formatting to the above) for each\n    generation of cells in the game until you reach max generations. The PBM\n    produced will have a comment in the first line indicating the programmer's\n    login and the generation number of the current file. These files will be\n    named the same as the input, but with the generation number appended before\n    the \".pbm\". For example, if the input file is \"spaceship.pbm\", the files\n    written will be named \"spaceship0.pbm\", \"spaceship1.pbm\", etc. Generation \n    0 is the configuration initially given to the program.\n\n    PBM files can be easily viewed using the xv command or converted to another\n    format using the pbmto* and ppmto* utilities. A plain ascii PBM file can be\n    created by adding a header line \"P1 <WIDTH> <HEIGHT> and followed by\n    a grid of data (0 = dead = white, 1 = live = black). A call to man pbm can\n    help answer questions about the format.\n\n    Arguments:\n      <file> is the location of a .pbm file to be used as input to the\n      program. This image represents generation 0 for the simulation.\n\n      <bornRule> is the born rule as explained above\n\n      <dieRule> is the die rule as explained above\n\n      <max generations> is the maximum number of generations to generate\n\n    Examples:\n      unix% GOL diesFast.pbm 3 0145678 3\n      unix%\n\n      diesFast.pbm contains the following:\n      P1 4 4\n      0 0 0 0\n      0 0 1 0\n      0 0 0 0\n      0 0 0 0\n\n      The files output will be:\n\n      diesFast0.pbm:\n      P1 4 4 ## Generation 0 of Conway's Game of Life by cs61c-tb\n      0 0 0 0\n      0 0 1 0\n      0 0 0 0\n      0 0 0 0\n\n      diesFast1.pbm:\n      P1 4 4 ## Generation 1 of Conway's Game of Life by cs61c-tb\n      0 0 0 0\n      0 0 0 0\n      0 0 0 0\n      0 0 0 0\n\n      diesFast2.pbm:\n      P1 4 4 ## Generation 2 of Conway's Game of Life by cs61c-tb\n      0 0 0 0\n      0 0 0 0\n      0 0 0 0\n      0 0 0 0\n\n");
	return 1;
}
