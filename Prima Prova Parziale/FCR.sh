#!/bin/sh

cd $1 #entro nella directory passata come primo parametro

#ciclo su tutti i file presenti in tale gerarchia
for F in *
do
	#controllo che sia un file e che sia leggibile
	if test -f $F -a -r $F
	then
		#verifico che nel nome del file ci siano le estensioni richieste dal problema
		#se ci sono le estensioni scrivo i file sul file temporaneo corrente
		case $F in
			*\.$2) echo HO TROVATO UN FILE NELLA GERARCHIA CORRENTE CHE TERMINA CON ESTENSIONE \.$2
                 echo `pwd`/$F >> $4
                 ;;
             *\.$3) echo HO TROVATO UN FILE NELLA GERARCHIA CORRENTE CHE TERMINA CON ESTENSIONE \.$3
                  echo `pwd`/$F >> $5
                 ;;
               *);;
        esac
    fi

done

#faccio la chiamata ricorsiva per ogni directory presente nella gerarchia
for i in *
do
	#controllo che i sia una directory e che sia attraversabile
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4 $5
	fi
done
