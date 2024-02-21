#!/bin/sh
echo QUESTA E\' LA PROVA DEL 3 GIUGNO 2020
echo FILIPPO LEONELLI `date`

#controllo sul numero di parametri passati
if test $# -lt 3
then
	echo NUMERO DI PARAMETRI PASSATI NOT OK
	exit 1
fi

#controllo sul tipo dei parametri
#controllo sul primo parametro
C=$1
case $C in
	?) echo PARAMETRO 1 OK
       ;;
     *) echo PARAMETRO 1 NOT OK
        exit 2
        ;;
esac

#controllo sul resto dei parametri
shift
count=2
for i in $*
do
	case $i in
		/*) if test -d $i -a -x $i
            then
            	echo PARAMETRO $count OK
            else
            	echo PARAMETRO $count NOT OK
            	exit 3
            fi
            ;;
        *) echo PARAMETRO $count NOT OK
           exit 4
           ;;
    esac
count=`expr $count + 1`
done

#impostazioni per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/nomiAssoluti

#chiamata al file ricorsivo
for i in $*
do
	FCR.sh $i $C /tmp/nomiAssoluti

done

#comandi post chiamata al file ricorsivo

echo NUMERO DI DIRECTORY CHE SODDISFANO I VINCOLI : `wc -l < /tmp/nomiAssoluti`
count=1 #variabile per contare le directory nel for
answer= #variabile per contenere la risposta dell'utente
for i in `cat /tmp/nomiAssoluti`
do
    echo DIRECTORY $count : $i
    echo -n ELENA VUOLE VISUALIZZARE IL CONTENUTO DELLA SEGUENTE DIRECTORY \?
    read answer
    case $answer in
    	S*|s*|y*|Y*) echo IL CONTENUTO DELLA DIRECTORY $i E\' :
                     ls -a $i
                     ;;
               *) echo OK NON MOSTRERO\' IL CONTENUTO DELLA DIRECTORY $i
                  ;;
    esac

count=`expr $count + 1`
done

rm /tmp/nomiAssoluti

echo HO FINITO TUTTO
