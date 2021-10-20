#include <pthread.h>
#include <stdlib.h>
#include <mcheck.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
	int rowN; //number of the processed line
	int colls; //number of columns
	int rows; //number of lines

	int** array;	//pointers to matrices
	int** resArr;
}pthrData;

int discover_n(char* file)
{
	int fd, n = 0;
    char buffer;

    if((fd=open(file, O_RDONLY)) == -1)
    {
        perror("Open");
        return -1;
    }

    while (read(fd, &buffer, 1) != 0)
    {
        if(buffer == '\n')
        {
            n++;
        }
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
    if((fd=open(file, O_RDONLY)) == -1)
    {
        perror("Open");
        return -1;
    }

        
    while (read(fd, &buffer, 1) != 0)
    {
        while(read(fd, &buffer, 1) == 1)
        {
            if(buffer != ' ' && buffer != '\n')
            {   
                boolean = 0;
            }
            
            if(buffer >= '0' && buffer <= '9')
            {      
                if(boolean == 0)
                {
                    m++;    //digit counter
                }
                boolean = 1;
            }

            if(buffer == '\n')
            {
                break;
            }
        }

        if(n != 0 && M == m)    //check if number of digits in current line is the same as in the previous line
        {
            M = m;
            m = 0;
            boolean = 0;
        }
        else if(n == 0)
        {
            M = m;
            m = 0;
            boolean = 0;
        }
        else
        {
            perror("Matrix incorrect");
            return -2;
        }
        n++;
    }
    close(fd);
    return M;
}

void* threadFunc(void* thread_data)
{
	//getting a structure with data
	pthrData *data = (pthrData*) thread_data;

	for(int i = 0; i < data->rows; i++)
	{
	    for(int j = 0; j < data->colls; j++)
	    {
	        data->resArr[data->rowN][i] += data->array[data->rowN][j] * data->array[j][i];
	    }
	}
	
	return NULL;
}

int main(int argc,char* argv[])
{	
	//          	   m_1  	                 m_2	
	//	      	    ---------     	           -------
	//	      	    | 1 2 3 |		  	       | 1 2 |	
	// matrix1=	n_1 | 4 5 6 |  matrix2=    n_2 | 3 4 |	m_1 need to be equal to n_2, if it`s true multiplication is possible
	//	      	    						   | 5 6 |	

	int i, j, k, counter;

	int n = discover_n(argv[1]);
	int m = discover_m(argv[1]);
	if(n < 0 || m < 0)
	{
		return 0;
	}

	printf("n = %d, m = %d\n", n, m);

	FILE* input;
	input = fopen(argv[1], "r");
	if (input == 0)
	{
	    perror("Open");
	    return -1;
	}

	int **A = (int**)malloc(sizeof(int*) * n);
	for(int i = 0; i < n; i++)
	{
	    A[i] = (int*)malloc(m * sizeof(int));
	}

	fseek(input, 0, SEEK_SET);
    for (int i = 0; i < n; ++i)
    {		
    	for (int j = 0; j < m; ++j)
	    {
	    	fscanf(input, "%d", &A[i][j]);
	    }
    }
    fclose(input);

    printf("matrix :\n");
	//print_matrix(&B[0][0], n_2, m_2);
	for (i = 0; i < n; i++)
	{
	    for (j = 0; j < m; j++)
	    {
	        printf("%d ", A[i][j]);
	    }
	    printf("\n");
	}
	printf("\n");

    if(m != n)
    {
    	perror("Can't multiply these two matrix");
    	return -3;
    }

    //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

	int **B = (int**)malloc(m * sizeof(int*));	//matrix C
	for(i = 0; i < m ; i++)
	{
	    B[i] = (int*)malloc(n * sizeof(int));
	}

	for (i = 0; i < n; i++)
	{
	    for (j = 0; j < m; j++)
	    {
	        B[i][j] = 0;
	    }
	}

	//allocating memory for an array of thread IDs
	pthread_t* threads = (pthread_t*) malloc(n * sizeof(pthread_t));
	//how many threads - so many structures with streaming data
	pthrData* threadData = (pthrData*) malloc(n * sizeof(pthrData));

	//initialize the thread structures
	for(int i = 0; i < n; i++)
	{	
		threadData[i].colls = m;
		threadData[i].rows = n;
		threadData[i].rowN = i;
		threadData[i].array = A;
		threadData[i].resArr = B;
		pthread_create(&(threads[i]), NULL, threadFunc, &threadData[i]);	//thread start
	}
 
	//waiting for all threads to execute
	for(int i = 0; i < n; i++)
	{
		pthread_join(threads[i], NULL);
	}


	printf("\nThe result:\n");

	input = fopen(argv[2], "w+");
	if (input == 0)
	{
	    perror("Open");
	    return -1;
	}

	for (i = 0; i < n; i++)
	{
	    for (j = 0; j < m; j++)
	    {
	        printf("%d ", B[i][j]);
	        if(j == m-1)
	        {
	        	fprintf(input, "%d\n", B[i][j]);
	        }
	        else
	        {
	        	fprintf(input, "%d ", B[i][j]);
	        }
	    }
	    printf("\n");
	}

	fclose(input);

	//free mem
	free(threads);
	free(threadData);
	for (i = 0; i < n; i++)
	{
	    free(A[i]);
	}
	for (i = 0; i < n; i++)
	{
	    free(B[i]);
	}
	free(A);
	free(B);

	return 0;
}