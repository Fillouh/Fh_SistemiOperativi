#!/bin/sh
cd $1
nfile=0
for i in *
do
	if test -f $i
		then nl=`wc -c <$i`
	    if test $nl -eq $3
	    then 
	    	 case `cat $i` in
	    	 	[0-9]) nfile=`expr $nfile + 1 `
                       ;;
                   *) ;;
              esac
        fi
    fi
done

if test $nfile -ge $2
then 
	echo TROVATA UNA DIRECTORY CHE SODDISFA I VINCOLI : `pwd`
	echo `pwd` >> $4
fi

for i in *
do 
	if test -d $i -a -x $i
	then 
		FCR.sh `pwd`/$i $2 $3 $4
    fi
done
