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

	char C;//carattere passato come argomento
	int K;//numero di linee passate come argomento
	int N=argc-3; //numero di file passati
	int pid,pidFiglio,ritorno,status;
	int i,k;
	int fd;


	//controllo sul numero dei parametri
	if(argc<5){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//controllo sul carattere
	if(argv[argc-2]==NULL||strlen(argv[argc-2])!=1){
		printf("IL PARAMETRO PASSATO NON E' UN CARATTERE!\n");
		exit(2);
	}
	C=argv[argc-2][0];

	//controllo sul numero delle linee
	if(argv[argc-1]==NULL){
		printf("IL PARAMETRO PASSATO NON E' UN NUMERO STRETTAMENTE POSITIVO!\n");
		exit(3);
	}
	K=atoi(argv[argc-1]);
	if(K<=0){
		printf("IL PARAMETRO PASSATO NON E' UN NUMERO STRETTAMENTE POSITIVO!\n");
		exit(3);
	}

	//allocazione della memoria per le n pipe
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(5);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli n figli e procedimento per i figli

	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA FORK %d-ESIMA\n",i);
			exit(6);
		}
		if(pid==0){
			//ENTRO DENTRO AL FIGLIO
			printf("DEBUG-SONO IL PROCESSO FIGLIO CON PID %d E STO PER ESEGUIRE I MIEI COMPITI\n",getpid());

			//chiudo il lato delle pipe che non uso
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			//proviamo ad aprire il file corrispondente all'i-esimo processo
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE %d-ESIMO\n",i);
				exit(-1);
			}

			int cont=0;//variabile di conteggio di quante linee nel file ho letto
			int rd=-1;//variabile da associare al ritorno della read
			char c; //variabile per leggere ogni singolo carattere nel file
			long int *occ=calloc(K,sizeof(long int));//array dinamico per contenere le occorrenze del carattere nell'i-esima linea
			bool chk=false;
			

			while(1){
				if(cont==K||rd==0){
					break;
				}
				rd=read(fd,&c,1);
				if(c==C){
					++occ[cont];
					chk=true;
				}
				if(c=='\n'){
					write(piped[i][1],&occ[cont],sizeof(long int));
					++cont;
				}
				

			}
			exit(chk);
		}

	}

	//SONO NEL PADRE
	//chiude tutte le pipe che non usa
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	//recupera tutti i valori dei figli
	//int rd;//variabile da associare al ritorno della read
	long int occ[1];//variabile per contare le occorrenze
	for(i=0;i<N;++i){
		for(int j=0;j<K;++j){
			read(piped[i][0],occ,sizeof(long int));
			printf("IL FIGLIO NUMERO %d ASSOCIATO AL FILE %s ALLA LINEA %d HA TROVATO %ld OCCORRENZE DEL CARATTERE %c\n",i+1,argv[i+1],j+1,*occ,C);
		}
	}

	//il padre aspetta tutti i figli
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
