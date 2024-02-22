//PROVA CON FIGLI A COPPIE
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
#include <math.h>

typedef int pipe_t[2];

int main(int argc,char **argv){

	int N=argc-2;
	int fd;
	int pid,pidFiglio,ritorno,status;
	int i,k;
	char c,C;
	long int pos;
	int occ;
	int nw,nr;

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//controllo sull'ultimo parametro
	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(2);
	}
	C=argv[argc-1][0];


	//allocazione per le n pipe 
	pipe_t *pipedF1toF2=malloc(N*sizeof(pipe_t));
	if(pipedF1toF2==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	pipe_t *pipedF2toF1=malloc(N*sizeof(pipe_t));
	if(pipedF2toF1==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedF1toF2[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
		if(pipe(pipedF2toF1[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),2*N);

	//genero i 2*n figli
	for(i=0;i<2*N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(5);
		}
		if(pid==0){
			//sono dentro ai figli
			
			if(i<N){
				//sono dentro ai primi figli della coppia
				//chiudo i lati delle pipe che non utilizzo
				for(k=0;k<N;++k){
					close(pipedF1toF2[k][0]);
					close(pipedF2toF1[k][1]);
					if(k!=i){
						close(pipedF1toF2[k][1]);
						close(pipedF2toF1[k][0]);
					}
				}

				//apro il file relativo al mio indice
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
					exit(-1);
				}

				//leggo dal file
				occ=0;
				while(read(fd,&c,1)!=0){
					if(c==C){
						++occ;
						pos=lseek(fd,0,SEEK_CUR);
						write(pipedF1toF2[i][1],&pos,sizeof(long int));
						nr=read(pipedF2toF1[i][0],&pos,sizeof(long int));
						if(nr!=sizeof(long int)){
							break;
						}
					}
				}
				/*while(1){
					nr=read(fd,&c,1);
					printf("%c\n",c);
					if(nr==0){
						//vuol dire che sono arrivato alla fine del file
						pos=-1;
						if(write(pipedF1toF2[i][1],&pos,sizeof(long int))!=sizeof(long int)){
							printf("ERRORE NELLA WRITE DELLA POSIZIONE\n");
							exit(-1);
						}
						break;
					}
					else{
						//ho letto qualcosa quindi devo verificare che abbia trovato un'occorrenza
						if(c==C){
							++occ;
							pos=lseek(fd,0,SEEK_CUR);
							printf("POSIZIONE TROVATA %ld\n",pos);
							//scriviamo la posizione trovata sull'altra pipe
							if(write(pipedF1toF2[i][1],&pos,sizeof(long int))!=sizeof(long int)){
								printf("ERRORE NELLA WRITE DELLA POSIZIONE\n");
								exit(-1);
							}
							//leggiamo la posizione che ci manda il secondo figlio
							if(read(pipedF2toF1[i][0],&pos,sizeof(long int))!=sizeof(long int)){
								printf("ERRORE NELLA READ DELLA POSIZIONE\n");
								exit(-1);
							}
							if(pos==-1){
								break;
							}
							lseek(fd,pos,SEEK_SET);
						}
					}
				}*/
				


			}
			else{
				//sono dentro ai secondi figli della coppia
				//chiudo i lati delle pipe che non utilizzo
				for(k=0;k<N;++k){
					close(pipedF1toF2[k][1]);
					close(pipedF2toF1[k][0]);
					if(k!=2*N-i-1){
						close(pipedF1toF2[k][0]);
						close(pipedF2toF1[k][1]);
					}
				}

				//apro il file relativo al mio indice
				if((fd=open(argv[2*N-i],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
					exit(-1);
				}

				//leggo dalla pipe
				nr=read(pipedF1toF2[2*N-i-1][0],&pos,sizeof(long int));
				lseek(fd,pos,SEEK_SET);
				while(read(fd,&c,1)!=0){
					if(c==C){
						++occ;
						pos=lseek(fd,0,SEEK_CUR);
						write(pipedF2toF1[2*N-1-i][1],&pos,sizeof(long int));
						nr=read(pipedF1toF2[2*N-1-i][0],&pos,sizeof(long int));
						if(nr!=sizeof(long int )){
							break;
						}
					}
				}

				/*while(1){
					if(read(pipedF1toF2[2*N-i-1][0],&pos,sizeof(long int))){
						printf("ERRORE NELLA LETTURA DELLA POSIZIONE\n");
						exit(-1);					
					}
					if(pos==-1){
						break;
					}
					lseek(fd,pos,SEEK_SET);
					nr=read(fd,&c,1);
					if(nr==0){
						pos=-1;
						if(write(pipedF2toF1[2*N-i-1][1],&pos,sizeof(long int))!=sizeof(long int)){
							printf("ERRORE NELLA WRITE DELLA POSIZIONE\n");
							exit(-1);
						}
						break;
					}
					else{
						if(c==C){
							++occ;
							pos=lseek(fd,0,SEEK_CUR);
							if(write(pipedF2toF1[2*N-i-1][1],&pos,sizeof(long int ))!=sizeof(long int)){
								printf("ERRORE NELLA WRITE DELLA POSIZIONE\n");
								exit(-1);
							}
							if(read(pipedF1toF2[2*N-i-1][0],&pos,sizeof(long int))!=sizeof(long int )){
								printf("ERRORE NELLA READ DELLA POSIZIONE\n");
								exit(-1);
							}
							if(pos==-1){
								break;
							}
							lseek(fd,pos,SEEK_SET);
						}
					}

				}*/
				
			}
			
			exit(occ);
		}
	}
	//sono nel padre
	//chiudo tutte le pipe
	for(k=0;k<N;++k){
		close(pipedF1toF2[k][0]);
		close(pipedF1toF2[k][1]);
		close(pipedF2toF1[k][0]);
		close(pipedF2toF1[k][1]);
	}


	//aspetto tutti i figli
	for( i=0;i<2*N;++i){
		if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(6);
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
