//PROVA CON ANELLO RING TRA SOLI FIGLI
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
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}

	int Q=argc-1;
	int fd;
	int pidFiglio,ritorno,status;
	int q,k;
	char c;
	int *pid=malloc(Q*sizeof(int));
	if(pid==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER I PID\n");
		exit(2);
	}
	char linea[256];//suppongo che le linee del file non siano maggiori di 256 caratteri

	//allocazione della memoria per le n pipe
	//ogni figlio leggere dalla pipe q e scriverà sulla pipe (q+1)%Q
	pipe_t *piped=malloc(Q*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	//creazione delle n pipe
	for(q=0;q<Q;++q){
		if(pipe(piped[q])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",q);
			exit(4);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),Q);

	//generazione dei Q figli
	for(q=0;q<Q;++q){
		if((pid[q]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",q+1);
			exit(5);
		}
		if(pid[q]==0){
			//sono dentro ai figli
			//chiudo le pipe che non utilizzo
			for(k=0;k<Q;++k){
				if(k!=q){
					close(piped[k][0]);
				}
				if(k!=((q+1)%Q)){
					close(piped[k][1]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[q+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",q+1);
				exit(-1);
			}

			//comincio la lettura della prima linea
			int occ=0;
			int j=0;
			int rit=0;
			while(read(fd,&c,1)!=0){
				if(c=='\n'){
					linea[j]='\0';
					//aspetto l'ok dal figlio precedente o dal padre
					bool ck=false;
					while(ck==false){
						read(piped[q][0],&ck,sizeof(bool));
					}
					//HO RICEVUTO L'OK DA PARTE DEL PADRE O DEL FIGLIO PRECEDENTE --> POSSO STAMPARE
					printf("SONO IL FIGLIO DI INDICE %d, CON PID %d HO TROVATO %d CARATTERI NUMERICI NEL FILE %s ALLA LINEA : %s\n",q,pid[q],occ,argv[q+1],linea);

					//mando l'ok al figlio successivo
					ck=true;
					write(piped[(q+1)%Q][1],&ck,sizeof(bool));
					rit=occ;
					j=0;
					occ=0;
				}
				if(isdigit(c)){
					++occ;
				}
				linea[j]=c;
				++j;
			}
			exit(rit);

		}
	}

	//SONO NEL PADRE
	//chiudo le pipe che non utilizzo
	for(q=1;q<Q;++q){
		close(piped[q][0]);
		close(piped[q][1]);
	}

	//mando l'ok al primo figlio
	bool ck=true;
	write(piped[0][1],&ck,sizeof(bool));

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
