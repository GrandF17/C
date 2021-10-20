#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#define N 4

void matrixDisplay(int cursorY, int cursorX, int y, int x, int **matrix, int matrixBool, int ch)
{
	if(cursorX * cursorY != 0){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				if(y + i == cursorY && x + 1 + 2 * j == cursorX) matrix[i][j] = ch - 48;
			}
		}
	}

	if(matrixBool == 0) mvaddch(4, N, 'C');
	else mvaddch(4 + N + 2, N, 'R');

	for(int i=0;i<N;i++){
		mvaddch(y + i, x, '|');
		mvaddch(y + i, 2 * N + x, '|');
		for(int j = 0; j < N; j++) mvaddch(y + i, x + 1 + 2 * j, matrix[i][j] + '0');
	}
	refresh();
}

void vectorDisplay(int cursorY, int cursorX, int y, int x, int *vector, int vectorBool, int ch)	//vectorBool decides which vector to print	
{
	if(cursorX * cursorY != 0){
		for(int i = 0; i < N; i++){
			if(cursorX == x + 3 + 2 * i && cursorY == y) vector[i] = ch - 48;
		}
	}

	if(vectorBool == 0) mvaddstr(y, x, "T (");
	else mvaddstr(y, x, "A (");

	for(int i = 0; i < N + 1; i++){
		if(i != N) mvaddch(y, 3 + 2 * i, vector[i] + '0');
		if(i == N) mvaddch(y, 3 + 2 * i - 1, ')');
	}
	refresh();
}

int main(int argc, char* argv[])
{
	int ch = 0, vectorT_boolean, matrixR_boolean, cursor_boolean = 0, x = 0, y = 0;
	int *A = (int*)malloc(N * sizeof(int));
	int *T = (int*)malloc(N * sizeof(int));
	int **C = (int**)malloc(sizeof(int*) * N);
	for(int i = 0; i < N; i++)
	    C[i] = (int*)malloc(N * sizeof(int));
	int **R = (int**)malloc(sizeof(int*) * N);
	for(int i = 0; i < N; i++)
	    R[i] = (int*)malloc(N * sizeof(int));
	
	// if cursor_boolean = 0 --> cursor off else --> cursor on //
	// if matrixR_boolean = 0 --> matrix R is fully consisting of zero digits //
	//	if vectorT_boolean = 0 --> vector T is fully consisting of zero digits //

	for(int i = 0; i < N; i++){	//assign a zero value to each cell of our matrices
		for(int j = 0; j < N; j++){
			C[i][j] = 0;
			R[i][j] = 0;
		}
	}

	for(int i = 0; i < N; i++){	//assign a zero value to each cell of our vectors
		A[i] = 0;
		T[i] = 0;
	}

	initscr();
	keypad(stdscr, TRUE);
	noecho();

	while(1){
		vectorDisplay(0/*cursorY*/, 0/*cursorX*/, 2/*y*/, 0/*x*/, T, 0/*vectorBool*/, '0'/*ch*/);
		vectorDisplay(0/*cursorY*/, 0/*cursorX*/, 0/*y*/, 0/*x*/, A, 1/*vectorBool*/, '0'/*ch*/);

		matrixDisplay(0/*cursorY*/, 0/*cursorX*/, 5/*y*/, 0/*x*/, C, 0/*vectorBool*/, '0'/*ch*/);	//printing our matrices
		matrixDisplay(0/*cursorY*/, 0/*cursorX*/, 4 + N + 2 + 1/*y*/, 0/*x*/,R, 1/*matrixBool*/, '0'/*ch*/);

		vectorT_boolean = 0;
		matrixR_boolean = 0;
		if(cursor_boolean == 1) //turns on our cursor
			curs_set(TRUE);

		while((ch = getch()) != 10)	//while !Enter
		{	
			if(ch <= 57 && ch >= 48)
			{
				if(cursor_boolean == 0) vectorDisplay(y/*cursorY*/, x/*cursorX*/, 2/*y*/, 0/*x*/, T, 0/*vectorBool*/, ch);
				matrixDisplay(y/*cursorY*/, x/*cursorX*/, 4 + N + 2 + 1/*y*/, 0/*x*/, R, 1/*matrixBool*/, ch/*ch*/);
			}

			if(ch == KEY_UP && y > 0)y--;
			if(ch == KEY_DOWN && y < LINES - 1)y++;
			if(ch == KEY_LEFT && x > 0)x--;
			if(ch == KEY_RIGHT && x < COLS - 1)x++;
			if(ch == 'q'){
				for (int i = 0; i < N; i++)
				    free(R[i]);
				for (int i = 0; i < N; i++)
				    free(C[i]);
				free(R);
				free(C);
				free(T);
				free(A);
				endwin();
				return 0;
			}
			
			move(y, x);
			refresh();
		}

		for(int i = 0; i < N; i++){	//check is there are some significant digits or not
			if(T[i] > 0) vectorT_boolean = 1;	
			for(int j = 0; j < N; j++)
				if(R[i][j] > 0) matrixR_boolean = 1;
		}
		if(vectorT_boolean == 0 || matrixR_boolean == 0){
			mvaddstr(LINES - 1, COLS/2 - 23, "Enter significant digits in vector T/matrix C.");
			refresh();
			continue;
		}
		curs_set(0);

		if(cursor_boolean == 0){
			for(int i = 0; i < N; i++){
				A[i] = T[i];
				mvaddch(0, 5 + 2 * i, A[i] + '0');
			}
		}

		clear();
		refresh();

		for(int i = 0; i < N; i++){
			sleep(1);
			for(int j = 0; j < N; j++){
				if(R[i][j] > A[j]){	
					clear();
					mvaddstr(LINES - 1, COLS/2 - 5, "Deadlocked.");
					
					for (int i = 0; i < N; i++)
					    free(R[i]);
					for (int i = 0; i < N; i++)
					    free(C[i]);
					free(R);
					free(C);
					free(T);
					free(A);

					refresh();
					getch();
					endwin();
					return 0;
				}
			}
			for(int j = 0; j < N; j++){
				A[j] -= R[i][j];
				C[i][j] += R[i][j];
			}

			vectorDisplay(0/*cursorY*/, 0/*cursorX*/, 0/*y*/, 0/*x*/, A, 1/*vectorBool*/, '0'/*ch*/);
			matrixDisplay(0/*cursorY*/, 0/*cursorX*/, 5/*y*/, 0/*x*/, C, 0/*vectorBool*/, '0'/*ch*/);

			usleep(100000);
		}
		mvaddstr(LINES - 1, COLS/2 - 7, "Not deadlocked.");
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				R[i][j] = 0;
			}
		}
		refresh();

		for(int i = 0; i < N; i ++)
			if(A[i] > 0) cursor_boolean = 1;
	}
}