#include <ncurses.h>
#define N 100


int main(int argc, char* argv[])
{
	int ch;

	initscr();

	int y = LINES/2;
	int x = COLS/2;
	int snake_x[N];
	int snake_y[N];

	for(int i = 0; i < N; i++)
	{
		mvaddch( y, ++x, '*');

		snake_x[i] = x;
		snake_y[i] = y;
	}

	keypad(stdscr, TRUE);
	noecho();
	refresh();

	while((ch = getch()) != 'q')
	{
		if(ch == KEY_UP)
		{
			--y;
			if(y < 0)
			{
				y = y + LINES;
			}
		}
		if(ch == KEY_DOWN)
		{
			++y;
			y = y % LINES;
		}
		if(ch == KEY_RIGHT)
		{	
			++x;
			x = x % COLS;
		}
		if(ch == KEY_LEFT)
		{	
			--x;
			if(x < 0)
			{
				x = x + COLS;
			}
		}

		for(int i = 0; i < N - 1; i++)
		{
			snake_x[i] = snake_x[i + 1];
			snake_y[i] = snake_y[i + 1];
		}

		snake_x[N - 1] = x;
		snake_y[N - 1] = y;

		clear();

		for(int i = 0; i < N; i++)
		{
			mvaddch( snake_y[i], snake_x[i], '*');
		}

	}

	endwin();
	return 0;
}