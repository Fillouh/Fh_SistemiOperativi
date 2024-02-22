//SECONDA PROVA PARZIALE
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
	int fd;
	int pidFiglio,ritorno,status;
	int i,k;
	char c;
	int K,X;
	int rs;
	int *pid=malloc(N*sizeof(int));
	char linea[256];
	char s[256];


	//controllo sull'ultimo parametro
	K=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||K<=0){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO\n");
		exit(2);
	}

	//allocazione della memoria per le n pipe
	//PIPE DI COMUNICAZIONE FIGLIO-PADRE
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(4);
		}
	}

	//chiediamo all'utente di fornirci un numero X che sia compreso tra 1 e K
	printf("MI DIRESTI UN NUMERO STRETTAMENTE POSITIVO MA MINORE DI K ? \n");
	printf("DIGITA : ");
	rs=scanf("%s",s);
	X=atoi(s);
	if(X<=0||X>K||rs==0){
		printf("ERRORE, IL NUMERO CHE HAI INSERITO NON E' CORRETTO\n");
		exit(5);
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d",i+1);
			exit(6);
		}
		if(pid[i]==0){
			//sono dentro al figlio
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d",i+1);
				exit(-1);
			}

			//provo a leggere dal file
			int indl=1; //indice di linea corrente
			int j=0; //indice per linea
			int L=0; //lunghezza della linea ricercata
			bool ck=false; //mi verifica che la linea esiste
			while(read(fd,&c,1)!=0){
				if(c=='\n'){
					++indl;
					j=0;
				}
				if(indl==X){
					ck=true;
					linea[j]=c;
					++j;
					++L;
				}
			}

			//scelgo il percorso disponibile in base alla struttura del file
			if(ck){
				//la linea esiste e manderò al padre la lunghezza e la linea ricercata
				--L;
				linea[L+1]='\0';
				write(piped[i][1],&L,sizeof(int));
				write(piped[i][1],linea,sizeof(linea));
				

			}
			else{
				//la linea non esiste perciò manderò la linea vuota e come lunghezza 0
				L=0;
				linea[L]='\0';
				write(piped[i][1],&L,sizeof(int));
				
			}

			exit(L);
		}
	}
	//SONO NEL PADRE
	//chiudo tutte le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	//leggo i valori
	int L;
	for(i=0;i<N;++i){
		read(piped[i][0],&L,sizeof(int));
		if(L!=0){
			read(piped[i][0],linea,sizeof(linea));
			printf("PER IL FIGLIO DI INDICE %d, PID %d, RELATIVO AL FILE %s LA LINEA DI INDICE %d E' LUNGA %d ED E' : %s\n",i,pid[i],argv[i+1],X,L,linea);
		}
		else{
			printf("PER IL FIGLIO DI INDICE %d, PID %d, RELATIVO AL FILE %s LA LINEA DI INDICE %d NON ESISTE\n",i,pid[i],argv[i+1],X);
		}
	}

	//aspetto tutti i figli
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
