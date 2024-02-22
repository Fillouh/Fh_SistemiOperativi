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


int main(int argc, char **argv){

	int N=argc-1;
	int fd,fcreato;
	int pid,pidFiglio,ritorno,status;
	int i,k,j;
	char c;
	char s[250];
	int L;
	char linea[200];
	int lunghezza,rit;
	

	//controllo sul numero dei parametri
	if(argc<3){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//allocazione e gestione per la creazione del file fcreato
	char *nome=malloc((strlen("/tmp/")+strlen("Filippo")+strlen("Leonelli")+1)*sizeof(char));
	if(nome==NULL){
		printf("ERRORE NELL' ALLOCAZIONE DI MEMORIA PER NOME\n");
		exit(2);
	}
	sprintf(nome,"/tmp/FilippoLeonelli");

	//provo a creare il file fcreato
	if((fcreato=creat(nome,0644))<0){
		printf("ERRORE NELLA CREAZIONE DEL FILE FCREATO\n");
		exit(3);
	}

	//creazione di una pipe per il figlio speciale
	pipe_t p;
	if(pipe(p)<0){
		printf("ERRORE NELLA CREAZIONE DELLA PIPE PER IL FIGLIO SPECIALE\n");
		exit(4);
	}

	//creazione di un processo figlio speciale
	if((pid=fork())<0){
		printf("ERRORE NELLA CREAZIONE DEL FIGLIO SPECIALE\n");
		exit(5);
	}
	if(pid==0){
		//SONO NEL FIGLIO SPECIALE
		close(0);
		close(p[0]);
		//provo ad aprire il primo file
		if((fd=open(argv[1],O_RDONLY))<0){
			printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO SPECIALE\n");
			exit(-1);
		}
		close(1);
		dup(p[1]);
		close(p[1]);
		execl("/usr/bin/wc","wc","-l",(char*)0);
		printf("ERRORE NELLA EXEC\n");
		exit(-1);
	}
	//ritorno nel padre
	close(p[1]);
	j=0;
	while(read(p[0],&c,1)){
		s[j]=c;
		++j;
	}
	s[j]='\0';
	L=atoi(s);
	if(L<=0){
		printf("NON HO TROVATO UN INDICE DI LINEE SUFFICIENTE\n");
		exit(6);
	}

	//aspetto il figlio speciale
	if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("IL FIGLIO SPECIALE CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno);
		}


	//allocazione per le n pipe
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(7);
	}

	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(8);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d",i+1);
			exit(9);
		}
		if(pid==0){
			//sono nei figli
			//chiudo tutte le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(piped[k][0]);
				close(p[0]);
				close(p[1]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			//provo ad aprire per ogni figlio il proprio file
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE NEL FIGLIO %d",i);
				exit(-1);
			}

			//leggo linea per linea del mio file
			j=0;
			lunghezza=0;
			while(read(fd,&c,1)!=0){
				linea[j]=c;
				++j;
				++lunghezza;
				if(c=='\n'){
					linea[j]='\0';
					//comunico al padre prima la lunghezza della linea
					write(piped[i][1],&lunghezza,sizeof(int));
					//poi la linea
					write(piped[i][1],linea,(lunghezza)*sizeof(char));
					rit=lunghezza;
					j=0;
					lunghezza=0;
				}
			}
			exit(rit);
		}
	}

	//sono nel padre
	//chiudo le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	//leggo i valori per ogni figlio
	for(i=0;i<N;++i){
		read(piped[i][0],&lunghezza,sizeof(int));
		read(piped[i][0],linea,(lunghezza)*sizeof(char));
		write(fcreato,linea,(lunghezza)*sizeof(char));
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
