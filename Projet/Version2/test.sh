#!/bin/bash

x=100
bin/BAMV_LZW -c input/dicoAB.txt output/dicoAB_comp.txt
a=$(wc -c<input/dicoAB.txt)
b=$(wc -c<output/dicoAB_comp.txt)
echo""
echo "Pour le fichier dicoAB.txt :"
echo""
echo "Taille avant compression"
echo $a
echo "Taille apres compression"
echo $b