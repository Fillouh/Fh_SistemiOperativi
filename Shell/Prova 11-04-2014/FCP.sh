#!/bin/sh
echo QUESTA E\' LA PROVA DEL 11 APRILE 2014
echo FILIPPO LEONELLI `date`

#controllo del numero dei parametri
if test $# -lt 2
	then echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
         exit 1
fi

count=1 #variabile che mi serve per contare nella gerarchia
#impostazione per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/file

#chiamata al file ricorsivo per ogni gerachia dopo i controlli
for G in $*
do
	case $G in
		/*) if test -d $G -a -x $G
            then 
            	echo PARAMETRO $count OK
            	echo POSSO ESPLORARE LA GERARCHIA $G :
            else
            	echo PARAMETRO $count NOT OK
            	exit 2
            fi
            ;;
        *) echo PARAMETRO $count NOT OK
           exit 3
           ;;
    esac
FCR.sh $G /tmp/file
count=`expr $count + 1`
done

NFTOT=`wc -l < /tmp/file`
echo IL NUMERO DI FILE TOTALI DI TUTTE LE GERARCHIE E\' : $NFTOT
rm /tmp/file

#fase 2
c=1
for G in $*
do
	case $G in
		/*) if test -d $G -a -x $G
            then 
            	echo PARAMETRO $c OK
            	echo POSSO ESPLORARE LA GERARCHIA $G :
            else
            	echo PARAMETRO $c NOT OK
            	exit 4
            fi
            ;;
        *) echo PARAMETRO $c NOT OK
           exit 5
           ;;
    esac

>/tmp/file
FCR.sh $G /tmp/file
nl=`wc -l < /tmp/file`
echo DIRECTORY CORRENTE : $G
echo NUMERO DI FILE TROVATI PER LA DIRECTORY CORRENTE : $nl
X= #variabile che mi serve per leggere la risposta da stdin

for i in `cat /tmp/file`
do
	echo FILE : $i
	echo DIMMI UN NUMERO
	read X
	case $X in
		[0-9]) echo MI HAI PASSATO REALMENTE UN NUMERO, BRAVO
               ;;
           *) echo NON MI HAI PASSATO UN NUMERO, CATTIVO
              continue
              ;;
    esac
    echo LE PRIME RIGHE DEL FILE $i SONO :
    head -$X $i
done

c=`expr $c + 1`
rm /tmp/file
done

echo HO FINITO TUTTO
