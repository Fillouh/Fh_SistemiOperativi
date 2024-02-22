//SECONDA PROVA PARZIALE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef int pipe_t[2];
#define MSGSIZE 5


int main(int argc,char **argv){

	int N=argc-1;
	int fd;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,status1,ritorno2,status2;
	int i,k;
	char linea[MSGSIZE];
	char buffer[MSGSIZE];

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//allocazione per le n-1 pipe
	pipe_t *piped=malloc((N-1)*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	for(i=0;i<N-1;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(3);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(4);
		}
		if(pid==0){
			//sono dentro ai figli
			if(i==0){
				//sono dentro al primo figlio
				//chiudo i lati delle pipe che non utilizzo
				for(k=0;k<N-1;++k){
					close(piped[k][0]);
				}

				//apro il file corrispondente
				if((fd=open(argv[1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL PRIMO FIGLIO\n");
					exit(-1);
				}

				//leggo a blocchi di MSGSIZE
				while(read(fd,linea,MSGSIZE)){
					linea[MSGSIZE-1]='\0';
					for(i=0;i<N-1;++i){
						write(piped[i][1],linea,MSGSIZE*sizeof(char));
					}
				}
			}
			else{
				//sono dentro agli altri fratelli
				//chiudo i lati delle pipe che non utilizzo
				for(k=0;k<N;++k){
					close(piped[k][1]);
					if(k!=(i-1)){
						close(piped[k][0]);
					}
				}

				//apro il file assegnatomi
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
					exit(-1);
				}

				//leggo il valore inviato dal figlio P0
				while(read(piped[i-1][0],buffer,MSGSIZE*sizeof(char))!=0){
					while(read(fd,linea,MSGSIZE*sizeof(char))!=0){
						linea[MSGSIZE-1]='\0';
						if((pid1=fork())<0){
							printf("ERRORE NELLA CREAZIONE DEL NIPOTE\n");
							exit(-1);
						}
						if(pid1==0){
							//SONO DENTRO AL NIPOTE
							close(1);
							open("/dev/null",O_WRONLY);
							close(2);
							open("/dev/null",O_WRONLY);
							execl("/usr/bin/diff","diff",buffer,linea,(char*)0);
							printf("ERRORE NELLA EXEC\n");
							exit(-1);

						}
						//sono di nuovo dentro al figlio
						//aspetto ciò che mi ritorna il nipote
						if((pidNipote=wait(&status1))<0){
							printf("ERRORE NELLA WAIT DEL PADRE\n");
							exit(7);
						}
						if((status1 & 0XFF)!=0){
							printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

						}
						else{
							ritorno1=(int)((status1 >> 8) & 0xFF);
							printf("IL NIPOTE CON PID %d HA RITORNATO %d\n",pidNipote,ritorno1);
						}

						//controllo il valore di ritorno
						if(ritorno1==0){
							//significa che buffer e linea come file sono uguali
							printf("I FILE %s e %s SONO UGUALI\n",buffer,linea);
						}
						else{
							//significa che buffer e linea come file sono diversi
							printf("I FILE %s e %s SONO DIVERSI\n",buffer,linea);
						}
						
					}
					lseek(fd,0,SEEK_SET);
				}
			}
			exit(i);
		}
	}

	//SONO NEL PADRE
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<N;++k){
		close(piped[k][0]);
		close(piped[k][1]);
	}

	//aspetto tutti i figli
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
