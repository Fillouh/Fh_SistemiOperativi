#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef int pipe_t[2];

int main(int argc, char **argv){
	//controllo sul numero di parametri
	if(argc<4){
		printf("ERRORE, NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}
	int N=argc-2;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,ritorno2,status1,status2;
	int fd;
	int i,k;
	int *pidf=malloc(N+sizeof(int));

	//controllo sull'ultimo parametro
	int H=atoi(argv[argc-1]);
	if(argv[argc-1]==NULL||H<=0){
		printf("ERRORE, L'ULTIMO PARAMETRO DEVE ESSERE UN INTERO STRETTAMENTE POSITIVO\n");
		exit(2);
	}

	//allocazione di memoria e creazione della pipe tra figli e padre
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

	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA FORK DEL FIGLIO %d-ESIMO\n",i);
			exit(4);
		}
		if(pid==0){
			//SONO DENTRO AL FIGLIO
			printf("DEBUG-SONO IL PROCESSO FIGLIO CON PID %d E STO PER ESEGUIRE I MIEI COMPITI\n",getpid());
			pidf[i]=getpid();
			//chiudo il lato della pipe che non utilizzo con il padre
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}
			//creo una pipe per il nipote
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
				//SONO DENTRO AL NIPOTE
				close(p[0]);
				/*
				//provo ad aprire il file
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE DI APERTURA DEL FILE NEL NIPOTE %d-ESIMO",i);
					exit(-1);
				}*/

				close(0);
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE DI APERTURA DEL FILE NEL NIPOTE %d-ESIMO\n",i);
					exit(-1);
				}
				close(1);
				dup(p[1]);
				close(p[1]);
				execl("/usr/bin/wc","wc","-l",(char *)0);
				printf("ERRORE NELLA EXEC\n");
				exit(-1);

			}
			//chiudo il lato della pipe nipote che non uso
			close(p[1]);
			char *s=calloc(2,sizeof(char));
			char c;
			int j=0;
			int sz=2;
			while(read(p[0],&c,1)){
				s[j]=c;
				++j;
				++sz;
				s=realloc(s,sz*sizeof(char));
			}
			s[j]='\0';
			int lunghezza=atoi(s);
			char s1[]="Sopra la media";
			char s2[]="Sotto la media";
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
			if(lunghezza>=H){
				write(piped[i][1],s1,sizeof(s1));
			}
			else{
				write(piped[i][1],s2,sizeof(s2));
			}

			exit(ritorno1);
		}
	}

	//sono dentro al padre
	//chiude tutte le pipe che non usa
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

//recupero i valori comunicati dal figlio
	char *s=calloc(2,sizeof(char));
	char c;
	int j=0;
	int sz=2;
	/*for(i=0;i<N;++i){
		while(read(piped[i][0],&c,sizeof(char))){
		s[j]=c;
		++j;
		++sz;
		s=realloc(s,sz*sizeof(char));
		}
		s[j]='\0';
		printf("IL FIGLIO NUMERO %d CON PID %d RELATIVO AL FILE %s HA RITORNATO : %s\n",i+1,pidf[i],argv[i+1],s);
	}*/
	

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
		while(read(piped[i][0],&c,sizeof(char))){
		s[j]=c;
		++j;
		++sz;
		s=realloc(s,sz*sizeof(char));
		}
		s[j]='\0';
		printf("IL FIGLIO NUMERO %d CON PID %d RELATIVO AL FILE %s HA RITORNATO : %s\n",i+1,pidFiglio,argv[i+1],s);
			ritorno2=(int)((status2 >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d\n",pidFiglio,ritorno2);

		}
	}

	
	



	printf("HO FINITO TUTTO\n");
	exit(0);
}
