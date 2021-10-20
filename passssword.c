#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 256
#define MD5_LEN 32

void smth1(int a/*detecting where to go and what to do*/, int choice, char* str1, int strlen)
{	
	switch (a){
		case 1:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Trying to hack me? 0_o\n");
		}
		break;

		case 2:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("You won't hack me, you know?\n");
		}
		break;

		case 3:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("You've just got confused:)\n");
		}
		break;

		case 4:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Are you still here?\n");
		}
		break;

		case 5:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Nope? Still no access?\n");
		}
		break;

		case 6:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Still trying?\n");
		}
		break;

		case 7:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Give up!\n");
		}
		break;

		case 8:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("No chance to break it!\n");
		}
		break;

		case 9:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			for(int i = 0; i < 1000; i++)
			{
				printf("-_-_-_-_-_-_-_-_-_-_-_-_LMAO-_-_-_-_-_-_-_-_-_-_-_-_");
			}
		}
		break;

		case 10:
		if(choice == 0){
			for(int i = 0; i < strlen; i++){
		    	printf("%c", str1[i]);
		    }
		    printf("\n");
		}
		else{
			printf("Access a... NO, NOT ALLOWED!!!\n");
		}
		break;
	}
}

int smth2(char *file_name, char *md5_sum)
{
    #define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
    char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
    sprintf(cmd, MD5SUM_CMD_FMT, file_name);
    #undef MD5SUM_CMD_FMT

    FILE *p = popen(cmd, "r");
    if (p == NULL) return 0;

    int i, ch;
    for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++){
        *md5_sum++ = ch;
    }

    *md5_sum = '\0';
    pclose(p);
    if(remove ("1.txt") == -1){		//deleting our file with "Access allowed!"
    	perror("Delete");
    	return 0;
	}
    return i == MD5_LEN;
}

int main(int argc,char* argv[])
{
    int j = 0;
    char str1[] = { 49, 2, 16, 22, 4, 28, 82, 5, 28, 13, 28, 4, 18, 11, 83, 100};
    char* str2 = malloc(sizeof(char) * strlen(argv[1]));

    srand(time(NULL));
    int a = (rand() % 10 + 1);

    strcpy (str2, argv[1]);
    int fd; 
    if((fd = creat("1.txt", 0777)) == -1){	//creat file to search md5sum of its content
        perror("Creat");
        return -1;
    }

    for(int i = 0; i < sizeof(str1) - 1; i++){	//writing to our file decoded "Access allowed!"
    	if(str2[j] == '\0'){
    		j = 0;
    	}
    	str1[i] = str1[i] ^ str2[j];
    	write(fd, &str1[i], 1);
    	j++;
    }
    close(fd);
    system("clear");
    printf("Your password: '%s'\n\n", str2);
    memset(str2, 0, sizeof(str2));

    char md5[MD5_LEN + 1];
    if (!smth2("1.txt", md5)){	//function to search md5sum of our file
        perror("MD5SUM");
        return 0;
    }
    int choice = strcmp(md5, "e6e058b46f210f34639393574f37b601");	//trying to confuse our hacker
    int strlen = sizeof(str1) - 1;
    smth1(a, choice, str1, strlen);
    memset(str1, 0, sizeof(str1));
	return 0;
}