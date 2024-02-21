#!/bin/sh
echo QUESTA E\' LA PROVA DEL 13 APRILE 2022
echo FILIPPO LEONELLI `date`

#controllo numero parametri
NPARAM=$#
if test $# -lt 3
then 
	echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo sul tipo dei parametri
case $1 in
	[0-9])echo PARAMETRO 1 OK
          ;;
       *) PARAMETRO 2 NOT OK
          exit 2
          ;;

esac

X=$1
shift
count=2
for i in $*
do 
	case $i in
		/*) if test -d $i -a -x $i
	        then echo PARAMETRO $count OK
	        else echo PARAMETRO $count NOT OK
	             exit 3
	        fi
	       ;;

	    *) echo PARAMETRO $count NOT OK
	       exit 4
	       ;;
	esac
done

PATH=`pwd`:$PATH
export PATH


count=1
#nr=0 #variabile per contare i file trovati nella gerarchia corrente
for i in $*
do
	>/tmp/nomiAssoluti-$count
	FCR.sh $X $i /tmp/nomiAssoluti-$count
	#nl=`wc -l < /tmp/nomiAssoluti-$count`
	#nr=`expr $nl - $nr`
	echo NUMERO TOTALE DI FILE TROVATI PER LA GERARCHIA `pwd`/$i : `wc -l </tmp/nomiAssoluti-$count`
	cat /tmp/nomiAssoluti-$count
	count=`expr $count + 1`
done

count=1
countv=2
	#statements
	while true
	do
		if test $countv -eq $NPARAM
		then
			break
	    fi
	for i in `cat /tmp/nomiAssoluti-$count`
    do 
  
   for o in `cat /tmp/nomiAssoluti-$countv`
   do 
   	   diff $i $o >/dev/null 2>&1
   	   if test $? -eq 0
   	   then
   	   	echo I FILE $i E $o SONO UGUALI
   	   else echo I FILE $i $o SONO DIVERSI
   	   fi
   	done
    done
    countv=`expr $countv + 1`
done



rm /tmp/nomiAssoluti*

echo HO FINITO TUTTO
