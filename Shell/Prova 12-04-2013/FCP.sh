#!/bin/sh
echo QUESTA E\' LA PROVA DEL 12 APRILE 2013
echo FILIPPO LEONELLI `date`

#controllo sul numero dei parametri
if test $# -ne 3
then
	echo NUMERO DEI PARAMETRI PASSATI NON CORRETTO
	exit 1
fi

#controllo sul tipo dei parametri
G=$1
S=$2
N=$3
#controllo sul primo parametro
case $G in
	/*) if test -d $G -a -x $G
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

#controllo sulla stringa non ha senso farlo
echo PARAMETRO 2 OK

#controllo sul terzo parametro
test $N -ge 0 >/dev/null 2>&1
case $? in
	0)echo PARAMETRO 3 OK
      ;;

    *)echo PARAMETRO 3 NOT OK
         	exit 4
         	;;
esac

#impostazione per la chiamata ricorsiva
PATH=`pwd`:$PATH
export PATH
>/tmp/dir

#chiamata al file ricorsivo
FCR.sh $G $S /tmp/dir

#post chiamata al file ricorsivo
ndir=`wc -l </tmp/dir`
echo IL NUMERO DI DIRECTORY CHE SODDISFANO LE CONDIZIONI SONO : $ndir
echo TUTTE LE DIRECTORY CHE SODDISFANO LE CONDIZIONI SONO :
cat /tmp/dir

num= #variabile per memorizzare il numero passato dall'utente
if test $ndir -gt $N
then
	echo SCRIVI UN NUMERO :
	read num
	if test $num -ge 1 -a $num -le $N
	then
		dire=`head -$num /tmp/dir | tail -1`
		echo LA DIRECTORY SELEZIONATA E\' : $dire
		cd $dire
		for i in *
		do
			if test -f $i -a -r $i
			then
				case $i in
					*\.$S) echo LA PRIMA RIGA DEL FILE `pwd`/$i : 
                           echo `head -1 $i`
                           ;;
                       *);;
                esac
            fi
        done
    else
    	echo $num NON E\' COMPRESO TRA 1 E $N
    fi
else
	echo IL NUMERO DI DIRECTORY TROVATE NON E\' MAGGIORE DI $N
fi

rm /tmp/dir

echo HO FINITO TUTTO  
