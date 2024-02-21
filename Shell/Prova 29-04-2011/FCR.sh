#!/bin/sh

cd $1
nf=$3
export nf
case $4 in 
	0) 
for i in *
do 
	if test -f $i
		then if test $i=$2
		     then
		     	echo A: LA CARTELLA CHE SODDISFA I VINCOLI E\' : `pwd` 
		          nf=`expr $nf + 1`
		 fi
    fi
done


for i in *
do 
	if test -d $i -a -x $i
	then 
		FCR.sh `pwd`/$i $2 $nf $4
    fi
done

;;
   

   1) 
     for i in *
do 
	if test -f $i
		then if test $i != $2
		     then echo B : LA CARTELLA CHE SODDISFA I VINCOLI E\' : `pwd` 
		          > $2
		 fi
    fi
done
 FCR.sh `pwd` $2 $nf 0


;;

esac
