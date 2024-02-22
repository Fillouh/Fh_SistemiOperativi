//FINITO F
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

/*int finitof()
{
/* questa funzione verifica i valori memorizzati nell'array finito: appena trova un elemento uguale a 0 vuole dire che non tutti i processi figli sono finiti e quindi torna 0; tornera' 1 se e solo se tutti gli elementi dell'array sono a 1 e quindi tutti i processi sono finiti */
  /*int i;
	for (i=0; i < N; i++)
		if (!finito[i])
		/* appena ne trova uno che non ha finito */
			/*return 0; /* ritorna falso */
 	/*return 1;
} */

int main(int argc,char **argv){

	int N=argc-2;
	int fd;
	int pid,pidFiglio,ritorno,status;
	int i,k;
	char c[2];
	int nr;
	int invio;

	//controllo sul numero dei parametri
	if(argc<4){
		printf("ERRORE,NUMERO DI PARAMETRI PASSATI NON CORRETTO!\n");
		exit(1);
	}

	//apro l'ultimo file
	if((fd=open(argv[argc-1],O_WRONLY))<0){
		printf("ERRORE NELL'APERTURA DELL'ULTIMO FILE\n");
		exit(2);
	}

	//allocazione per le n pipe 
	pipe_t *piped=malloc(N*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<N;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
	}

	//genero gli n figli
	for(i=0;i<N;++i){
		if((pid=fork())<0){
			exit(5);
		}
		if(pid==0){
			//sono nel figlio
			//chiudo le pipe che non utilizzo
			for(k=0;k<N;++k){
				close(piped[k][0]);
				if(k!=i){
					close(piped[k][1]);
				}
			}

			//apro il file
			if((fd=open(argv[i+1],O_RDONLY))<0){
				exit(-1);
			}

			invio=0;
			while((nr=read(fd,c,2))!=0){
				write(piped[i][1],c,2);
				invio+=nr;
			}
			exit(invio);
		}
	}
	//sono nel padre
	//chiudo i lati delle pipe che non utilizzo
	for(k=0;k<N;++k){
		close(piped[k][1]);
	}

	//leggo i valori dai figli

/*	while(!finitof())
{
	for (n=0; n < N; n++)
	{
		//tentiamo di leggere i caratteri dal figlio i-esimo: contestualmente viene aggiornato il valore del corrispondente elemento dell'array finito 
		nr=read(piped[n][0], chs, 2);  
		finito[n] = (nr == 0);  
		/* nel caso la read torni un valore 0, il corrispondente valore di finito viene settato a 1 dato che la condizione e' vera! 
		if (!finito[n])
	        {
			write(fdw, chs, nr);
			// il padre scrive sul file: ATTEZNIONE SI DEVE USARE nr e NON 2 
		}
	}
}*/
	for(i=0;i<N;++i){
		nr=read(piped[i][0],c,2);
		lseek(fd,0,SEEK_END);
		write(fd,c,2);
	}

	//aspetto tutti i figli
	for (i=0; i < N; i++){
        pidFiglio = wait(&status);
        if (pidFiglio < 0)
        {
                printf("Errore in wait\n");
                exit(8);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        { 
		ritorno=(int)((status >> 8) & 0xFF);
        	printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
        } 
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
