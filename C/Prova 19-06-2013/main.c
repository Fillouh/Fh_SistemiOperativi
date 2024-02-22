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
	int c1;
	int c2;
} str;


int main(int argc,char **argv){

	//controllo sul numero dei parametri
		//controllo sul numero dei parametri
	if(argc<=1){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int N=argc-1;
	int pidFiglio,ritorno,status;
	int i,k;
	int fd;
	int *pid=calloc(N,sizeof(int));//array per tenere i pid dei vari figli da poter recuperare
	char c;
	str gg;
	str rd;
	int num=0;

	//allocazione e creazione delle N pipe per pipeline
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(3);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli n figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA FORK %d\n",i+1);
			exit(4);
		}
		if(pid[i]==0){

			//sono nei figli
			//chiudo i lati delle pipe che non utilizzo, LEGGERO' DALLA PIPE i+1 E SCRIVERO' SULLA PIPE i-esima
			for(k=0;k<N;++k){
				if(k!=i){
					close(piped[k][1]);
				}
				if((i==N-1)||(k!=i+1)){
					close(piped[k][0]);
				}
			}

			//provo ad aprire il file in ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d",i+1);
				exit(-1);
			}

			if(i==N-1){
				while(read(fd,&c,1)!=0){
					num=(int)(c-48);
					gg.c1=i;
					gg.c2=num;
					write(piped[i][1],&gg,sizeof(str));
				}
			}
			else{
				//sono negli altri figli
				while(read(fd,&c,1)!=0){
					num=(int)(c-48);
					read(piped[i+1][0],&rd,sizeof(str));
					if(rd.c2>=num){
						gg.c1=rd.c1;
						gg.c2=rd.c2;
					}
					else{
						gg.c1=i;
						gg.c2=num;
					}
					write(piped[i][1],&gg,sizeof(str));
				}

			}
			exit(num);

		}
	}

	//SONO NEL PADRE 
	//chiudo i lati delle pipe corrette
	for(k=0;k<N;++k){
		if(k!=0){
			close(piped[k][0]);
		}
		close(piped[k][1]);
	}

	//leggo il valore restituito sulla pipe
	read(piped[0][0],&gg,sizeof(str));
	printf("IL PADRE HA RICEVUTO COME STRUTTURA I CAMPI : C1 : %d , C2 : %d\n",gg.c1,gg.c2);
	printf("IL FIGLIO RELATIVO A QUESTE INFORMAZIONI HA COME INDICE %d, PID %d ED E' RELATIVO AL FILE %s\n",gg.c1,pid[gg.c1],argv[gg.c1+1]);
	
	

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
