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

	//controllo sui parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}
	int N=argc-2;
	int fd,fs;
	int pid,pidFiglio,ritorno,status;
	int i,k;

	//controllo sull'ultimo parametro
	int K=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||K<=0){
		printf("L'ULTIMO CARATTERE DEVE ESSERE UN NUMERO INTERO STRETTAMENTE POSITIVO\n");
		exit(2);
	}

	char *Fout=malloc((strlen("output")+1+strlen(argv[argc-1]))*sizeof(char));
	if(Fout==NULL){
		printf("ERRORE NELL'ALLOCAZIONE DI MEMORIA PER FOUT\n");
		exit(3);
	}
	sprintf(Fout,"output.%s",argv[argc-1]);
	if((fd=creat(Fout,0644))<0){
		printf("ERRORE NELLA CREAZIONE DI FOUT\n");
		exit(4);
	}
	close(fd);

	if((fs=open(Fout,O_WRONLY))<0){
				printf("NON RIESCO AD APRIRE IL FILE\n");
					exit(-1);
				
			}

	//dichiarazione delle pipe per la comunicazione figlio padre e padre figlio
	pipe_t *pipedStoP=malloc(N*sizeof(pipe_t));
	if(pipedStoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(5);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedStoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(6);
		}
	}

	pipe_t *pipedPtoS=malloc(N*sizeof(pipe_t));
	if(pipedPtoS==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(7);
	}

	for(i=0;i<N;++i){
		if(pipe(pipedPtoS[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(8);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA GENERAZIONE DEL FIGLIO %d\n",i+1);
			exit(9);
		}
		if(pid==0){
			//chiusura lati pipe che non uso
			for(k=0;k<N;++k){
				close(pipedStoP[k][0]);
				if(k!=i){
					close(pipedStoP[k][1]);
				}
			}
			for(k=0;k<N;++k){
				close(pipedPtoS[k][1]);
				if(k!=i){
					close(pipedPtoS[k][0]);
				}
			}

			//provo ad aprire il file
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE PER IL FIGLIO %d\n",i+1 );
				exit(-1);
			}
			long int lunghezza=0;
			char c;
			while(read(fd,&c,1)!=0){
				++lunghezza;
			}

			//scrivo al padre
			write(pipedStoP[i][1],&lunghezza,sizeof(long int));

			
			

			//attendo la risposta dal padre
			char response[12];
			read(pipedPtoS[i][0],response,sizeof(response));
			char *conc=malloc((strlen(argv[i+1])+1+strlen(response))*sizeof(char));
			sprintf(conc,"%s %s\n",argv[i+1],response);
			printf("%s",conc);
			write(fs,conc,sizeof(conc)+1);
			lseek(fs,0,SEEK_END);
			write(fs,"\n",1);
			if(strcmp(response,"Sopra media")==0){
				exit(2);
			}
			else if(strcmp(response,"Equal media")==0){
				exit(0);
			}
			else{
				exit(1);
			}

		}
	}

	//sono nel padre

	//chiude tutte le pipe che non usa
	for(k=0;k<N;++k){
		close(pipedStoP[k][1]);
		close(pipedPtoS[k][0]);
	}

	//leggo i valori dai figli
	long int lunghezza=0;
	char r1[12]="Sopra media";
	char r2[12]="Equal media";
	char r3[12]="Sotto media";

	for(i=0;i<N;++i){
		read(pipedStoP[i][0],&lunghezza,sizeof(long int));
		if(lunghezza>K){
			write(pipedPtoS[i][1],r1,sizeof(r1));
		}
		else if(lunghezza==K){
			write(pipedPtoS[i][1],r2,sizeof(r2));
		}
		else{
			write(pipedPtoS[i][1],r3,sizeof(r3));
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



	exit(0);
}
