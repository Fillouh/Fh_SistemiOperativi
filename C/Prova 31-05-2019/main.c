//SECONDA PROVA PARZIALE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef int pipe_t[2];

typedef struct{
	int c1;
	int c2;
	char c3[250];
}str;


int main(int argc,char **argv){
	int N=argc-1;
	//int fd;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,status1,ritorno2,status2;
	int i,k,j;
	str gg;
	int lunghezza;

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//allocazione della memoria per le n pipe
	//pipe figlio-padre
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(2);
	}

	//creazione delle n pipe
	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(3);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI\n",getpid(),N);

	//generazione degli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d",i+1);
			exit(4);
		}
		if(pid==0){
			//sono dentro al figlio
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			pipe_t p;
			if(pipe(p)<0){
				printf("ERRORE NELLA CREAZIONE DELLA PIPE PER IL NIPOTE\n");
				exit(-1);
			}

			//creo il nipote che mi serve per leggere
			if((pid1=fork())<0){
				printf("ERRORE NELLA FORK DEL NIPOTE %d-ESIMO\n",i);
				exit(-1);
			}
			if(pid1==0){
				//sono nel nipote
				close(p[0]);
				close(1);
				dup(p[1]);
				close(p[1]);
				execl("/usr/bin/sort","sort","-f",argv[i+1],(char*)0);
				printf("ERRORE NELL'EXEC\n");
				exit(-1);
			}
			close(p[1]);

			//leggo carattere per carattere ciò che ottengo dal nipote
			j=0;
			lunghezza=0;
			while(read(p[0],&(gg.c3[j]),1)!=0){
				if(gg.c3[j]=='\n'){
					++lunghezza;
					gg.c2=lunghezza;
					break;
				}
				++j;
				++lunghezza;
			}

			//aspetto il nipote
			if((pidNipote=wait(&status1))<0){
			printf("ERRORE NELLA WAIT DEL FIGLIO\n");
			exit(7);
			}
			if((status1 & 0XFF)!=0){
				printf("ERRORE, IL NIPOTE SI E' CHIUSO IN MODO ANOMALO\n");

			}
			else{
				ritorno1=(int)((status1 >> 8) & 0xFF);
				printf("IL NIPOTE CON PID %d HA RITORNATO %d\n",pidNipote,ritorno1);
			}

			gg.c1=ritorno1;
			write(piped[i][1],&gg,sizeof(str));
			exit(lunghezza-1);
		}
	}
	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	//leggo i valori delle strutture da tutti i figli
	for(i=0;i<N;++i){
		read(piped[i][0],&gg,sizeof(str));
		gg.c3[gg.c2]='\0';
		printf("IL FIGLIO DI INDICE %d RELATIVO AL FILE %s HA CONFEZIONATO LA SEGUENTE STRUTTURA : C1 : %d, C2 : %d, C3 : %s",i,argv[i+1],gg.c1,gg.c2,gg.c3);
	}

	//aspetto tutti i figli
	for( i=0;i<N;++i){
		if((pidFiglio=wait(&status2))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status2 & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno2=(int)((status2 >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno2);
		}
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
