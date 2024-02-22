//PROVA CON STRUTTURE DATI E SCHEMA A PIPELINE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>

typedef int pipe_t[2];

typedef struct{
	int c1;//pid processo
	long int c2; //occorrenze del carattere C calcolate dal processo corrente
}str;

int main(int argc,char **argv){

	int N=argc-2;
	int fd;
	int pid,pidFiglio,ritorno,status;
	int i,k;
	long int occ;
	char ch,C;
	//str gg;//struttura su cui scrivo o leggo


	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//controllo sull'ultimo carattere
	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(2);
	}
	C=argv[argc-1][0];

	//allocazione per le n pipe per la comunicazione in pipeline (SI SCRIVERA' SULLA PIPE I-ESIMA E SI LEGGERA' DALLA PIPE I-1 ESIMA)
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(5);
		}
		if(pid==0){
			//sono nei figli

			//per ogni figlio va aperto il file
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
				exit(-1);
			}
			if(i==0){
				//sono nel primo figlio, lui dovrà solo scrivere e non leggere dalla pipe
				//chiudo i lati delle pipe che non mi servono
				for(k=0;k<N;++k){
					close(piped[k][0]);
					if(k!=0){
						close(piped[k][1]);
					}
				}
				//leggo le occorrenze che mi servono
				occ=0;
				while(read(fd,&ch,1)!=0){
					if(ch==C){
						++occ;
					}
				}
				str *cur=malloc((i+1)*sizeof(str));
				cur[i].c1=getpid();
				cur[i].c2=occ;
				//scrivo sulla pipe i-esima per mandare l'informazione al figlio successivo
				write(piped[i][1],cur,(i+1)*sizeof(str));
			}
			else{
				//sono in tutti gli altri figli
				//chiudo i lati delle pipe che non mi servono
				for(k=0;k<N;++k){
					if(k!=i-1){
						close(piped[k][0]);
					}
					if(k!=i){
						close(piped[k][1]);
					}
				}

				//leggo le occorrenze che mi servono
				occ=0;
				while(read(fd,&ch,1)!=0){
					if(ch==C){
						++occ;
					}
				}
				str *cur=malloc((i+1)*sizeof(str));
				read(piped[i-1][0],cur,i*sizeof(str));
				cur[i].c1=getpid();
				cur[i].c2=occ;
				write(piped[i][1],cur,(i+1)*sizeof(str));

			}
			exit(i);

		}
	}
	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<N;++k){
		if(k!=N-1){
			close(piped[k][0]);
		}
		close(piped[k][1]);
	}

	//leggo i valori dalla pipeline che mi spetta
	str *cur=malloc(N*sizeof(str));
	read(piped[N-1][0],cur,N*sizeof(str));
	for(i=0;i<N;++i){
		printf("IL FIGLIO DI INDICE %d, CON PID %d, RELATIVO AL FILE %s HA TROVATO COME OCCORRENZE DEL CARATTERE %c : %ld\n",i,cur[i].c1,argv[i+1],C,cur[i].c2);
	}

	//aspetto tutti i figli
	for( i=0;i<N;++i){
		if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO CON PID %d SI E' CHIUSO IN MODO ANOMALO\n",pidFiglio);

		}
		else{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno);
		}
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
