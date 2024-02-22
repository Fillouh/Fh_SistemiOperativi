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
	if(argc!=4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int i,k;
	int pid, pidFiglio,ritorno,status;
	int fd,fsp,fst;

	//controllo sul secondo parametro
	int N=atoi(argv[2]); //numero di linee del file
	if(argv[2]==NULL||N<=0){
		printf("ERRORE, IL SECONDO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO\n");
		exit(2);
	}

	//controllo sul terzo parametro
	int H=atoi(argv[3]);//lunghezza media delle linee
	if(argv[3]==NULL||H<=0){
		printf("ERRORE, IL TERZO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO\n");
		exit(3);
	}

	//creazione dei file fsotto e fsopra
	//allocazione della memoria per i file fsotto e fsopra

	char *name1=malloc((strlen(argv[1]+1+strlen(".sopra")))*sizeof(char));
	sprintf(name1,"%s.sopra",argv[1]);
	
	if(name1==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER I FILE\n");
		exit(8);
	}
	fsp=creat(name1,0644);
	if(fsp<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE SOPRA\n");
		exit(4);
	}
	close(fsp);

	char *name2=malloc((strlen(argv[1]+1+strlen(".sotto")))*sizeof(char));
	sprintf(name2,"%s.sotto",argv[1]);
	fst=creat(name2,0644);
	if(fst<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE SOTTO\n");
		exit(5);
	}
	close(fst);

	//creazione delle varie pipe
	pipe_t *pipedSopra=malloc(N*sizeof(pipe_t));
	if(pipedSopra==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(6);
	}
	for(i=0;i<N;++i){
		if(pipe(pipedSopra[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(7);
		}
	}

	pipe_t *pipedSotto=malloc(N*sizeof(pipe_t));
	if(pipedSotto==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(6);
	}
	for(i=0;i<N;++i){
		if(pipe(pipedSotto[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(7);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N+2);

	for(i=0;i<N+2;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA FORK DEL %d FIGLIO",i+1);
			exit(6);
		}
		if(pid==0){
			//chiusura dei lati giusti delle pipe
			if(i<N){
				for(k=0;k<N;++k){
					close(pipedSopra[k][0]);
					close(pipedSotto[k][0]);
					if(k!=i){
						close(pipedSopra[k][1]);
						close(pipedSotto[k][1]);
					}
				}

				//provo ad aprire il file F
				if((fd=open(argv[1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d",i+1);
					exit(-1);
				}
				int lunghezza=0;
				char c;
				char *s=calloc(2,sizeof(char));
				int j=0;
				int sz=2;
				int linx=0;//indice di linea
				while(read(fd,&c,1)!=0){
					if(c=='\n'){
						++lunghezza;
						s[j]=c;
						if(i==linx){
							++j;
							++sz;
							s=realloc(s,sz*sizeof(char));
							s[j]='\0';

							//controllo il valore di lunghezza
							if(lunghezza>=H){
								//vai sotto
								write(pipedSotto[i][1],s,sizeof(s));
							}
							else{
								//vai sopra
								write(pipedSopra[i][1],s,sizeof(s));
							}
						}
						++linx;
						lunghezza=0;
						j=0;
					}
					++lunghezza;
					s[j]=c;
					++j;
					++sz;
					s=realloc(s,sz*sizeof(char));
				}
				
				exit(lunghezza);

			}
			else{
				for(k=0;k<N;++k){
					close(pipedSopra[k][1]);
					close(pipedSotto[k][1]);
					if(k!=i){
						close(pipedSopra[k][0]);
						close(pipedSotto[k][0]);
					}
				}

				//si apre la distinzione tra i due processi
				if(i==N){
					//siamo in PSOPRA
					close(pipedSotto[i][0]);
					close(pipedSotto[i][1]);
					close(pipedSopra[i][1]);

					//leggo i valori ricevuti sulla pipe
					char c;
					char *s=calloc(2,sizeof(char));
					int j=0;
					int sz=2;
					int linee=0;
					while(read(pipedSopra[i][0],&c,sizeof(char))){
						if(c=='\n'){
							++linee;
						}
						s[j]=c;
						++j;
						++sz;
						s=realloc(s,sz*sizeof(char));
					}

					//scriviamo la linea sul file
					write(fsp,s,sizeof(s));
					exit(linee);
				}
				else{
					//siamo in PSOTTO
					close(pipedSopra[i][0]);
					close(pipedSopra[i][1]);
					close(pipedSotto[i][1]);

					char c;
					char *s=calloc(2,sizeof(char));
					int j=0;
					int sz=2;
					int linee=0;
					while(read(pipedSotto[i][0],&c,sizeof(char))){
						if(c=='\n'){
							++linee;
						}
						s[j]=c;
						++j;
						++sz;
						s=realloc(s,sz*sizeof(char));
					}

					//scriviamo la linea sul file
					write(fst,s,sizeof(s));
					exit(linee);
				}
			}



		}
	}
	//sono nel padre
	//chiudo tutte le pipe perchè non le utilizzo
	for(k=0;k<N;++k){
		close(pipedSopra[k][0]);
		close(pipedSotto[k][0]);
		close(pipedSopra[k][1]);
		close(pipedSotto[k][1]);
	}

	//aspetto tutti i figli
	for( i=0;i<N+2;++i){
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
