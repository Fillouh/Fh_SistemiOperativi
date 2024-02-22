//SECONDA PROVA PARZIALE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

typedef int pipe_t[2];
typedef struct{
	int c1;//pid processo nipote
	char c2[250]; //dovrà contenere la linea
	int c3; //lunghezza della linea
} ST;

int main(int argc,char **argv){
	printf("QUESTA E' LA SECONDA PROVA PARZIALE DI FILIPPO LEONELLI IN DATA 01/06/2023:\n");

	//VARIABILI LOCALI
	int Z=argc-1; //numero dei figli da generare
	int fd; //variabile utilizzata come file descriptor
	int pid,pid1,pidFiglio,pidNipote,ritorno1,status1,ritorno2,status2; //gestione dei processi e della wait
	int z,k,j; //indici per i cicli
	char Buff[250];//variabile per leggere via via la linea corrente del file
	ST strutt; //struttura dati sia che devono inviare i figli sia quella che riceve il padre
	char c; //carattere che leggo nelle varie read
	int lunghezza; //variabile per misurare la lunghezza della linea
	int nr,nw;//variabili di controllo delle read e delle write

	//controllo sul numero dei parametri
	if(argc<3){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//allocazione della memoria per le Z pipe
	//il padre leggerà sulla pipe mentre il figlio scriverà
	pipe_t *piped=malloc(Z*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA PER LA PIPE\n");
		exit(2);
	}

	//creazione delle Z pipe
	for(z=0;z<Z;++z){
		if(pipe(piped[z])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",z);
			exit(3);
		}
	}

	printf("SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE %d FIGLI CHE GENERERANNO A LORO VOLTA UN PROPRIO NIPOTE\n",getpid(),Z);

	//genereo gli Z figli
	for(z=0;z<Z;++z){
		if((pid=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",z);
			exit(4);
		}
		if(pid==0){
			//SONO DENTRO AL FIGLIO
			
			//chiudo i lati delle pipe che non utilizzo (in questo caso per i figli dato che scrivono sulla pipe k-esima andranno chiusi i lati di lettura)
			for(k=0;k<Z;++k){
				close(piped[k][0]);
				if(k!=z){
					close(piped[k][1]);
				}
			}

			//provo ad aprire il file corrispondente per vedere se il file esiste o comunque si possa leggere e nel caso non si possa ritorno un errore
			if((fd=open(argv[z+1],O_RDONLY))<0){
				printf("ERRORE NELL'APERTURA DEL FILE %s\n",argv[z+1]);
				exit(-1);
			}

			//creo la pipe per il proprio nipote
			pipe_t p;
			if(pipe(p)<0){
				printf("ERRORE NELLA CREAZIONE DELLA PIPE PER IL NIPOTE\n");
				exit(-1);
			}


			//passo alla creazione del nipote
			if((pid1=fork())<0){
				printf("ERRORE NELLA CREAZIONE DEL NIPOTE %d\n",z);
				exit(-1);
			}
			if(pid1==0){
				//SONO DENTRO AL NIPOTE

				//chiudo i lati della pipe che non utilizzo e chiudo lo stdout, duplico la pipe del nipote in scrittura e chiudo la pipe stessa in scrittura
				//in questo modo posso ridirigere lo stdout del nipote sulla pipe
				close(p[0]);
				close(1);
				dup(p[1]);
				close(p[1]);
				//eseguo il comando unix rev
				execl("/usr/bin/rev","rev",argv[z+1],(char*)0);
				printf("ERRORE NELL'EXEC");
				exit(-1);
			}
			//sono di nuovo nel figlio
			//chiudo i lati della pipe che non utilizzo
			close(p[1]);
			//comincio la lettura di ciò che mi manda il nipote
			j=0;
			lunghezza=0;
			while((nr=read(p[0],&c,1))!=0){
				if(c=='\n'){
					Buff[j]=c;
					++lunghezza;
					//metto questa lunghezza dentro alla mia struttura
					strutt.c3=lunghezza;
					//copio la linea trovata nel mio array di char della struttura
					for(k=0;k<=j;++k){
						strutt.c2[k]=Buff[k];
					}
					lunghezza=0;
					j=0;
				}
				else{
					Buff[j]=c;
					++j;
					++lunghezza;
				}
			}

			//aspetto il nipote
			if((pidNipote=wait(&status1))<0){
				printf("ERRORE NELLA WAIT DEL FIGLIO\n");
				exit(5);
			}
			if((status1 & 0XFF)!=0){
				printf("ERRORE, IL NIPOTE SI E' CHIUSO IN MODO ANOMALO\n");

			}
			else{
				ritorno1=(int)((status1 >> 8) & 0xFF);
				printf("IL NIPOTE CON PID %d HA RITORNATO %d\n",pidNipote,ritorno1);
			}
			//metto il valore del pid del nipote nell'apposito campo
			strutt.c1=pidNipote;

			//comunico al padre la struttura dati elaborata
			nw=write(piped[z][1],&strutt,sizeof(ST));
			if(nw!=sizeof(ST)){
				printf("ERRORE NELLA WRITE DEL FIGLIO\n");
				exit(-1);
			}
			exit(strutt.c3-1);
		}
	}

	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<Z;++k){
		close(piped[k][1]);
	}

	//leggo i valori che mi manda ciascun figlio
	for(z=0;z<Z;++z){
		//leggo la singola struttura mandata dal figlio z-esimo
		nr=read(piped[z][0],&strutt,sizeof(ST));
		if(nr!=sizeof(ST)){
			//in caso in cui uno o più file passati non sono validi quindi non leggo correttamente dalla pipe del padre passo all'iterazione successiva per non rovinare lo stdout ed evitare
			//di interrompere il processo di lettura del padre. L'ERRORE VIENE COMUNQUE SEGNALATO ALL'UTENTE CON LA SEGUENTE PRINTF.
			printf("ERRORE NELLA READ DEL PADRE AL FIGLIO DI INDICE: %d\n",z);
			continue;
		}
		//rendo la linea mandata sul campo c2 una stringa come richiesto dal testo del problema. Questo rende la linea stampabile direttamente con l'opzione %s
		strutt.c2[strutt.c3]='\0';
		//stampo il risultato. Per rendere più leggibile su stdout il risultato i campi delle strutture sono separati da un '\n'
		printf("IL FIGLIO CON INDICE %d RELATIVO AL FILE %s HA RESTITUITO COME STRUTTURA DATI :\nC1 : %d\nC2: %sC3 : %d\n\n",z,argv[z+1],strutt.c1,strutt.c2,strutt.c3);
	}

	//aspetto tutti i figli
	for( z=0;z<Z;++z){
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
