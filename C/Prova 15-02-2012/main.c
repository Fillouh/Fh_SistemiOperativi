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

int main(int argc, char *argv[]) {

	//controllo sul numero di parametri
	if(argc<4){
		printf("ERRORE, NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}
	int N=argc-2;
	int pid,pidFiglio,ritorno,status;
	int fd;
	int i,k;

	//controllo sull'ultimo parametro
	int H=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||H<=0){
		printf("ERRORE,L'ULTIMO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO\n");
		exit(2);
	}

	//dichiarazione delle pipe
	//dichiarazione pipe per comunicazione dei caratteri
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	//dichiarazione pipe per comunicazione del segnale di OK
	pipe_t *com=malloc(N*sizeof(pipe_t));
	if(com==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	//apertura delle varie pipe
	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE-CARATTERI %d-ESIMA\n",i+1);
			exit(5);
		}
	}

	for(i=0;i<N;++i){
		if(pipe(com[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE-COMUNICAZIONE %d-ESIMA\n",i+1);
			exit(6);
		}
	}


	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli N figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA FORK %d-ESIMA\n",i+1);
			exit(7);
		}
		if(pid==0){
			//SONO DENTRO AL FIGLIO
			//chiudo in lettura la pipe dei caratteri
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			//chiudo in scrittura la pipe di comunicazione
			for(k=0;k<N;++k){
				close(com[k][1]);
				if(k!=i){
					close(com[k][0]);
				}
			}

			//provo ad aprire il file i+1-esimo
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d-ESIMO\n",i+1);
				exit(-1);
			}

			char *s=calloc(2,sizeof(char));
			int j=0;
			int sz=2;
			char c;
			int linout=0;//variabile che mi dice quante linee ho stampato sullo stdout;
			bool chk=false;
			int nl=0;
			

			while(read(fd,&c,1)!=0){
				if(c=='\n'||c=='\0'){
					s[j]='\0';
					++nl;
					//scrivo al padre il primo carattere della linea corrente
					write(piped[i][1],&s[0],sizeof(char));

					j=0;
				}
				s[j]=c;
				++j;
				++sz;
				s=realloc(s,sz*sizeof(char));
			}
			printf("%s\n",s);
			//leggo dal padre il valore di ok per la scrittura su stdout
		    while(read(com[i][0],&chk,sizeof(bool))){
						if(chk){
							printf("IL FIGLIO %d CON PID %d CORRISPONDENTE AL FILE %s HA L'OK PER SCRIVERE SU STDOUT\n",i+1,getpid(),argv[i+1]);
							printf("ALLA LINEA %d DEL FILE %s CORRISPONDE LA STRINGA : %s\n",nl,argv[i+1],s);
							++linout;
						}

			}		
					
			exit(linout);

		}
	}

	//SONO NEL PADRE
	//chiude tutte le pipe che non usa
	for(k=0;k<N;++k){
		close(piped[k][1]);
		close(com[k][0]);
	}


	//leggo i valori corrispondenti agli N figli per le H righe
	char *arr=malloc(N*sizeof(char));
	bool cd=false;
	for(int q=0;q<H;++q){
		char max=0;
		int index;
		for(i=0;i<N;++i){
			read(piped[i][0],&arr[i],sizeof(char));
			if(arr[i]>=max){
				max=arr[i];
				index=i;
			}
		}
	
		
		for(i=0;i<N;++i){
			if(i==index){
				cd=true;
				write(com[i][1],&cd,sizeof(bool));
				cd=false;
			}
			else{
				write(com[i][1],&cd,sizeof(bool));
			}
		}
		
	}

	for(i=0;i<N;++i){
		close(com[i][1]);
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
