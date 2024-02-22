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
	char c;
	long int position;
}str;


int main(int argc,char **argv){

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}
	int N=argc-3;
	int pid,pid1,pidFiglio,pidNipote,ritorno1,ritorno2,status1, status2;
	int fd,fd1;
	int i,k;

	//controlli sui caratteri
	if(argv[argc-2]==NULL||strlen(argv[argc-2])!=1){
		printf("ERRORE, IL PENULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(2);
	}
	char C1=argv[argc-2][0];

	if(argv[argc-1]==NULL||strlen(argv[argc-1])!=1){
		printf("ERRORE, IL PENULTIMO PARAMETRO DEVE ESSERE UN SINGOLO CARATTERE\n");
		exit(3);
	}
	char C2=argv[argc-1][0];


	//allocazione e generazione delle pipe padre figlio e padre nipote
	//pipe padre nipote
	pipe_t *pipedNtoP=malloc(N*sizeof(pipe_t));
	if(pipedNtoP==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(6);
	}
	for(i=0;i<N;++i){
		if(pipe(pipedNtoP[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d-ESIMA\n",i);
			exit(7);
		}
	}

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI CHE GENERERANNO %d NIPOTI\n",getpid(),N,N);

	//generazione degli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			printf("ERRORE NELLA GENERAZIONE DEL FIGLIO %d\n",i+1);
			exit(8);
		}
		if(pid==0){
			//SONO DENTRO AL FIGLIO

			//creazione della pipe per la comunicazione con il nipote
			pipe_t p;
			if(pipe(p)<0){
				printf("ERRORE NELLA CREAZIONE DELLA PIPE PER IL NIPOTE\n");
				exit(-1);
			}
			//provo ad aprire il file
			if((fd=open(argv[i+1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			//creo il nipote
			if((pid1=fork())<0){
				printf("ERRORE NELLA GENERAZIONE DEL NIPOTE\n");
				exit(-1);
			}
			if(pid1==0){
				//SONO DENTRO AL NIPOTE

				//chiudo il lato di lettura nella pipe con il padre
				for(k=0;k<N;++k){
					close(pipedNtoP[k][0]);
					if(k!=i){
						close(pipedNtoP[k][1]);
					}
				}

				//chiude il lato di scrittura con la pipe_t p
				close(p[1]);

				//provo ad aprire il file
				if((fd1=open(argv[i+1],O_RDONLY))<0){
					printf("ERRORE DI APERTURA DEL FILE NEL NIPOTE %d\n",i+1);
					exit(-1);
				}

				//avvio la lettura sul file
				str gg2;
				char c;
				int occ2;
				long int pos2=0;
				str rstr; //struttura di lettura


				while(read(fd1,&c,1)!=0){
					if(c==C2){
						++occ2;
						read(p[0],&rstr,sizeof(str));
						if(rstr.position>pos2){
							//comunico al padre una struttura pc2 e c2
							gg2.c=C2;
							gg2.position=pos2;
							write(pipedNtoP[i][1],&gg2,sizeof(str));

						}
						else{
							//comunico al padre una struttura pc1 e c1
							gg2.c=rstr.c;
							gg2.position=rstr.position;
							write(pipedNtoP[i][1],&gg2,sizeof(str));

						}

					}
					++pos2;
				}

				exit(occ2);
			}
			//sono di nuovo nel figlio
			close(p[0]);
			str gg1;
			char c;
			int occ1;
			long int pos1=0;

			while(read(fd,&c,1)!=0){
				if(c==C1){
					++occ1;
					gg1.c=C1;
					gg1.position=pos1;
					write(p[1],&gg1,sizeof(str));
				}
				++pos1;
			}

			//aspettare il nipote
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

			exit(occ1);

		}
	}

	//SONO NEL PADRE

	//chiudere tutte le pipe che non utilizzo
	for(k=0;k<N;++k){
		close(pipedNtoP[k][1]);
	}

	//leggo i valori
	str gg3;
	for(i=0;i<N;++i){
		while(read(pipedNtoP[i][0],&gg3,sizeof(str))){
			printf("IL PADRE HA RICEVUTO LA POSIZIONE %ld DEL CARATTERE %c NEL FILE %s\n",gg3.position,gg3.c,argv[i+1]);
		}
	}

	//aspetto i figli
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
