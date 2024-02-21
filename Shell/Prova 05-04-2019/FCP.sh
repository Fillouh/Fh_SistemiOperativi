#!/bin/sh
echo QUESTA E\' LA PROVA DEL 05 APRILE 2019
echo FILIPPO LEONELLI `date`

#controllo del numero dei parametri
if test $# -lt 3
then
	echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
    exit 1
fi

#controllo del tipo dei parametri
count=1
for i in $*
do
	if test $count -eq $#
	then
		case $i in
			*/*) echo PARAMETRO $count NOT OK
                 exit 2
                 ;;
              *) echo PARAMETRO $count OK
                ;;
        esac

        S=$i

    else
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
               exit 3
               ;;
        esac
    fi
count=`expr $count + 1`
done

#impostazioni per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/file

count=1
for i in $*
do
	if test $count -eq $#
	then
		break
	fi
	FCR.sh $i $S /tmp/file
done

#comandi post chiamata al file ricorsivo
echo IL NUMERO TOTALE DI FILE CHE SODDISFA I VINCOLI E\' : `wc -l < /tmp/file`
echo MOSTRO IL CONTENUTO del file temp:
cat /tmp/file
vv=0
for i in `cat /tmp/file`
do
    if test `expr $vv % 2` -eq 0
    then
	echo CARATTERI FILE:
    echo $i
    else
        echo NOME FILE :
        echo $i
	    var=
	    answer=
        echo VUOI ORDINARE IL FILE \? :
        read answer
        case $answer in
    	         S* | s* |y*|Y*) #ordino il file
                        echo FILE ORDINATO :
                        sort -f < $i
                        ;;
                    *) echo OK ALLORA NON ORDINO IL FILE
                       ;;
        esac
    fi
    vv=`expr $vv + 1`
done

rm /tmp/file

echo HO FINITO TUTTO
