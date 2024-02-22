//PROVA CON FIGLI A COPPIE
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
	if(argc<2){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}
	int N=argc-1;
	int fd,fcreato;
	int pid,pidFiglio,ritorno,status;
	int n,k;
	char cognome[20];
	char c;
	char linea[250];
	printf("MI POTRESTI DIRE IL/I TUO/I COGNOME/I ?\n");
	printf("DIGITA: ");
	scanf("%s",cognome);
	//mi salvo solo un cognome
	int j=0;
	while(cognome[j]!=' '){
		++j;
	}
	cognome[j]='\0';

	//ora creo un file con il cognome
	char *nomef=malloc((strlen(cognome)+strlen(".log"))*sizeof(char));
	sprintf(nomef,"%s.log",cognome);

	//provo a creare il file fcreato
	if((fcreato=creat(nomef,0644))<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE FCREATO\n");
		exit(2);
	}



	//allocazione della memoria per le n pipe
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	//creazione delle n pipe
	for(n=0;n<N;++n){
		if(pipe(piped[n])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",n);
			exit(4);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero 2*n figli
	int i=1;
	for(n=0;n<2*N;++n){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",n+1);
			exit(5);
		}
		if(pid==0){
			//sono nel figlio
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=n){
					close(piped[k][1]);
				}
			}

			//provo ad aprire il file per ognuno dei figli
			if((fd=open(argv[i],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",n+1);
				exit(-1);
			}
			int nro=0;

			if(n==0||(n%2)==0){
				//processi pari
				j=0;
				int lunghezza=0;
				int indl=1;//indice di linea
				while(read(fd,&c,1)!=0){
					if((indl%2)==0){
						linea[j]=c;
						++j;
						++lunghezza;
						if(c=='\n'){
							if(lunghezza>nro){
								nro=lunghezza;
							}
							++indl;
							j=0;
							lunghezza=0;
							//comunico al padre la lunghezza della linea pari
							write(piped[n][1],&lunghezza,sizeof(int));
						}
					}
					if(c=='\n'){
						++indl;
					}
					
				}
			}
			else{
				//processi dispari
				j=0;
				int lunghezza=0;
				int indl=1;//indice di linea
				while(read(fd,&c,1)!=0){
					if((indl%2)!=0){
						linea[j]=c;
						++j;
						++lunghezza;
						if(c=='\n'){
							if(lunghezza>nro){
								nro=lunghezza;
							}
							++indl;
							j=0;
							lunghezza=0;
							//comunico al padre la lunghezza della linea pari
							write(piped[n][1],&lunghezza,sizeof(int));
						}
					}
					if(c=='\n'){
						++indl;
					}
					
				}

			}


			exit(nro);

		}
		if((n%2)==0){
			++i;
		}
	}

	//sono nel padre
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	int q=1;
	int lunghezza=0;
	
	for(n=0;n<2*N;++n){
		printf("LE LUNGHEZZE DELLE LINEE DEL FILE %s SONO :\n",argv[q]);
		int indl=1;
		while(read(piped[n][0],&lunghezza,sizeof(int))){
			printf("LA LINEA %d HA COME LUNGHEZZA %d\n",indl,lunghezza);
			++indl;
		}
		if((n%2)==0){
			++q;
		}
	}


	//aspetto tutti i figli
	for( n=0;n<2*N;++n){
		if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno=(int)((status >> 8) & 0xFF);
			write(fcreato,&pidFiglio,sizeof(int));
			write(fcreato,&ritorno,sizeof(int));
			write(fcreato,'\n',sizeof(char));
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno);
		}
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
