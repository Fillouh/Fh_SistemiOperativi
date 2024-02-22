#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

typedef int pipe_t[2];

int main(int argc,char **argv){

	//comtrollo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int N=argc-2;
	int pidFiglio,ritorno,status;
	int i,k;
	int fd;
	int af;
	int *pid=calloc(N,sizeof(int));//array per tenere i pid dei vari figli da poter recuperare
	char c;
	//char rc;//carattere che riceve il padre
	int rck;//numero che riceve il figlio se è 0 non procede se è 1 procede
	bool *unsuc=calloc(N,sizeof(bool));
	//int rd;//valore di ritorno della read

	//allocazione e creazione delle N pipe per pipeline
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

	pipe_t *pipedPtoS=malloc(N*sizeof(pipe_t));
	if(pipedPtoS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedPtoS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(5);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(6);
		}
		if(pid[i]==0){
			//sono dentro al figlio
			//chiudo tutte le pipe che no utilizzo
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				close(pipedPtoS[k][1]);
				if(k!=i){
					close(pipedStoP[k][1]);
					close(pipedPtoS[k][0]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			//il figlio si mette in attesa di un ok da parte del padre
			while(read(pipedPtoS[i][0],&rck,sizeof(int))){
				if(rck==0){
					break;
				}
				read(fd,&c,1);
				write(pipedStoP[i][1],&c,sizeof(char));
			}

			exit(0);
		}
	}

	//SONO NEL PADRE
	//chiudo tutte le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
		close(pipedPtoS[k][0]);
	}

	//il padre apre il file Af
	if((af=open(argv[argc-1],O_RDONLY))<0){
		printf("IL PADRE NON E' RIUSCITO AD APRIRE IL FILE AF\n");
		exit(7);
	}

	//il padre manda il primo ok a tutti i figli per leggere il primo carattere
	rck=1;
	char v;
	while(read(af,&v,1)){
		for(i=0;i<N;++i){
			if(!unsuc[i]){
				rck=1;
				write(pipedPtoS[i][1],&rck,sizeof(int));
				read(pipedStoP[i][0],&c,sizeof(char));
				if(c!=v){
					unsuc[i]=true;
				}
			}
		}
	}

	for(i=0;i<N;++i){
		if(unsuc[i]){
			kill(pid[i],SIGKILL);
		}
		else{
			rck=0;
			write(pipedPtoS[i][1],&rck,sizeof(int));
		}
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

	printf("HO FINITO TUTTO\n");
	exit(0);
}
