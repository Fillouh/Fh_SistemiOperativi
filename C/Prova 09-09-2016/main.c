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
	char v1;
	long int v2;
} str;

void scambia(str *a,str *b){
	str tmp=*a;
	*a=*b;
	*b=tmp;
}

void bubbleSort(str v[], int dim)
{ int i; bool ordinato = false;
while (dim>1 && !ordinato)
{ ordinato = true; /* hp: è ordinato */
 for (i=0; i<dim-1; i++)
 if (v[i].v2>v[i+1].v2)
 {
scambia(&v[i],&v[i+1]);
ordinato = false;
 }
dim--;
}
} 


int main(int argc,char **argv){

	//controllo sul numero dei parametri
	if(argc!=2){
		printf("ERRORE, NUMERO DEI PARAMETRI PASSATI NON CORRETTO\n");
		exit(1);
	}

	int pidFiglio,ritorno,status;
	int i,k;
	int fd;
	char c;
	str arr[26];
	long int occ=0;
	int *pid=calloc(26,sizeof(int));

	//allocazione e creazione delle N pipe per pipeline
	pipe_t *piped=malloc(26*sizeof(pipe_t));
	if(piped==NULL){
		printf("ERRORE NELLA ALLOCAZIONE DI MEMORIA\n");
		exit(3);
	}

	for(i=0;i<26;++i){
		if(pipe(piped[i])<0){
			printf("ERRORE NELLA CREAZIONE DELLA PIPE %d\n",i+1);
			exit(4);
		}
	}
	//LO SCHEMA DI LETTURA SU QUESTA PIPE PREVEDE CHE IO LEGGA DALLA PIPE i-1

	printf("DEBUG-SONO IL PROCESSO PADRE CON PID %d E STO PER GENERARE 26 FIGLI\n",getpid());

	//genero i 26 figli
	for(i=0;i<26;++i){
		if((pid[i]=fork())<0){
			printf("ERRORE NELLA CREAZIONE DEL FIGLIO %d\n",i+1);
			exit(5);
		}
		if(pid[i]==0){
			//sono nel figlio

			//chiudo le pipe che non utilizzo
			for(k=0;k<26;++k){
				if(k!=i){
					close(piped[k][1]);
				}
				if((i==0)||(k!=i-1)){
					close(piped[k][0]);
				}
			}

			//provo ad aprire il file in ogni file
			if((fd=open(argv[1],O_RDONLY))<0){
				printf("ERRORE DI APERTURA DEL FILE NEL FIGLIO %d\n",i+1);
				exit(-1);
			}

			//leggo dal file per ogni figlio
			occ=0;
			while(read(fd,&c,1)!=0){
				if(c=='a'+i){
						++occ;
				}
			}
				
			if(i!=0){
				//leggo l'array dalla pipe precedente
				read(piped[i-1][0],arr,sizeof(str) * 26);
					
			}
			char C='a'+i;
			arr[i].v1=C;
			arr[i].v2=occ;
			//comunico il risultato al figlio successivo o al padre
			write(piped[i][1],arr,sizeof(str) * 26);
			exit(c);
			
		}
	}

	//SONO NEL PADRE
	//chiudo le pipe che non utilizzo
	for(k=0;k<26;++k){
		if(k!=26-1){
			close(piped[k][0]);
		}
		close(piped[k][1]);
	}

	//leggo i valori dall'ultima pipe
	read(piped[25][0],arr,sizeof(str) * 26);

	//ordino i valori
	bubbleSort(arr,26);

	//mostro i risultati
	for(i=0;i<26;++i){
		printf("IL PROCESSO DI INDICE %d CON PID %d HA CONFEZIONATO LA STRUTTURA : V1 : %c, V2 : %ld \n",i,pid[i],arr[i].v1,arr[i].v2);
	}

	//aspetto tutti i figli
	for( i=0;i<26;++i){
		if((pidFiglio=wait(&status))<0){
			printf("ERRORE NELLA WAIT DEL PADRE\n");
			exit(7);
		}
		if((status & 0XFF)!=0){
				printf("ERRORE, IL FIGLIO SI E' CHIUSO IN MODO ANOMALO\n");

		}
		else{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("IL FIGLIO CON PID %d HA RITORNATO %d COME CARATTERE ASCII E %c COME CARATTERE SINGOLO\n",pidFiglio,ritorno,ritorno);
		}
	}

	printf("HO FINITO TUTTO\n");
	exit(0);
}
