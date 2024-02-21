#!/bin/sh
echo QUESTA E\' LA PROVA DEL 7 APRILE 2017
echo FILIPPO LEONELLI `date`

#controllo sul numero di parametri passati
if test $# -lt 3
then
	 echo NUMERO DI PARAMETRI PASSATI NON CORRETTO
	 exit 1
fi

#controllo sul tipo dei parametri
#controllo sul primo parametro
F=$1
case $F in
	/*)echo PARAMETRO 1 NOT OK
       exit 2
       ;;
    */*) echo PARAMETRO 1 NOT OK
         exit 3
         ;;
     *) echo PARAMETRO 1 OK
        ;;
esac

#controllo sul resto dei parametri
shift
count=2
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
           exit 5
           ;;
    esac
count=`expr $count + 1`
done

#impostazioni per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/file

#chiamata al file ricorsivo per ogni gerarchia passata
for i in $*
do
	FCR.sh $i $F /tmp/file
done

#comandi post chiamata al file ricorsivo
echo NUMERO DI FILE CREATI GLOBALMENTE : `wc -l < /tmp/file`
for i in `cat /tmp/file`
do
	echo FILE : $i
	echo PRIMA LINEA : `head -1 $i`
	echo ULTIMA INEA : `tail -1 $i`
done

rm /tmp/file

echo HO FINITO TUTTO
