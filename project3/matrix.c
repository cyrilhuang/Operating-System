#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int a[100][100];
int b[100][100];
int c[100][100];
int m,n,k;

struct v{
	int i;
	int j;
};

pthread_t tid[100][100];
pthread_attr_t attr[100][100];

void *runner(void *param)
{
	struct v *data = param;
	int n, sum = 0;
	
	for(n = 0; n < k; ++n){
		sum += a[data->i][n] * b[n][data->j];
	}
	
	c[data->i][data->j] = sum;
	
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	FILE *fileHandler;
	fileHandler = fopen("matrixData.txt","r");
	fscanf(fileHandler,"%d%d%d",&m,&k,&n);
	printf("\nthe first matrix has %d rows, %d cols", m, k);
	printf("\nthe second matrix has %d rows, %d cols", k, n);
	int i, j;
	printf("\nthe first matrix is:\n");
	for (i = 0; i < m; i++){
		for (j = 0; j < k; j++){
			fscanf(fileHandler,"%d",&a[i][j]);
			printf("%d\t",a[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	printf("\nthe second matrix is:\n");
	for (i = 0; i < k; i++){
		for (j = 0; j < n; j++){
			fscanf(fileHandler,"%d",&b[i][j]);
			printf("%d\t",b[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	fclose(fileHandler);
	
	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++){
			struct v *data = (struct v*) malloc(sizeof(struct v));
			data -> i = i;
			data -> j = j;
					
			pthread_attr_init(&attr[i][j]);
			pthread_create(&tid[i][j], &attr[i][j], runner, data);
			
			//pthread_join(tid[i][j], NULL);		
			
		}
	}

    for (i = 0; i < m; i++ ){
        for (j = 0; j < n; j++)
            pthread_join(tid[i][j], NULL);
    }
	
	
	printf("\nthe result matrix is:\n");
	for (i = 0; i < m; i++){
		for (j = 0; j < n; j++){
			printf("%d\t",c[i][j]);
		}
		printf("\n");
	}
	
	return 0;
	
}
