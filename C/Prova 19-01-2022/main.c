//PROVA CON FIGLI A COPPIE
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
	if(argc<3){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}
	int N=argc-2;
	int fd,fcreato;
	int pidFiglio,ritorno,status;
	int i,k;
	int C;
	int nroTotale,nro;
	int nw;//numeri di controllo di ciò che ritorna la read e la write
	int *pid=malloc(N*sizeof(int));
	if(pid==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER L'ARRAY DI PID\n");
		exit(2);
	}
	

	//controllo sull'ultimo parametro
	C=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||C<=0||(C%2)==0){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO E DISPARI\n");
		exit(3);
	}
	char *buffer=malloc(C*sizeof(char));

	//allocazione della memoria per le n pipe
	//PIPE DI COMUNICAZIONE FIGLIO1-FIGLIO2
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(6);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),2*N);

	for(i=0;i<2*N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(8);
		}
		if(pid[i]==0){
			//sono dentro ai figli
			if(i<N){
				//PRIMO FIGLIO DELLA COPPIA
				//chiudo le pipe che non utilizzo
				for(k=0;k<N;++k){
					close(piped[k][0]);
					if(k!=i){
						close(piped[k][1]);
					}
				}

				//provo ad aprire il file in ogni primo figlio della coppia
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
					exit(-1);
				}

				//MI CALCOLO LA LUNGHEZZA IN BYTE DELL'INTERO FILE CON LA lseek
				nroTotale=lseek(fd,0,SEEK_END)/C;
				nro=nroTotale/2;
				lseek(fd,0,SEEK_SET);
				//LEGGO LA META' CHE MI SPETTA
				for(int j=0;j<nro;++j){
					read(fd,buffer,C);
					nw=write(piped[i][1],buffer,sizeof(buffer));
					if(nw!=sizeof(buffer)){
						printf("ERRORE NELLA WRITE DEL PRIMO PROCESSO FIGLIO\n");
						exit(0);
					}

				}

			}
			else{
				//SECONDO FIGLIO DELLA COPPIA
				//chiudo le pipe che non utilizzo
				for(k=0;k<N;++k){
					close(piped[k][1]);
					if(k!=i){
						close(piped[k][0]);
						
					}
				}

				//provo ad aprire il file in ogni primo figlio della coppia
				if((fd=open(argv[(i%N)+1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
					exit(-1);
				}

				//CREO IL FILE SU CUI POTER SCRIVERE ESSENDO NEL SECONDO FIGLIO
				char *nome=malloc((strlen(argv[(i%N)+1])+strlen(".mescolato")+1)*sizeof(char));
				sprintf(nome,"%s.mescolato",argv[(i%N)+1]);
				if((fcreato=creat(nome,0644))<0){
					printf("ERRORE NELLA CREAZIONE DEL FILE NEL SECONO FIGLIO DI INDICE %d\n",i+1);
				}

				//MI CALCOLO LA LUNGHEZZA IN BYTE DELL'INTERO FILE CON LA lseek
				nroTotale=lseek(fd,0,SEEK_END)/C;
				nro=nroTotale/2;
				lseek(fd,(nro*C)+1,SEEK_SET);
				//LEGGO LA META' CHE MI SPETTA E LA SCRIVO SUL FILE CHE HO CREATO
				for(int j=0;j<nro;++j){
					read(fd,buffer,C);
					write(fcreato,buffer,sizeof(buffer));
					read(piped[i][0],buffer,C);
					write(fcreato,buffer,sizeof(buffer));

				}
				
			}
			exit(nro);
		}
	}

	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	//chiude tutte le pipe che non usa
	for(k=0;k<N;++k){
		close(piped[k][0]);
		close(piped[k][1]);
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
