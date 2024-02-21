#!/bin/sh
#introduzione al programma

echo QUESTA E\' LA PROVA DEL 15 GENNAIO 2014
echo FILIPPO LEONELLI `date`

#controllo del numero dei parametri

if test $# -ne 3
then
	echo NUMERO DEI PARAMETRI PASSATO NON CORRETTO
	exit 1
fi

#assegnazione delle variabili richieste dal problema ai parametri

G1=$1
G2=$2
K=$3
#controllo sul tipo dei parametri

#controllo sul primo parametro

case $G1 in
	/*) if test -d $G1 -a -x $G1
        then
        	echo PARAMETRO 1 OK
        else
        	echo PARAMETRO 1 NOT OK
        	exit 2
        fi
        ;;
    *) echo PARAMETRO 1 NOT OK
        	exit 3
        	;;
esac

#controllo sul secondo parametro

case $G2 in
	/*) if test -d $G2 -a -x $G2
        then
        	echo PARAMETRO 2 OK
        else
        	echo PARAMETRO 2 NOT OK
        	exit 4
        fi
        ;;
    *) echo PARAMETRO 2 NOT OK
        	exit 5
        	;;
esac

#controllo sul terzo parametro

test $K -ge 0 >/dev/null 2>&1
case $? in
	0)echo PARAMETRO 3 OK
      ;;
     *)echo PARAMETRO 3 NOT OK
       exit 6
       ;;
esac

#impostazioni per la chiamata ricorsiva

PATH=`pwd`:$PATH
export PATH
> /tmp/file1_$$
> /tmp/file2_$$

#chiamata ricorsiva per ogni gerarchia
echo CARTELLE CHE SODDISFANO I VINCOLI NELLA G1 :
FCR.sh $G1 $K /tmp/file1_$$
echo CARTELLE CHE SODDISFANO I VINCOLI NELLA G2 :
FCR.sh $G2 $K /tmp/file2_$$

n1=0 #variabile per memorizzare il numero di file trovati in G1
n2=0 #variabile per memorizzare il numero di file trovati in G2

n1=`wc -l < /tmp/file1_$$`
n2=`wc -l < /tmp/file2_$$`

if test $n1 -eq $n2 -a $K
then
	echo POSSO CHIAMARE LA PARTE IN C :
	echo TUTTI I FILE IN G1 :
	cat /tmp/file1_$$
	echo TUTTI I FILE IN G2 :
	cat /tmp/file2_$$
else
	echo NON POSSO STAMPARE NULLA
fi

rm /tmp/file1_$$
rm /tmp/file2_$$

echo HO FINITO TUTTO
