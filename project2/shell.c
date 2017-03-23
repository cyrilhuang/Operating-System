#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_LINE 80
//#define buffer "\n\nCommand History:\n"

char history[10][MAX_LINE];
char prefer[MAX_LINE] = "cal";
int count = 0;
int caught = 0;
char buffer[50];

void displayHistory()
{
	printf("\n\nCommand History:\n");
	int i;
	int j = 0;
	int hiscount = count;
	for (i = 0; i < 10; ++i){
		printf("%d. ", hiscount);
		while(history[i][j] != '\n' && history[i][j] != '\0'){
			printf("%c",history[i][j]);
			++j;
		}
		j = 0;
		printf("\n");
		hiscount--;
		if (hiscount == 0) break;
	}
	printf("\n");
}

void handle_SIGINT()
{
	write(STDOUT_FILENO,buffer,sizeof(buffer));
    displayHistory();
    caught = 1;
}

int setup(char inputBuffer[], char *args[],int *background)
{
	int length, i, start, ct;
	
	ct = 0;
	
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
	
	if (caught == 1){
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
		caught = 0;
	}
	
	start = -1;
	if (length == 0)
		exit(0);
	if (length < 0){
		perror("error reading the command\n");
		exit(-1);
	}
	
	for (i = 0; i < length; i++){
		switch(inputBuffer[i]){
			case ' ':
			case '\t':
			if (start != -1){
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			start = -1;
			break;
			
			case '\n':
			if (start != -1){
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL;
			break;
			
			default:
			if (start == -1)
				start = i;
			if (inputBuffer[i] == '&'){
				*background = 1;
				inputBuffer[i] = '\0';
			}
		}
	}
	
	args[ct] = NULL;
	if (strcmp(args[0],"history") == 0){
		if (count > 0) displayHistory();
		else printf("\nno history command\n");
		return -1;
	}
	else if (args[0][0] - 'r' == 0){
        if (args [0][1] == '\0'){
            strcpy(inputBuffer,history[0]);
        }
        else{
            int k;
            for (k = 0; k < 10 && k < count; ++k){
                if (args[0][1] == history[k][0]){
                    strcpy(inputBuffer,history[k]);
					break;
                }
            }
            if (k == 10 || k == count){
                printf("\nNo such command in history\n");
                strcpy(inputBuffer,"Wrong command");
            }
        }
    }
	else if (strcmp(args[0],"prefer") == 0){
		strcpy(inputBuffer,prefer);
	}
	else if (args[0][0] == 'c' && args[0][1] == 'h' && args[0][2] == 'a' && args[0][3] == 'n' && args[0][4] == 'g' && args[0][5] == 'e'){
		int j = 6;
		while(args[0][j] != '\0' && args[0][j] != '\n'){
			prefer[j-6] = args[0][j];
			++j;
		}
		inputBuffer[j-6] = '\0';
		return -1;
	}
	
	for (i = 9; i > 0; i--){
		strcpy(history[i], history[i-1]);
	}
	
	strcpy(history[0],inputBuffer);
	++count;
	if (count > 10) count = 10;
	return 0;
}

int main(void)
{
	char inputBuffer[MAX_LINE];
	int background;
	char *args[MAX_LINE/2 + 1];
	
	pid_t pid;
	int i;
	
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	sigaction(SIGINT, &handler, NULL);
	
	strcpy(buffer, "Caught Control C\n");
	
	while (1){
		background = 0;
		printf("Command->\n");
		if (setup(inputBuffer,args,&background) != -1){
		
		pid = fork();
		if (pid < 0){
			printf("fork failed\n");
			exit(1);
		}
		else if (pid == 0){
			if (execvp(args[0], args) == -1)
				printf("error executing command\n");
		}
		else{
			if (background == 0){
				//wait(NULL);
				waitpid(pid,NULL,0);
			}
		}
		}
	}
}