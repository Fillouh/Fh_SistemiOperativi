#!/bin/sh

cd $1

for i in *
do 
	if test -f $i
	then 
		nl=`wc -l <$i`
		if test $nl -gt $2
		then 
			echo `pwd`/$i >> $3
			echo FILE CON LINEE MAGGIORI DI $2 : `pwd`/$i
		fi
	fi

done

for i in *
do 
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
