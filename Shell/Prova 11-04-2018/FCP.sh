#!/bin/sh
echo QUESTA E\' LA PROVA DEL 11 APRILE 2018
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -lt 3
then
echo NUMERO DEI PARAMETRI NON CORRETTO
exit 1
fi 

#controllo sul tipo dei parametri
Y=$1
test $Y -ge 0 >/dev/null 2>&1
case $? in
0) echo PARAMETRO 1 OK
;;
*) echo PARAMETRO 1 NOT OK
;;
esac

#controllo dei restanti parametri 
shift
count=2
for G in $*
do
    case $G in
    	/*) if test -d $G -a -x $G
            then echo PARAMETRO $count OK
        else
        	echo PARAMETRO $count NOT OK
        	exit 2
        fi
        ;;
        *) echo PARAMETRO $count NOT OK
        	exit 3
        ;;
    esac
count=`expr $count + 1`
done

#impostazione per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
>/tmp/filecreati

#chiamata ricorsiva
for i in $*
do 
	FCR.sh $i $Y /tmp/filecreati
	echo CONTENUTO /tmp/filecreati per G :
	cat /tmp/filecreati
done

#comandi post chiamata ricorsiva
echo I FILE CHE RISPETTANO I VINCOLI IN TOTALE SONO : `wc -l < /tmp/filecreati`
for i in `cat /tmp/filecreati`
do
	echo FILE : $i
	nl=`wc -l < $i`
	echo CONTENUTO DEL FILE $i :
	if test $nl -eq 0
	then
		echo IL FILE $i E\' VUOTO
	else 
	cat $i
    fi
done

rm /tmp/filecreati

echo HO FINITO TUTTO
