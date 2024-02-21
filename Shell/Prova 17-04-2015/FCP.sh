#!/bin/sh
echo QUESTA E\' LA PROVA DEL 17 APRILE DEL 2015
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -lt 3
then
	echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo sul tipo dei parametri
X=$1
case $X in
	[0-9]) echo PARAMETRO 1 OK
          ;;
      *)echo PARAMETRO 1 NOT OK
        exit 2
        ;;
esac

shift
cp=2 #variabile conta parametri
for G in $*
do
	case $G in
		/*) if test -d $G -a -x $G
            then
            	echo PARAMETRO $cp OK
            else echo PARAMETRO $cp NOT OK
            	 exit 3
            fi
            ;;
         *) echo PARAMETRO $cp NOT OK
            exit 4
            ;;
    esac
cp=`expr $cp + 1`
done

#impostazione per la chiamata al file ricorsivo
PATH=`pwd`:$PATH
export PATH
>/tmp/file

for i in $*
do
	FCR.sh $i $X /tmp/file
done

#comandi post chiamata al file ricorsivo
NFTOT=`wc -l </tmp/file`
echo IL NUMERO TOTALE DI FILE TROVATI E\' : $NFTOT
K= #variabile per memorizzare la risposta dell'utente

for i in `cat /tmp/file`
do
	echo FILE : $i
	echo DIMMI UN NUMERO :
	read K
    case $K in
    	[0-9]) if test $K -le $X 
               then echo QUESTO NUMERO VA BENE
           else echo QUESTO NUMERO NON VA BENE
           	   continue
           fi
           ;;
          *) echo NON MI HAI PASSATO UN NUMERO
             continue
             ;;
    esac

    echo LA $K-ESIMA RIGA DEL FILE $i E\' :
    head -$K $i | tail -1
done

rm /tmp/file

echo HO FINITO TUTTO
