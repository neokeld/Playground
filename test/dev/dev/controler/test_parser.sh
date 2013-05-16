#!/bin/bash

nb_test_file=5
test_file[0]="test.gv"
test_file[1]="test_graphe_faux.gv"
test_file[2]="test_graphe_faux2.gv"
test_file[3]="test_graphe_faux3.gv"
test_file[4]="test_graphe_faux4.gv"

i=0
while [ $i -lt $nb_test_file ]
do
    cmd=${test_file[$i]}
    echo Test de \'$cmd\' :
    ./test_parser test_files/$cmd

    let i=i+1
done

