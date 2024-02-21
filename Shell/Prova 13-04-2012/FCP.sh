#!/bin/sh
echo QUESTA E\' LA PROVA DEL 13 APRILE 2012
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 1
	then echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
         exit 1
fi

#controllo sul tipo di parametri
G=$1
case $G in
	/*) if test -d $G -a -x $G
        then
        	echo PARAMETRO 1 OK
        else
        	echo PARAMETRO 1 NOT OK
        	exit 2
        fi
        
       	;;
     *)echo PARAMETRO 1 NOT OK
        	exit 3
        	;;
esac

#impostazione delle chiamate ricorsive
PATH=`pwd`:$PATH
export PATH
>/tmp/nlivelli
echo 0 > /tmp/nlivelli #imposto a zero i livelli

#esecuzione della fase A
echo ENTRO NELLA FASE A
FCR.sh $G A /tmp/nlivelli
nliv=`cat /tmp/nlivelli` #variabile che mi rappresenta i livelli trovati nella gerarchia
echo NUMERO DI LIVELLI DELLA GERARCHIA PASSATA : $nliv
> /tmp/nlivelli

#fase B
num= #variabile a cui assegno la risposta dell'utente
echo SCRIVI UN NUMERO :
read num
if test $num -ge 1 -a $num -le $nliv
then 
	echo ENTRO NELLA FASE B :
	FCR.sh $G B /tmp/nlivelli $num
else echo $num NON E\' COMPRESO TRA 1 E $nliv, NON POSSO CHIAMARE LA FASE B
fi


rm /tmp/nlivelli
echo HO FINITO TUTTO
