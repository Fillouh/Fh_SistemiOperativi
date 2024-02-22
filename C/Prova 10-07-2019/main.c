//PROVA CON FIGLI A STAFFETTA
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

typedef int pipe_t[2];

int main(int argc,char **argv){

		//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}
	int N=argc-2;
	int fd,fcreato;
	int pid,pidFiglio,ritorno,status;
	int n,k;
	int cindex=0;//indice della coppia della pipe
	int i;

	//controllo sull'ultimo parametro
	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(2);
	}
	char Cx=argv[argc-1][0];

	//allocazione della memoria per le n pipe
	pipe_t *pipedSS=malloc(N*sizeof(pipe_t));
	if(pipedSS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(pipedSS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(3);
		}
	}

	//allocazione della memoria per le n pipe
	pipe_t *pipedTT=malloc(N*sizeof(pipe_t));
	if(pipedTT==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(pipedTT[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(3);
		}
	}


	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	for(i=0;i<2*N;++i){
		if((pid=fork())<0){

		}
		if(pid==0){
			//sono dentro al figlio
			for(k=0;k<N;++k){

			}
		}
	}




	exit(0);
}
