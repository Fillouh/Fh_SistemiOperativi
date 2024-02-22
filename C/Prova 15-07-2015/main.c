//PROVA CON UTILIZZO OBBLIGATORIO DEI SEGNALI
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

int main(int argc, char **argv){
	int N=argc-1;
	int fd,fcreato;
	int pidFiglio,ritorno,status;
	int i,k;
	bool ck,ck1;
	char nome[]="Merge";
	int nr;
	char c;
	int *pid=malloc(N*sizeof(int));
	if(pid==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER L'ARRAY DI PID\n");
		exit(1);
	}
	//un processo è ritenuto finito se il suo valore all'indice corrispondente nell'arrya finito è uguale ad 1
	int *finito=calloc(N,sizeof(int));
	if(finito==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER L'ARRAY DEI PROCESSI FINITI\n");
		exit(2);
	}

	//controllo sul numero dei parametri
	if(argc<3){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(3);
	}

	//creo il file fcreato Merge
	if((fcreato=creat(nome,0644))<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE MERGE \n");
		exit(4);
	}

	//allocazione per le n pipe padre figlio
	pipe_t *pipedFtoP=malloc(N*sizeof(pipe_t));
	if(pipedFtoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(5);
	}
	pipe_t *pipedPtoF=malloc(N*sizeof(pipe_t));
	if(pipedPtoF==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(6);
	}


	for(i=0;i<N;++i){
		if(pipe(pipedFtoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(7);
		}
		if(pipe(pipedPtoF[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(8);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(7);
		}
		if(pid[i]==0){
			//sono dentro al figlio
			//chiudo i lati della pipe che non utilizzo
			for(k=0;k<N-1;++k){
				close(pipedFtoP[k][0]);
				close(pipedPtoF[k][1]);
				if(k!=i){
					close(pipedFtoP[k][1]);
					close(pipedPtoF[k][0]);
				}
			}

			//provo ad aprire il file corrente
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
				exit(-1);
			}

			ck1=false;
			ck=false;
			while(1){
				read(pipedPtoF[i][0],&ck,sizeof(bool));
				if(ck==true){
					nr=read(fd,&c,1);
					if(nr!=sizeof(char)){
						ck1=true;
						write(pipedFtoP[i][1],&ck1,sizeof(bool));
						break;
					}
					else{
						ck1=false;
						write(pipedFtoP[i][1],&ck1,sizeof(bool));
						write(pipedFtoP[i][1],&c,sizeof(char));
					}
				}
				else{
					break;
				}
			}

			exit(c);

		}
	}
	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<N;++k){
		close(pipedFtoP[k][1]);
		close(pipedPtoF[k][0]);
	}

	//leggo i valori per ogni figlio
	ck=true;
	i=0;
	while(1){
		if((i%N)==0){
			i=0;
		}
		write(pipedPtoF[i][1],&ck,sizeof(bool));
		read(pipedFtoP[i][0],&ck1,sizeof(bool));
		if(ck1==true){
			//devo interrompere tutti i figli
			finito[i]=1;
			for(int j=0;j<N;++j){
				if(j!=i){
					kill(pid[j],SIGKILL);
				}
			}
			ck=false;
			break;
		}
		else{
			//posso scrivere il carattere che ricevo sul file fcreato
			read(pipedFtoP[i][0],&c,sizeof(char));
			write(fcreato,&c,1);
			++i;
		}
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
