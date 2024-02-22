//PROVA CON FIGLI CHE COMUNICANO CON I NIPOTI
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

int Cifre(int num){
	int cifre=0;
	while(num>0){
		num/=10;
		++cifre;
	}
	return cifre;
}

int main(int argc,char **argv){

	int N=argc-1;
	int fd,fcreato;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,status1,ritorno2,status2;
	int i,k;
	char c;
	bool ck,ck1;

	//controllo sul numero dei parametri
	if(argc<3||((argc-1)%2)!=0){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO OPPURE NON PARI!\n");
		exit(1);
	}

	//allocazione per le n/2 pipe per la comunicazione tra figlio e nipote
	pipe_t *pipedStoN=malloc((N/2)*sizeof(pipe_t));
	if(pipedStoN==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}
	//allocazione per le n/2 pipe per la comunicazione tra nipote e figlio
	pipe_t *pipedNtoS=malloc((N/2)*sizeof(pipe_t));
	if(pipedNtoS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<N/2;++i){
		if(pipe(pipedStoN[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
		if(pipe(pipedNtoS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(5);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N/2);

	//genero gli n/2 figli
	for(i=0;i<N/2;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d",i+1);
			exit(6);
		}
		if(pid==0){
			//sono dentro al figlio

			//creo il file fcreato
			int cifre=Cifre(i);
			char *nome=malloc((strlen("merge")+cifre+1)*sizeof(char));
			if(nome==NULL){
				printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER IL FILE FCREATO NEL FIGLIO %d\n",i);
				exit(-1);
			}
			sprintf(nome,"merge%d",i);
			if((fcreato=creat(nome,0644))<0){
				printf("ERRORE NELLA CREAZIONE DEL FILE FCREATO PER IL FIGLIO %d",i);
				exit(-1);
			}

			//apro il file su cui devo leggere
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i);
				exit(-1);
			}
			//passo alla creazione del nipote
			if((pid1=fork())<0){
				printf("ERRORE NELLA CREAZIONE DEL NIPOTE %d\n",i);
				exit(-1);
			}
			if(pid1==0){
				//SONO DENTRO AL NIPOTE
				//chiudo le pipe che non utilizzo
				for(k=0;k<N;++k){
					close(pipedStoN[k][1]);
					close(pipedNtoS[k][0]);
					if(k!=i){
						close(pipedStoN[k][0]);
						close(pipedNtoS[k][1]);
					}
				}

				//apro il file che mi serve
				if((fd=open(argv[N/2+i+1],O_RDONLY))<0){
					printf("ERRORE NELL'APERTURA DEL FILE NEL NIPOTE %d",i);
					exit(-1);
				}

				//processo di lettura
				ck1=false;
				while(read(fd,&c,1)!=0){
					read(pipedStoN[i][0],&ck,sizeof(bool));
					if(ck==true){
						printf("IL NIPOTE SCRIVE \n");
						ck=false;
						//posso scrivere sul file fout
						write(fcreato,&c,sizeof(char));
						/*if(c==10){
							ck1=false;
						}*/
						ck1=true;
						write(pipedNtoS[i][1],&ck1,sizeof(bool));
					}
						
					
				}

				exit(c);
			}
			//sono di nuovo nel figlio
			//chiudo i lati delle pipe che non utilizzo
			for(k=0;k<N;++k){
				close(pipedStoN[k][0]);
				close(pipedNtoS[k][1]);
				if(k!=i){
					close(pipedStoN[k][1]);
					close(pipedNtoS[k][0]);
				}
			}


			while(read(fd,&c,1)!=0){
				//ck1=false;
				printf("FIGLIO SCRIVE\n");
				write(fcreato,&c,sizeof(char));
				ck=true;
				write(pipedStoN[i][1],&ck,sizeof(bool));
				read(pipedNtoS[i][0],&ck1,sizeof(bool));
				/*if(ck1==false){
					break;
				}*/
				
			}

			//aspetto il nipote
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

			exit(ritorno1);
		}
	}
	//sono nel padre
	//chiudo tutti i lati delle pipe
	for(k=0;k<N;++k){
		close(pipedStoN[k][0]);
		close(pipedStoN[k][1]);
		close(pipedNtoS[k][0]);
		close(pipedNtoS[k][1]);
	}

	//aspetto tutti i figli
	for( i=0;i<N;++i){
		if((pidFiglio=wait(&status2))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status2 & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO CON PID %d SI E' CHIUSO IN MODO ANOMALO\n",pidFiglio);

		}
		else{
			ritorno2=(int)((status2 >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno2);
		}
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
