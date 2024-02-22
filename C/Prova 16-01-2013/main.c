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
	char c;
	long int occ;
	char cx;
	long int occx;
} str;

int main(int argc,char **argv){

	//controllo dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}
	int N=argc-2;
	int fd;
	int pid,pidFiglio,ritorno,status;
	int i,k;

	//controllo sull'ultimo parametro
	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE,L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE ALFABETICO\n");
		exit(2);
	}
	char C=argv[argc-1][0];
	if(C<65||C>122||(C>90&&C<97)){
		printf("ERRORE,L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE ALFABETICO\n");
		exit(2);
	}

	//dichiarazione delle pipe per la comunicazione figlio padre
	pipe_t *pipedStoP=malloc(N*sizeof(pipe_t));
	if(pipedStoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedStoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(4);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);
	//genero i figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA GENERAZIONE DEL FIGLIO %d\n",i+1);
			exit(5);
		}
		if(pid==0){
			//sono nel figlio

			//chiudiamo le pipe che non utilizziamo
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				if(k!=i){
					close(pipedStoP[k][1]);
				}
			}
			

			//provo ad aprire il file corrispondente
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d",i+1);
				exit(-1);
			}

			//mi preparo alla lettura
			str gg;
			gg.occ=0;
			gg.occx=0;
			char c;
			bool ck;
			if(C>=65&&C<=90){
				ck=false;
			}
			else{
				ck=true;
			}

			while(read(fd,&c,1)!=0){
				if(c==C){
					++gg.occ;
					gg.c=c;
				}
				if(c==C&&ck==false){
					++gg.occx;
					gg.cx=C+32;
					
				}
				else if(c==C&&ck==true){
					++gg.occx;
					gg.cx=C-32;
				}
			}

			write(pipedStoP[i][1],&gg,sizeof(gg));

			if(gg.occ>=gg.occx){
				exit(1);
			}
			else{
				exit(0);
			}

		}
	}

	//sono nel padre
	//chiudo tutte le pipe che non uso
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
	}

	//leggo i valori
	str gg;

	for(i=0;i<N;++i){
		read(pipedStoP[i][0],&gg,sizeof(gg));
		printf("IL FILE %s HA COME OCCORRENZE DI %c %ld E COME OCCORRENZE DI %c %ld\n",argv[i+1],gg.c,gg.occ,gg.cx,gg.occx);
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
