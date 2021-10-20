#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>

#ifndef PORT
	#define PORT 9999
#endif

static int n;
static int y[2];
static int x[2];
static int bufBullet_Y[2] = {0, 0};
static int bufBullet_X[2] = {1, 1};
static int X[2] = {0, 0};
static int Y[2] = {-1, -1};
static int mass[2][2];
static int tank_1[3][3] = { '#', ' ', '#', '#', 'o', '#', '#', ' ', '#' };
static int tank_2[3][3] = { '#', '#', '#', ' ', 'o', ' ', '#', '#', '#' };
static int tank[2][3][3];
static int Tank_x[2][3];
static int Tank_y[2][3];
static int muzzleArray[2][3][3]  = {/*for the 1-st tank*/{{'0', '|', '0'}, {'0', '0', '0'}, {'0', '0', '0'}}, 
/*for 2-nd tank*/{{'0', '|', '0'}, {'0', '0', '0'}, {'0', '0', '0'}}};
static int win = 2;
static int boolean[2] = {0, 0};
static int boolean_for_bullet[2] = {0, 0};
static int bullet_end[2] = {0, 0};
static int work_flag = 1;
static int ch;
static int ch_temp[2];
static char *player1;
static char *player2;

void zeroMuzzleArray(){
	for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            muzzleArray[n][i][j] = '0';
}

void drawTank(int k){
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mvaddch(Tank_y[k][i], Tank_x[k][j], tank[k][i][j]);
}

void tankRotate(int boolean)
{
	if(boolean == 0 || boolean == 2)
		for(int j = 0; j < 3; j++)  // cycle by lines
			for(int i = 0; i < 3; i++) // cycle by columns
				tank[n][i][j] = tank_1[i][j];
	else
		for(int j = 0; j < 3; j++)  // cycle by lines
			for(int i = 0; i < 3; i++) // cycle by columns
				tank[n][i][j] = tank_2[i][j];
}


void make_tank(){
	if(ch_temp[n] != 's'){
		if(ch_temp[n] == KEY_UP || ch_temp[n] == KEY_LEFT){
			x[n] = Tank_x[n][0];
			y[n] = Tank_y[n][0];

			if(ch_temp[n] == KEY_UP){	
				boolean[n] = 0; //cause vertical

				tankRotate(boolean[n]);

				--y[n];
				if(y[n] < 0)
					++y[n];
				else
					for(int i = 1; i >= 0; i--)
						Tank_y[n][i + 1] = Tank_y[n][i];
				Tank_y[n][0] = y[n];
			}

			if(ch_temp[n] == KEY_LEFT)
			{	
				boolean[n] = 1;
				tankRotate(boolean[n]);

				--x[n];
				if(x[n] < 0)
					++x[n];
				else
					for(int i = 1; i >= 0; i--)
						Tank_x[n][i + 1] = Tank_x[n][i];
				Tank_x[n][0] = x[n];
			}
		}

		if(ch_temp[n] == KEY_DOWN || ch_temp[n] == KEY_RIGHT){
			x[n] = Tank_x[n][2];
			y[n] = Tank_y[n][2];

			if(ch_temp[n] == KEY_DOWN){	
				boolean[n] = 0; //cause vertical
				tankRotate(boolean[n]);

				++y[n];
				if(y[n] > LINES - 1)
					--y[n];
				else
					for(int i = 1; i < 3; i++)
						Tank_y[n][i - 1] = Tank_y[n][i];
				Tank_y[n][2] = y[n];
			}
				
			if(ch_temp[n] == KEY_RIGHT){	
				boolean[n] = 1;
				tankRotate(boolean[n]);

				++x[n];
				if(x[n] > COLS - 1)
					--x[n];
				else
					for(int i = 1; i < 3; i++)
						Tank_x[n][i - 1] = Tank_x[n][i];
				Tank_x[n][2] = x[n];
			}
		}

		if(ch_temp[n] == 'a' || ch_temp[n] == 'q' || ch_temp[n] == 'w' || ch_temp[n] == 'e' || ch_temp[n] == 'd' || ch_temp[n] == 'c' || ch_temp[n] == 'x' || ch_temp[n] == 'z')
			zeroMuzzleArray(&muzzleArray[n][0][0]);

		if(ch_temp[n] == 'q')
			muzzleArray[n][0][0] = '\\';
		if(ch_temp[n] == 'w')
			muzzleArray[n][0][1] = '|';
		if(ch_temp[n] == 'e')
			muzzleArray[n][0][2] = '/';
		if(ch_temp[n] == 'd')
			muzzleArray[n][1][2] = '-';
		if(ch_temp[n] == 'c')
			muzzleArray[n][2][2] = '\\';
		if(ch_temp[n] == 'x')
			muzzleArray[n][2][1] = '|';
		if(ch_temp[n] == 'z')
			muzzleArray[n][2][0] = '/';
		if(ch_temp[n] == 'a')
			muzzleArray[n][1][0] = '-';

		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				if(boolean[n] == 1)
					tank[n][i][j] = tank_2[i][j];
				if(boolean[n] == 0)
					tank[n][i][j] = tank_1[i][j];
				if(muzzleArray[n][i][j] != '0')
					tank[n][i][j] = muzzleArray[n][i][j];
			}
		}
		ch_temp[n] = '0';
	}
}

void* win_table(){
	while(1){
		if(win == 0){
			mvaddstr(LINES/2 - 7, COLS/2 - 8, "1-st tank wins!");
			pthread_exit(0);
		}
		if(win == 1){
			mvaddstr(LINES/2 - 7, COLS/2 - 8, "2-nd tank wins!");
			pthread_exit(0);
		}
		if(ch == 'p') pthread_exit(0);
	}
}

void simple_func(int a, int b, int i, int j){
	Y[n] = a;
	X[n] = b;
	mass[n][0] = Tank_y[n][i];
	mass[n][1] = Tank_x[n][j];
	bufBullet_Y[n] = i;
	bufBullet_X[n] = j;
}

void* bullet(){
	while(1){
		if(win != 2 || ch == 'p') break;
		if(ch_temp[n] == 's' && bullet_end[n] == 0){
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++){
					if(tank[n][i][j] == '-' && i == 1 && j == 2)
				 		simple_func(0, 1, i, j);
					if(tank[n][i][j] == '-' && i == 1 && j == 0)
			 			simple_func(0, -1, i, j);
					if(tank[n][i][j] == '\\' && i == 0 && j == 0)
			 			simple_func(-1, -1, i, j);
					if(tank[n][i][j] == '\\' && i == 2 && j == 2)
			 			simple_func(1, 1, i, j);
					if(tank[n][i][j] == '/' && i == 0 && j == 2)
			 			simple_func(-1, 1, i, j);
					if(tank[n][i][j] == '/' && i == 2 && j == 0)
				 		simple_func(1, -1, i, j);
					if(tank[n][i][j] == '|' && i == 0 && j == 1)
				 		simple_func(-1, 0, i, j);
					if(tank[n][i][j] == '|' && i == 2 && j == 1)
						simple_func(1, 0, i, j);
				}
			}
			bullet_end[n] = 1;
			boolean_for_bullet[n] = 1;
			while(1){
				if(ch == 'p') break;

				mass[n][0] += Y[n];
				mass[n][1] += X[n];
				if(mass[n][0] == LINES + 1 || mass[n][1] == COLS + 1 || mass[n][0] == -1 || mass[n][1] == -1){
					bullet_end[n] = 0;
					break;
				}
				if((mass[n][1] == Tank_x[(n+1)%2][0] || mass[n][1] == Tank_x[(n+1)%2][1] || mass[n][1] == Tank_x[(n+1)%2][2]) && (mass[n][0] == Tank_y[(n+1)%2][0] || mass[n][0] == Tank_y[(n+1)%2][1] || mass[n][0] == Tank_y[(n+1)%2][2])){
					win = n;
					break;
				}
				usleep(40000);
			}
			mass[n][0] = Tank_y[n][bufBullet_Y[n]];
			mass[n][1] = Tank_x[n][bufBullet_X[n]];
			ch_temp[n] = '0';
		}
	}
}

void* draw(){
	while(1){	
		if(win != 2 || ch == 'p') break;
		clear();
		drawTank(0);
		drawTank(1);
		if(boolean_for_bullet[0] == 1 && bullet_end[0] == 1)
			mvaddch(mass[0][0], mass[0][1], '*');
		if(boolean_for_bullet[1] == 1 && bullet_end[1] == 1)
			mvaddch(mass[1][0], mass[1][1], '*');
		usleep(30000);
		refresh();
	}
}

void* udp_server(void* none){
    int sock, addr_len, ret;
    char buffer[4];
    fd_set readfd;
    struct sockaddr_in addr;
  
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if ( 0 > sock ) {
        perror("socket");
        return NULL;
    }

    addr_len = sizeof(struct sockaddr_in);
    memset((void*)&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if ( bind(sock,(struct sockaddr*)&addr, addr_len) < 0) {
        perror("bind");
        close(sock);
        return NULL;
    }

    while (work_flag){
    	if(ch == 'p') break;
        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        ret = select(sock+1, &readfd, NULL, NULL, 0);
        if (ret > 0){
            if (FD_ISSET(sock, &readfd)){
                recvfrom(sock, buffer, 4, 0, (struct sockaddr*)&addr, &addr_len);   
                if(strcmp(player1,inet_ntoa(addr.sin_addr))==0){
                	n = 0;
                	if(*((int*)buffer) == 's' && bullet_end[n] == 1)
						ch_temp[n] = '0';
					else
						ch_temp[n] = *((int*)buffer);
                	make_tank();
                }
               	if(strcmp(player2,inet_ntoa(addr.sin_addr))==0){
               		n = 1;
               		if(*((int*)buffer) == 's' && bullet_end[n] == 1)
						ch_temp[n] = '0';
					else
						ch_temp[n] = *((int*)buffer);
               		make_tank();
               	}
                memset(buffer, 0, sizeof(buffer));
            }
        }
    }
    close(sock);
    return (void*)NULL;
}

int main(int argc, char* argv[])
{	
	if(argc < 3 || argc > 3){
		perror("argc");
		exit(0);
	}
	player1 = malloc(sizeof(argv[1]));
	player2 = malloc(sizeof(argv[2]));
	player1 = argv[1];
	player2 = argv[2];

    int sock, yes = 1;
    pthread_t pid;
    struct sockaddr_in addr;
    int addr_len;
    int count;
    int ret;
    fd_set readfd;

    pthread_create(&pid, NULL,udp_server,NULL);
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock");
        return -1;
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
    if (ret == -1){
        perror("setsockopt");
        return 0;
    }

    addr_len = sizeof(struct sockaddr_in); 
    memset((void*)&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addr.sin_port = htons(PORT);

	initscr();
	keypad(stdscr, TRUE);
	curs_set(0);
	noecho();

	mvaddstr(LINES/2 - 7, COLS/2 - 12, "ENTER ANY KEY TO  START!");

	y[0] = LINES/2 - 1;
	x[0] = 9;

	y[1] = LINES/2 - 1;
	x[1] = COLS - 12;

	for(int i = 0; i < 3; i++){
		Tank_x[0][i] = x[0] + i;
		Tank_y[0][i] = y[0] + i;
	}
	for(int i = 0; i < 3; i++){
		Tank_x[1][i] = x[1] + i;
		Tank_y[1][i] = y[1] + i;
	}

	n = 0;
	tankRotate(boolean[0]);
	tank[0][0][1] = '|';
	mass[0][0] = Tank_y[0][0];
	mass[0][1] = Tank_x[0][1];
	bufBullet_Y[0] = 0;
	bufBullet_X[0] = 1;

	n = 1;
	tankRotate(boolean[1]);
	tank[1][0][1] = '|';
	mass[1][0] = Tank_y[1][0];
	mass[1][1] = Tank_x[1][1];
	bufBullet_Y[1] = 0;
	bufBullet_X[1] = 1;

	pthread_t thread_tank;
	pthread_t thread_win;
	pthread_t thread_bullet;

	pthread_create(&thread_tank, NULL, draw, NULL);
	pthread_create(&thread_win, NULL, win_table, NULL);
	pthread_create(&thread_bullet, NULL, bullet, NULL);

	while(work_flag && (ch = getch()) != 'p'){
		sendto(sock, &ch, 4, 0, (struct sockaddr*) &addr, addr_len);
		ch = '0';
		if(win != 2)break;
	}
	pthread_join(thread_bullet, NULL);
	pthread_join(thread_tank, NULL);
	pthread_join(thread_win, NULL);
	close(sock);
	endwin();
	return 0;
}