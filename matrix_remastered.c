#include <pthread.h>	//!!! THE ONLY ONE CODITION IS SPACE IN THE BEGGINIG OF ANY FILE ( 11 22 33\n)!!!
#include <stdlib.h>
#include <mcheck.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct{
	int rowN; //number of the processed line
	int colls; //number of columns
	int rows; //number of lines

	int** array1;	//pointers to matrices
	int** array2;
	int** resArr;
}pthrData;


void* threadFunc(void* thread_data)
{
	//getting a structure with data
	pthrData *data = (pthrData*) thread_data;

	for(int i = 0; i < data->rows; i++)
	{
	    for(int j = 0; j < data->colls; j++)
	    {
	        data->resArr[data->rowN][i] += data->array1[data->rowN][j] * data->array2[j][i];
	    }
	}
	
	return NULL;
}


int discover_n(char* file)
{
	int fd, n = 0;
    char buffer;

    if((fd=open(file, O_RDONLY)) == -1){
        perror("Open");
        return -1;
    }

    while (read(fd, &buffer, 1) != 0){
        if(buffer == '\n')
            n++;
    }
    close(fd);
    return n;
}


int discover_m(char* file)
{
	int fd, n = 0, m = 0, M = 0/*need for check*/, boolean = 0
    /*if boolean 0 it means that the previous symbol was NOT A DIGIT, 
    if 1 the previous symbol was A DIGIT*/;

    char buffer;
    if((fd=open(file, O_RDONLY)) == -1){
        perror("Open");
        return -1;
    }

        
    while (read(fd, &buffer, 1) != 0)
    {
        while(read(fd, &buffer, 1) == 1)
        {
            if(buffer == ' ' && buffer != '\n'){   
                boolean = 0;
            }
            if(buffer >= '0' && buffer <= '9' && boolean == 0){      
                m++;    	//digit counter
                boolean = 1;
            }
            if(buffer == '\n'){
                break;
            }
        }

        if(n != 0 && M == m || n == 0){   //check if number of digits in current line is the same as in the previous line
            M = m;
            m = 0;
            boolean = 0;
        }
        else{
            perror("Matrix incorrect");
            return -2;
        }
        n++;
    }
    close(fd);
    return M;
}


int main(int argc,char* argv[])
{	
	//          	   m_1  	                 m_2	
	//	      	    ---------     	           -------
	//	      	    | 1 2 3 |		  	       | 1 2 |	
	// matrix1=	n_1 | 4 5 6 |  matrix2=    n_2 | 3 4 |	m_1 need to be equal to n_2, if it`s true multiplication is possible
	//	      	    						   | 5 6 |
	//-_-_-_-_-_-_-_-_-_-_-_-_-Matrix A-_-_-_-_-_-_-_-_-_-_-_-_-
	int n_1 = discover_n(argv[1]);
	int m_1 = discover_m(argv[1]);
	if(n_1 < 0 || m_1 < 0){
		return 0;
	}

	FILE* input;
	input = fopen(argv[1], "r");
	if (input == 0){
	    perror("Open");
	    return -1;
	}

	int **A = (int**)malloc(sizeof(int*) * n_1);
	for(int i = 0; i < n_1; i++){
	    A[i] = (int*)malloc(m_1 * sizeof(int));
	}

	fseek(input, 0, SEEK_SET);
    for (int i = 0; i < n_1; ++i){		
    	for (int j = 0; j < m_1; ++j){
	    	fscanf(input, "%d", &A[i][j]);
	    }
    }
    fclose(input);

    printf("matrix A:\n");
	//print_matrix(&B[0][0], n_2, m_2);
	for (int i = 0; i < n_1; i++){
	    for (int j = 0; j < m_1; j++){
	        printf("%d ", A[i][j]);
	    }
	    printf("\n");
	}
	printf("\n");
	//-_-_-_-_-_-_-_-_-_-_-_-_-Matrix A-_-_-_-_-_-_-_-_-_-_-_-_-

    //-_-_-_-_-_-_-_-_-_-_-_-_-Matrix B-_-_-_-_-_-_-_-_-_-_-_-_-
    int n_2 = discover_n(argv[2]);
	int m_2 = discover_m(argv[2]);
	if(n_2 < 0 || m_2 < 0){
		return 0;
	}

    input = fopen(argv[2], "r");
	if (input == 0){
	    perror("Open");
	    return -1;
	}

	int **B = (int**)malloc(sizeof(int*) * n_2);
	for(int i = 0; i < n_2; i++){
	    B[i] = (int*)malloc(m_2 * sizeof(int));
	}

	fseek(input, 0, SEEK_SET);
    for (int i = 0; i < n_2; ++i){	
    	for (int j = 0; j < m_2; ++j){
	    	fscanf(input, "%d", &B[i][j]);
	    }
    }
    fclose(input);
    //-_-_-_-_-_-_-_-_-_-_-_-_-Matrix B-_-_-_-_-_-_-_-_-_-_-_-_-

    if(m_1 != n_2){
    	perror("Can't multiply these two matrix");
    	return -3;
    }

	int **C = (int**)malloc(m_1 * sizeof(int*));	//matrix C
	for(int i = 0; i < m_1 ; i++){
	    C[i] = (int*)malloc(n_2 * sizeof(int));
	}

	for (int i = 0; i < n_1; i++){
	    for (int j = 0; j < m_2; j++){
	        C[i][j] = 0;
	    }
	}

	//allocating memory for an array of thread IDs
	pthread_t* threads = (pthread_t*) malloc(n_1 * sizeof(pthread_t));
	//how many threads - so many structures with streaming data
	pthrData* threadData = (pthrData*) malloc(n_1 * sizeof(pthrData));

	printf("\nmatrix B:\n");
	//print_matrix(&B[0][0], n_2, m_2);
	for (int i = 0; i < n_2; i++){
	    for (int j = 0; j < m_2; j++){
	        printf("%d ", B[i][j]);
	    }
	    printf("\n");
	}
	printf("\n");

	//initialize the thread structures
	for(int i = 0; i < n_1; i++){	
		threadData[i].colls = m_1;
		threadData[i].rows = n_2;
		threadData[i].rowN = i;
		threadData[i].array1 = A;
		threadData[i].array2 = B;
		threadData[i].resArr = C;
		pthread_create(&(threads[i]), NULL, threadFunc, &threadData[i]);	//thread start
	}
 
	//waiting for all threads to execute
	for(int i = 0; i < n_1; i++){
		pthread_join(threads[i], NULL);
	}


	printf("\nThe result:\n");

	input = fopen(argv[3], "w+");
	if (input == 0){
	    perror("Open");
	    return -1;
	}

	for (int i = 0; i < n_1; i++){
	    for (int j = 0; j < m_2; j++){
	        printf("%d ", C[i][j]);
	        if(j == m_2-1){
	        	fprintf(input, "%d\n", C[i][j]);
	        }
	        else if(j == 0){
	        	fprintf(input, " %d ", C[i][j]);
	        }
	        else{
	        	fprintf(input, "%d ", C[i][j]);
	        }
	    }
	    printf("\n");
	}
	fclose(input);

	//free mem
	free(threads);
	free(threadData);
	for (int i = 0; i < n_1; i++){
	    free(A[i]);
	}
	for (int i = 0; i < n_2; i++){
	    free(B[i]);
	}
	for (int i = 0; i < m_1; i++){
	    free(C[i]);
	}
	free(A);
	free(B);
	free(C);

	return 0;
}