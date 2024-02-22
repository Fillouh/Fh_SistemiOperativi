//PROVA CON ANELLO RING TRA PADRE E FIGLI
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
	if(argc<5){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}
	int Q=argc-3;
	int fd;
	int pidFiglio,ritorno,status;
	int q,k;
	char c;
	int *pid=malloc(Q*sizeof(int));
	if(pid==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER I PID\n");
		exit(2);
	}

	//controllo sul primo parametro
	if(argv[1]==NULL||argv[1][0]!='/'){
		printf("ERRORE, IL PRIMO PARAMETRO DEVE ESSERE UN NOME ASSOLUTO DI UN FILE VALIDO\n");
		exit(3);
	}

	//controllo sul secondo parametro
	int L=atoi(argv[2]);
	if(argv[2]==NULL || L<=0){
		printf("ERRORE, IL SECONDO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMETE POSITIVO\n");
		exit(4);
	}

	//controllo sui restanti parametri
	for(int j=3;j<argc-1;++j){
		if(argv[j]==NULL||strlen(argv[j])!=1){
			printf("ERRORE, IL PARAMETRO %d DEVE ESSERE UN SINGOLO CARATTERE\n",j);
			exit(5);
		}
	}



	//allocazione della memoria per le n pipe
	//ogni figlio leggere dalla pipe q e scriverà sulla pipe q+1
	pipe_t *piped=malloc((Q+1)*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(6);
	}

	//creazione delle n pipe
	for(q=0;q<Q+1;++q){
		if(pipe(piped[q])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",q);
			exit(7);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),Q);

	//generazione dei q figli
	for(q=0;q<Q;++q){
		if((pid[q]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d",q);
			exit(8);
		}
		if(pid[q]==0){
			//sono dentro ai figli
			//chiudo le pipe che non utilizzo
			for(k=0;k<Q+1;++k){
				if(k!=q){
					close(piped[k][0]);
				}
				if(k!=q+1){
					close(piped[k][1]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d",q);
				exit(-1);
			}


			char Cx=argv[q+3][0];//carattere da controllare
			int occ=0;
			int rit=0;
			while(read(fd,&c,1)!=0){
				if(c=='\n'){
					//leggo il segnale di ok da parte del padre
					bool ck=false;
					while(ck==false){
						read(piped[q][0],&ck,sizeof(bool));
					}
					//HO L'OK PER SCRIVERE
					printf("FIGLIO CON INDICE %d, CON PID %d, HA TROVATO %d OCCORRENZE DEL CARATTERE %c NELLA LINEA CORRENTE\n",q,getpid(),occ,Cx);

					//mando il segnale di ok al figlio successivo
					ck=true;
					write(piped[q+1][1],&ck,sizeof(bool));
					rit=occ;
					occ=0;
				}
				if(c==Cx){
					++occ;
				}
			}

			exit(rit);
		}

	}

	//sono nel padre
	//chiudo le pipe che non mi interessano
	for(k=0;k<Q+1;++k){
		if(k!=0){
			close(piped[k][1]);
		}
		if(k!=Q){
			close(piped[k][0]);
		}
	}

	//faccio il ciclo per ogni linea
	bool ck;
	for(q=0;q<L;++q){
		printf("LINEA %d DEL FILE %s\n",q+1,argv[1]);
		//scrivo al primo figlio
		ck=true;
		write(piped[0][1],&ck,sizeof(bool));
		//rimango in attesa fintantochè tutti i figli non hanno svolto il loro compito
		ck=false;
		while(ck==false){
			read(piped[Q][0],&ck,sizeof(bool));
		}
	}

	//aspetto tutti i figli
	for( q=0;q<Q;++q){
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
