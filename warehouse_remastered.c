#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <semaphore.h>

#define N 4

sem_t semaphore;

static int counter = 0;

void printMassif(int n, int m, int x, int y, int* A)	//content of our warehouse
{
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			mvaddch( x + i, y + j, *(A + i * m + j));
		}
	}
}

void massifFilling(int x, int y, int n, int m, int boolean, int* A)
{	
	char ch;
	if(boolean == 1)
		ch = '*';
	else
		ch = '|';

	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			*(A + i * m + j) = ' ';	
		}
	}
	for(int i = n - 1; i >= n - x; i--){
		if(i == n - x){
			for(int j = m - 1; j >= m - y; j--){
				*(A + i * m + j) = ch;
			}
		}
		else{
			for(int j = m - 1; j >= 0; j--){
				*(A + i * m + j) = ch;
			}
		}
	}
}

void drawTheEdging(int n, int m, int x, int y)	//edging for our warehouse
{
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			if(i == 0 || i == n - 1 || j == 0 || j == m - 1){
				mvaddch( x + i, y + j, '#');
			}
		}
	}
}

void* count(void* args)
{
	int A[N][N], local, B[55][50];	//A - massif for our warehouse
									//B - massif for a queue of Customers/Fabricator
	initscr();

	keypad(stdscr, TRUE);
	noecho();
	refresh();

	while(1)
	{	
		clear();
		drawTheEdging(57, 52, 3, COLS - 54);
		drawTheEdging(57, 52, 3, 2);

		local = counter;
		if(local > 0 && local <= N * N){	
			int x = (local - (local % N)) / N + 1;
			int y = local % N;

			massifFilling(x, y, N, N, 1, &A[0][0]);

			drawTheEdging(N + 2, N + 2, LINES / 2 - (N / 2 + 1), COLS / 2 - (N / 2 + 1));
			printMassif(N, N, LINES / 2 - N / 2, COLS / 2 - N / 2, &A[0][0]);
		}
		else if( local < 0){	
			if(local > -(55 * 50)){
				mvaddstr( 2, 2, "Customers are awating for products...");
				drawTheEdging(N + 2, N + 2, LINES / 2 - (N / 2 + 1), COLS / 2 - (N / 2 + 1));

				int queueOfCustomers = 0 - local;
				int x = (queueOfCustomers - (queueOfCustomers % 50)) / 50 + 1;
				int y = queueOfCustomers % 50;

				massifFilling(x, y, 55, 50, 0, &B[0][0]);
				printMassif( 55, 50, 4, 3, &B[0][0]);
			}
			else{
				/*for(int i = 0; i < 55; i++){		//content of our warehouse
					for(int j = 0; j < 50; j++){
						mvaddch( 4 + i, 3 + j, '|');
					}
				}*/
				massifFilling(0, 0, 55, 50, 0, &B[0][0]);
				printMassif( 55, 50, 4, 3, &B[0][0]);
			}
		}
		else if( local > N * N){
			if(local < (55 * 50 + N * N)){
				mvaddstr( 2, COLS - 54, "Fabricators have more products..."); 	//full warehouse
				drawTheEdging(N + 2, N + 2, LINES / 2 - (N / 2 + 1), COLS / 2 - (N / 2 + 1));

				for(int i = 0; i < N; i++){		//content of our warehouse
					for(int j = 0; j < N; j++){
						mvaddch( LINES / 2 - N / 2 + i, COLS / 2 - N / 2 + j, '*');
					}
				}

				massifFilling(0, 0, N, N, 1, &B[0][0]);
				printMassif( N, N, LINES / 2 - N / 2, COLS / 2 - N / 2, &B[0][0]);

				local -= N * N;
				int x = (local - (local % 50)) / 50 + 1;
				int y = local % 50;

				massifFilling(x, y, 55, 50, 0, &B[0][0]);
				printMassif( 55, 50, 4, COLS - 53, &B[0][0]);
			}
			else{
				/*for(int i = 0; i < 55; i++){		//content of our warehouse
					for(int j = 0; j < 50; j++){
						mvaddch( 4 + i, COLS - 53 + j, '|');
					}
				}*/
				massifFilling(0, 0, 55, 50, 0, &B[0][0]);
				printMassif( 55, 50, 4, COLS - 53, &B[0][0]);
			}
		}
		refresh();
		usleep(10000);
		endwin();
	}
}

void* fabricator(void* args)
{
	int local;
	while(1){
		sem_wait(&semaphore);
		local = counter;
		local++;
		counter = local;
		usleep(10000);
		sem_post(&semaphore);
	}
}

void* customer(void* args)
{
	int local;
	while(1){
		sem_wait(&semaphore);
		local = counter;
		local--;
		counter = local;
		usleep(10000);
		sem_post(&semaphore);
	}
}

void main() 
{
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;

	sem_init(&semaphore, 0, 1);

	pthread_create(&thread1, NULL, fabricator, NULL);
	pthread_create(&thread2, NULL, customer, NULL);
	pthread_create(&thread3, NULL, count, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	sem_destroy(&semaphore);

	getch();    
    endwin();
}