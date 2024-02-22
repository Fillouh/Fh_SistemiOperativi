#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef int pipe_t[2];



int main(int argc,char **argv){

	//controllo sul numero di parametri
	if(argc<=1){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int N=argc-1;
	int pid,pidFiglio,ritorno,status;
	int i,k;

	//allocazione e creazione delle N pipe per comunicaione tra figli e padre
	pipe_t *pipedStoP=malloc(N*sizeof(pipe_t));
	if(pipedStoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedStoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(3);
		}
	}

	//genero i figli
	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(4);
		}
		if(pid==0){
			//sono dentro al figlio
			//chiudo i lati della pipe che non uso
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				if(k!=i){
					close(pipedStoP[k][1]);
				}
			}

			
			//devo chiudere il lato di scrittura nel figlio per poter utilizzare la pipe
			close(1);
			dup(pipedStoP[i][1]);
			close(pipedStoP[i][1]);

			//eseguo la exec
			execl("/usr/bin/head","head","-1",argv[i+1],(char*)0);
			printf("ERRORE NELLA EXEC\n");
			exit(-1);
		}

	}

	//sono nel padre
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
	}

	//leggo i valori dai figli
	for(i=0;i<N;++i){
		char c;
		char *s=calloc(2,sizeof(char));
		int j=0;
		int sz=2;
		while(read(pipedStoP[i][0],&c,sizeof(char))){
			s[j]=c;
			++j;
			++sz;
			s=realloc(s,sz*sizeof(char));
		}
		printf("IL FIGLIO %d CORRISPONDENTE AL FILE %s HA RIPORTATO COME PRIMA LINEA : %s",i+1,argv[i+1],s);
	}

	//aspetto i figli
	for( i=0;i<N;++i){
		if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno);
		}
	}

	exit(0);
}
