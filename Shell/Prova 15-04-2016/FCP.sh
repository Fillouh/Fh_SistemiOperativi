#!/bin/sh
echo QUESTA E\' LA PROVA DEL 15 APRILE 2016
echo FILIPPO LEONELLI `date`

#controllo il numero dei parametri
if test $# -lt 3
then
	echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
    exit 1
fi

#controllo sul tipo dei parametri
#controllo sui primi parametri
count=1
for G in $*
do
	if test $count -eq $#
	then
		#controllo sull'ultimo parametro
		X=$G
		test $X -ge 0 >/dev/null 2>&1
	case $? in
	0) echo PARAMETRO $count OK
       ;;
    *) echo PARAMETRO $count NOT OK
       exit 4
       ;;
	esac

	else
		case $G in
			/*) if test -d $G -a -x $G
                then
                	echo PARAMETRO $count OK
                else
                	echo PARAMETRO $count NOT OK
                	exit 2
                fi
               ;;
             *) echo PARAMETRO $count NOT OK
                exit 3
                ;;
        esac
    fi
count=`expr $count + 1`
done



#impostazione per la chiamata ricorsiva

PATH=`pwd`:$PATH
export PATH
>/tmp/dir

count=1
for i in $*
do
    if test $count -eq $#
    then
    	break
    else
    	FCR.sh $i $X /tmp/dir
    fi
count=`expr $count + 1`
done

#comandi post chiamata al file ricorsivo
echo CALCOLO LE LINEE
echo NUMERO DI DIRECTORY CHE SODDISFANO I VINCOLI : `wc -l < /tmp/dir`

for i in `cat /tmp/dir`
do
	echo DIRECTORY TROVATA : $i
	echo MI SPOSTO NELLA DIRECTORY CORRENTE
	cd $i
	for F in *
	do
		if test -f $F -a -r $F
		then
		echo FILE : $F ,
		echo -n LINEA RICHIESTA : `tail -$X $F`
	    fi
	done
done

rm /tmp/dir

echo HO FINITO TUTTO
