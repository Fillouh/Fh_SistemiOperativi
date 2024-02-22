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
	if(argc<3){
		printf("ERRORE, NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}

	int N=argc-2;
	int fd;
	int pid,pidFiglio,ritorno,status;
	int i,k;
	char c;

	//controllo sull'ultimo parametro
	if(argv[argc-1]==NULL|| strlen(argv[argc-1])!=1){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(2);
	}
	char Cx=argv[argc-1][0];

	//allocazione della memoria per le n pipe
	//PIPE PER FIGLIO PADRE
	pipe_t *pipedStoP=malloc(N*sizeof(pipe_t));
	if(pipedStoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(pipedStoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(3);
		}
	}

	//PIPE PER PADRE FIGLIO
	//allocazione della memoria per le n pipe
	pipe_t *pipedPtoS=malloc(N*sizeof(pipe_t));
	if(pipedPtoS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(pipedPtoS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(5);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(6);
		}
		if(pid==0){
			//SONO NEL FIGLIO
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				close(pipedPtoS[k][1]);
				if(k!=i){
					close(pipedStoP[k][1]);
					close(pipedPtoS[k][0]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[i+1],O_RDWR))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			//leggo dal file
			long int pos=0;
			char ck;
			int sos=0;//numero di sostituzioni effettuate
			while(read(fd,&c,1)!=0){
				if(c==Cx){
					//comunico al padre la posizione del carattere seguente
					write(pipedStoP[i][1],&pos,sizeof(long int));
					sleep(1);

					//leggo dal padre se sostituire o meno il carattere corrente
					read(pipedPtoS[i][0],&ck,sizeof(char));
					if(ck!=10){
						lseek(fd,-1,SEEK_CUR);
						write(fd,&ck,1);
						++sos;
					}
				}
				++pos;
			}
			exit(sos);
		}
	}
	//SONO NEL PADRE
	//chiudo le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
		close(pipedPtoS[k][0]);
	}

	long int *position=malloc(N*sizeof(long int));
	char ck;
	char s[256];//stringa su cui ricevo da stdin
	for(i=0;i<N;++i){
		//leggo la posizione per ogni figlio
		while(read(pipedStoP[i][0],&position[i],sizeof(long int))){
			printf("IL PADRE HA RICEVUTO DAL FIGLIO DI INDICE %d CORRISPONDENTE AL FILE %s LA POSIZIONE %ld\n",i,argv[i+1],position[i]);
			printf("VUOI SOSTITUIRE IL CARATTERE %c ALLA POSIZIONE %ld ? (DIGITA IL CARATTERE CHE VUOI OPPURE PREMI INVIO E BASTA SE NON VUOI SOSTITUIRLO)\n",Cx,position[i]);
			printf("DIGITA: \n");
			read(0,s,sizeof(char)*256);
			while(strlen(s)!=2){
				printf("SONO NEL CICLO\n");
				printf("\n MI DEVI DARE O UN CARATTERE O SEMPLICEMENTE UNA STRINGA VUOTA PREMENDO L'INVIO\n");
				printf("DIGITA : \n");
				//scanf("%s",s);
				read(0,s,sizeof(char)*256);
			}
			ck=s[0];
			//scrivo al figlio ciò che mi ha mandato l'utente
			write(pipedPtoS[i][1],&ck,sizeof(char));
		}
		

	}

	//aspetto tutti i figli
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
