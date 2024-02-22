//PROVA CON PIPELINE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef int pipe_t[2];

typedef struct{
	long int c1;
	int c2;
	long int c3;
} str;

int main(int argc,char **argv){

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int N=argc-2;
	int pidFiglio,ritorno,status;
	int i,k;
	int fd;
	int *pid=calloc(N,sizeof(int));//array per tenere i pid dei vari figli da poter recuperare
	char c;
	str gg;
	str rd;
	long int occ=0;

	//controllo sull'ultimo parametro
	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE,L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO PARAMETRO\n");
		exit(2);
	}
	char Cx=argv[argc-1][0];

	//allocazione e creazione delle N pipe per pipeline
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

	//genero i figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA FORK %d\n",i+1);
			exit(5);
		}
		if(pid[i]==0){
			//sono nei figli
			//chiudo le pipe che non mi servono
			for(k=0;k<N;++k){
				if(k!=i){
					close(piped[k][1]);
				}
				if((i==0)||(k!=i-1)){
					close(piped[k][0]);
				}
			}

			//apertura del file che vale per ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d",i+1);
				exit(-1);
			}

		
			if(i==0){
				//char c;
				//str gg;
				gg.c1=0;
				gg.c2=i;
				gg.c3=0;
				while(read(fd,&c,1)!=0){
					if(c==Cx){
						++gg.c1;
					}
				}
				gg.c3+=gg.c1;
				write(piped[i][1],&gg,sizeof(str));

			}
			else{
				//per tutti gli altri figli
				//char c;
				//long int occ=0;
				//str rd;
				while(read(fd,&c,1)!=0){
					if(c==Cx){
						++occ;
					}
				}

				//eseguo la read dalla pipe i-1
				//str gg;
				read(piped[i-1][0],&rd,sizeof(str));
				if(rd.c1>occ){
					gg.c1=rd.c1;
					gg.c2=rd.c2;
				}
				else{
					gg.c1=occ;
					gg.c2=i;
				}
				gg.c3=(occ+rd.c3);

				write(piped[i][1],&gg,sizeof(str));
			}

			exit(i);
		}
	}

	//SONO NEL PADRE
	for(k=0;k<N;++k){
		if(k!=N-1){
			close(piped[k][0]);
		}
		close(piped[k][1]);
	}

	//leggo il singolo valore dalla pipe n-1 
	//str gg;
	read(piped[N-1][0],&gg,sizeof(str));
	printf("IL PADRE HA RICEVUTO COME CAMPI DELLA STRUTTURA :  C1 : %ld, C2 : %d, C3 : %ld\n",gg.c1,gg.c2,gg.c3);
	printf("QUINDI IL FIGLIO CON MAGGIORI OCCORRENZE DEL CARATTERE %c E' QUELLI CON L'INDICE %d, PID %d, RELATIVO AL FILE %s\n",Cx,gg.c2,pid[gg.c2],argv[gg.c2+1]);

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
