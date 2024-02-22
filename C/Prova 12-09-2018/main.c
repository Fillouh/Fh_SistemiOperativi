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

	//controllo sul numero dei parametri
	if(argc<3){
		printf("ERRORE, NUMERO DEI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}

	int N=argc-1;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,ritorno2,status1,status2;
	int i,k;
	int fd;
	char c;

	//allocazione e creazione delle N pipe per pipeline
	//PIPE FIGLIO PADRE
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

	//PIPE NIPOTE PADRE
	pipe_t *pipedNtoP=malloc(N*sizeof(pipe_t));
	if(pipedNtoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedNtoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(5);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI e %d NIPOTI\n",getpid(),N,N);

	//generazione dei figli e dei nipoti

	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(6);
		}
		if(pid==0){
			//SONO NEL FIGLIO
			//genero i nipoti
			if((pid1=fork())<0){
				printf("ERRORE NELLA CREAZIONE DEL NIPOTE %d\n",i+1);
				exit(-1);
			}
			if(pid1==0){
				//SONO NEL NIPOTE
				//chiudo i lati delle pipe che non utilizzo
				int fs;
				for(k=0;k<N;++k){
					close(pipedStoP[k][0]);
					close(pipedStoP[k][1]);
					close(pipedNtoP[k][0]);
					if(k!=i){
						close(pipedNtoP[k][1]);
					}
				}

				//provo ad aprire il file per ogni nipote
				if((fs=open(argv[i+1],O_RDWR))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL NIPOTE %d\n",i+1);
					exit(-1);
				}

				//leggo i caratteri ed eseguo le trasformazioni
				int occ=0;
				long int trasformazioni=0;
				while(read(fs,&c,1)){
					if(c>=97&&c<=122){
						++occ;
						c-=32;
						lseek(fs,-1,SEEK_CUR);
						write(fs,&c,1);
						++trasformazioni;

					}
				}

				//comunico al padre tutte le trasformazioni effettuate
				write(pipedNtoP[i][1],&trasformazioni,sizeof(long int));



				exit(trasformazioni/256);
			}
			//sono tornato nel figlio
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
					close(pipedNtoP[k][0]);
					close(pipedNtoP[k][1]);
					close(pipedStoP[k][0]);
					if(k!=i){
						close(pipedStoP[k][1]);
					}
			}

			//provo ad aprire per ogni figlio il file
			if((fd=open(argv[i+1],O_RDWR))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			int occ=0;
			long int trasformazioni=0;
				while(read(fd,&c,1)){
					if(c>=48&&c<=57){
						++occ;
						c=' ';
						lseek(fd,-1,SEEK_CUR);
						write(fd,&c,1);
						++trasformazioni;

					}
			}
			//comunico al padre tutte le trasformazioni effettuate
			write(pipedStoP[i][1],&trasformazioni,sizeof(long int));

			//aspetto il nipote
			if((pidNipote=wait(&status1))<0){
				printf("ERRORE NELLA WAIT DEL PADRE\n");
				exit(7);
			}
			if((status1 & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

			}
			else{
				ritorno1=(int)((status1 >> 8) & 0xFF);
				printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidNipote,ritorno1);
			}
			exit(trasformazioni/256);
		}
	}

	//SONO NEL PADRE
	//chiudo tutte le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
		close(pipedNtoP[k][1]);
	}

	//leggo i valori nel padre
	long int trn=0;
	long int trf=0;
	for(i=0;i<N;++i){
		read(pipedNtoP[i][0],&trn,sizeof(long int));
		read(pipedStoP[i][0],&trf,sizeof(long int));
		printf("IL NIPOTE CON INDICE %d CORRISPONDENTE AL FILE %s HA EFFETTUATO %ld TRASFORMAZIONI\n",i,argv[i+1],trn);
		printf("IL FIGLIO CON INDICE %d CORRISPONDENTE AL FILE %s HA EFFETTUATO %ld TRASFORMAZIONI\n",i,argv[i+1],trf);
	}

	//aspetto i figli
	for( i=0;i<N;++i){

			if((pidFiglio=wait(&status2))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status2 & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno2=(int)((status2 >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno2);
		}
		
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
