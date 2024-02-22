#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef int pipe_t[2];

int mia_random(int n) {
	int casuale;
	casuale = rand() % n;
	return casuale;
}


int main(int argc,char **argv){

	//controllo sul numero dei parametri
	if(argc<6){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	int N=argc-2;
	int pidFiglio,ritorno,status;
	int i,k,j;
	int fd;
	int *pid=calloc(N,sizeof(int));//array per tenere i pid dei vari figli da poter recuperare
	char c;
	int Fcreato,lunghezza;
	char *s=calloc(256,sizeof(char));
	int *arrl=malloc(N+sizeof(int));//array di lunghezze da utilizzare nel padre per poter scegliere quale indice utilizzare
	int rindex;//indice che ogni figlio riceverà dal padre e che dovrà controllare la sua validità
	int scr;
	int cin1,cin2;

	//controllo sull'ultimo parametro
	int H=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||H<=0||H>255){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO MA MINORE O UGUALE A 255\n");
		exit(9);
	}
	

	//allocazione e creazione delle N pipe per pipeline
	pipe_t *pipedStoP=malloc(N*sizeof(pipe_t));
	if(pipedStoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedStoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(3);
		}
	}

	pipe_t *pipedPtoS=malloc(N*sizeof(pipe_t));
	if(pipedPtoS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(4);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedPtoS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(5);
		}
	}

	//inizializziamo il seme
	srand(time(NULL));

	//creo il file Fcreato
	if((Fcreato=creat("/tmp/creato",0644))<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE CREATO\n");
		exit(6);
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(7);
		}
		if(pid[i]==0){
			//sono dentro al figlio
			//chiudo i lati delle pipe che non utilizzo
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				close(pipedPtoS[k][1]);
				if(k!=i){
					close(pipedStoP[k][1]);
					close(pipedPtoS[k][0]);
				}
			}

			//provo ad aprire il file per ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			//leggo il contenuto del file segnandomi la lunghezza di ogni riga
			j=0;
			lunghezza=0;
			scr=0;

			while(read(fd,&c,1)!=0){
				if(c=='\n'){
					s[j]=c;
					++j;
					s[j]='\0';
					++lunghezza;
					//devo comunicare tale lunghezza al padre
					write(pipedStoP[i][1],&lunghezza,sizeof(int));

					//leggo dal padre sulla pipe
					read(pipedPtoS[i][0],&rindex,sizeof(int));
					if(rindex<lunghezza){
						//posso scrivere sul file fcreato il carattere corrispondente a quell'indice
						write(Fcreato,&s[rindex],1);
						lseek(Fcreato,0,SEEK_END);
						++scr;

					}
					lunghezza=0;
					j=0;

				}
				s[j]=c;
				++j;
				++lunghezza;
			}

			exit(scr);
		}

	}

	//SONO NEL PADRE
	//chiudo le pipe che non mi servono
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
		close(pipedPtoS[k][0]);
	}

	//il padre deve leggere per prima cosa le lunghezze mandate da ciascun figlio
	for(i=0;i<N;++i){
		read(pipedStoP[i][0],&arrl[i],sizeof(int));
		cin1=mia_random(N);//indice per la lunghezza casuale
		cin2=mia_random(arrl[cin1]);//indice per l'indice casuale
		write(pipedPtoS[i][1],&cin2,sizeof(int));
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
