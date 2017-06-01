#! /usr/bin/env bash
## USE THIS ON A MAC!

mkdir -p allfigs


for f in \
    R0.4_Fig1 \
	BkgFigure \
	R0.2_Fig \
	ResAj_R0.4_LeadProfile \
	ResAj_R0.4_SubLeadProfile \
	ResAj_R0.2_LeadProfile \
	ResAj_R0.2_SubLeadProfile \
	tdpt \
	tdptlo \
	R0.2_tdpt \
	R0.2_tdptlo   
do
    if [ -f plots/$f.pdf ]; then
	echo Copying $f.pdf
	cp plots/$f.pdf allfigs/$f.pdf
	pdff=plots/$f.pdf
	echo $pdff
	
	out=`echo $pdff |sed s/pdf/png/g | sed s/plots/allfigs/g`
	echo Converting' ' $pdff 
	echo to'         ' $out
	sips -s format png $pdff --out $out > /dev/null
    elif [ -f plots/$f.png ]; then
	echo Copying $f.png
	cp plots/$f.png allfigs/$f.png
    else
	echo Could not find plots/$f.pdf or plots/$f.png
    fi
    echo ===
done






