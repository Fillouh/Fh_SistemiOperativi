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


int main(int argc,char **argv){

	int Q=argc-1;
	int fd,fcreato;
	int pid,pidFiglio,ritorno,status;
	int q,k,j;
	char linea[250];
	int lunghezza;
	int indl;
	int sendline;
	char c;

	//controllo sul numero dei parametri
	if(argc<3){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//allocazione della memoria per le n pipe
	//pipe figlio-padre
	pipe_t *piped=malloc(Q*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	//creazione delle n pipe
	for(q=0;q<Q;++q){
		if(pipe(piped[q])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",q);
			exit(3);
		}
	}
	//creo il file camilla
	if((fcreato=creat("Camilla",0644))<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE CAMILLA\n");
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),Q);

	//genero i q figli
	for(q=0;q<Q;++q){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",q+1);
			exit(4);
		}
		if(pid==0){
			//sono dentro al figlio
			//chiudo i lati delle pipe che non utilizzo
			for(k=0;k<Q;++k){
				close(piped[k][0]);
				if(k!=q){
					close(piped[k][1]);
				}
			}

			//apro il file relativo all'indice corrente per ogni figlio
			if((fd=open(argv[q+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO DI INDICE %d\n",q);
				exit(-1);
			}

			//fase di lettura dal file
			j=0;
			lunghezza=0;
			indl=1;
			sendline=0;
			while(read(fd,&linea[j],1)!=0){
				if(linea[j]=='\n'){
					++lunghezza;
					//comunicazione al padre solo se serve
					if(isdigit(linea[0])&&(lunghezza<10)){
						//scrivo al padre
						write(piped[q][1],linea,lunghezza*sizeof(char));
						++sendline;
					}
					else{
						linea[0]='\0';
						printf("NON POSSO COMUNICARE LA LINEA DI INDICE %d NEL FILE %s\n",indl,argv[q+1]);
					}
					++j;
					linea[j]='\0';
					j=0;
					lunghezza=0;
					++indl;
				}
				else{
					++j;
					++lunghezza;
				}
			}

			exit(sendline);
		}
	}
	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<Q;++k){
		close(piped[k][1]);
	}

	//leggo le linee per ogni figlio

	for(q=0;q<Q;++q){
		j=0;
		indl=1;
		while(read(piped[q][0],&c,sizeof(char))){
			linea[j]=c;
			++j;
			if(c=='\n'){

				linea[j]='\0';
				printf("IL FIGLIO DI INDICE %d RELATIVO AL FILE %s HA RESTITUITO LA LINEA DI INDICE %d : %s",q,argv[q+1],indl,linea);
				j=0;
				++indl;
			}
		}
	}

	//aspetto tutti i figli
	for(q=0;q<Q;++q){
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
