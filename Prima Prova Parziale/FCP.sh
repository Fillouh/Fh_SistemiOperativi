#!/bin/sh
echo QUESTA E\' LA PROVA DEL 12 APRILE 2023
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -lt 4
then
	echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo sul tipo dei parametri

S1=$1 #assegno alla variabile S1 il valore del primo parametro
S2=$2 #assegno alal variabile S2 il valore del secondo parametro

#controllo sul primo parametro
#per verificare che sia solo una stringa vorrei evitare che sia il nome relativo di qualche directory
case $S1 in
	*/*) echo PARAMETRO 1 NOT OK
         exit 2
         ;;
     *) echo PARAMETRO 1 OK
        ;;
esac

#controllo sul secondo parametro
#per verificare che sia solo una stringa vorrei evitare che sia il nome relativo di qualche directory
case $S2 in
	*/*) echo PARAMETRO 2 NOT OK
         exit 3
         ;;
     *) echo PARAMETRO 2 OK
        ;;
esac

#controllo sul resto dei parametri
shift #mi sposto i parametri
shift #mi sposto i parametri
count=3 #variabile inizializzata per avere un conteggio all'interno del for e quindi fare un echo più umano
for G in $*
do
	case $G in
		/*) if test -d $G -a -x $G
            then
            	echo PARAMETRO $count OK
            else echo PARAMETRO $count NOT OK
            	 exit 4
            fi
            ;;
        *) echo PARAMETRO $count NOT OK
           exit 4
           ;;
    esac
count=`expr $count + 1`
done

#impostazione per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/nomiAssoluti-1 #creo un file temporaneo per contenere i file con estensione S1
>/tmp/nomiAssoluti-2 #creo un file temporaneo per contenere i file con estensione S2

#chiamata al file ricorsivo per ogni gerachia passata
for G in $*
do
	FCR.sh $G $S1 $S2 /tmp/nomiAssoluti-1 /tmp/nomiAssoluti-2

done

#comandi post chiamata al file ricorsivo

#ora conto tutti i file per le varie estensioni richieste
TOT1=`wc -l < /tmp/nomiAssoluti-1` #variabile per contare i file con l'estensione S1
TOT2=`wc -l < /tmp/nomiAssoluti-2` #variabile per contare i file con l'estensione S2

echo IL NUMERO DI FILE TROVATI CON ESTENSIONE $S1 E\' : $TOT1
echo IL NUMERO DI FILE TROVATI CON ESTENSIONE $S2 E\' : $TOT2
answer= #variabile per contenere il nome in risposta dell'utente alla richiesta del suo nome
nome= #variabile per contenere il primo nome passato dall'utente
X= #variabile per contenere il numero passato dall'utente
if test $TOT1 -le $TOT2
then
    echo MI DIRESTI IL TUO NOME\?
    read answer
    #questo for ha lo scopo di selezionare solo il primo input dell'utente
    for i in $answer
    do
    	nome=$i
    	break
    done
    echo $nome MI DIRESTI UN NUMERO COMPRESO TRA 1 E $TOT1 \?
    read X
    #verifico che l'input che mi passa l'utente sia effettivamente un numero valido
    #se è così allora posso mostrare i file corrispondenti a tale numero
    test $X -ge 1 -a $X -le $TOT1 >/dev/null 2>&1
    case $? in
    	 0) echo MI HAI PASSATO UN NUMERO VALIDO
            echo POSSO MOSTRARTI I FILE ALLA RIGA $X
            echo FILE CON ESTENSIONE $S1 A RIGA $X : `head -$X /tmp/nomiAssoluti-1 | tail -1`
            echo FILE CON ESTENSIONE $S2 A RIGA $X : `head -$X /tmp/nomiAssoluti-2 | tail -1`
            ;;
         *) echo NON MI HAI PASSATO UN NUMERO VALIDO
            echo NON POSSO MOSTRARTI I FILE ALLA RIGA $X
            ;;
    esac
else
	echo $TOT1 E\' MAGGIORE DI $TOT2
	echo NON POSSO MOSTRARTI NULLA
fi

#rimuovo tutti i file temporanei
rm /tmp/nomiAssoluti-1
rm /tmp/nomiAssoluti-2

#dichiaro la fine del programma
echo HO FINITO TUTTO
