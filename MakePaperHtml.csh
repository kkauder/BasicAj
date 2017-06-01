#!/usr/bin/env csh

rm -f "ShortAjData.html"

cat <<EOF1 >ShortAjData.html
<!DOCTYPE html>
<html>
  <head>
    <title>Di-jet imbalance measurements in Au+Au and pp collisions at &radic; s<sub>NN</sub>=200 GeV at STAR</title>
  </head>

<body>
	  <h1>Di-jet imbalance measurements in Au+Au and pp collisions at &radic; s<sub>NN</sub>=200 GeV at STAR</h1>
	  <h2>Phys. Rev. Lett. </h2>
<hr>
EOF1

echo '' >> ShortAjData.html
echo '<h1><a href="R0.4_Fig1.png">Figure 1</a></h1>' >> ShortAjData.html
cat R0.4_Fig.html >> ShortAjData.html
echo '<br><hr>' >> ShortAjData.html

echo '' >> ShortAjData.html
echo '<h1><a href="BkgFigure.png">Figure 2</a></h1>' >> ShortAjData.html
cat BkgFigure.html >> ShortAjData.html
echo '<br><hr>' >> ShortAjData.html

echo '' >> ShortAjData.html
echo '<h1><a href="R0.2_Fig.png">Figure 3</a></h1>' >> ShortAjData.html
cat R0.2_Fig.html >> ShortAjData.html
echo '<br><hr>' >> ShortAjData.html

## Supplement starts here

echo '<br><hr>' >> ShortAjData.html
echo '' >> ShortAjData.html
echo '<h1><a href="ResAj_R0.4_LeadProfile.png">Supplement - Figure 1, R=0.4, Leading</a></h1>' >> ShortAjData.html
echo '<h1><a href="ResAj_R0.4_SubLeadProfile.png">Supplement - Figure 1, R=0.4, Sub-Leading</a></h1>' >> ShortAjData.html
cat R0.4_Res.html >> ShortAjData.html
echo '<br><hr>' >> ShortAjData.html

echo '' >> ShortAjData.html
echo '<h1><a href="ResAj_R0.2_LeadProfile.png">Supplement - Figure 1, R=0.2, Leading</a></h1>' >> ShortAjData.html
echo '<h1><a href="ResAj_R0.2_SubLeadProfile.png">Supplement - Figure 1, R=0.2, Sub-Leading</a></h1>' >> ShortAjData.html
cat R0.2_Res.html >> ShortAjData.html
echo '<br><hr>' >> ShortAjData.html

# The rest we have to compile by hand

echo '' >> ShortAjData.html
echo '<h1><a href="tdpt.png">Supplement - Figure 2a)</a></h1>' >> ShortAjData.html
echo '<h1><a href="R0.2_tdpt.png">Supplement - Figure 2c)</a></h1>' >> ShortAjData.html
cat <<EOF2 >>ShortAjData.html
<table border="1" cellpadding="5">
	<tr>
		<th colspan=5>p<sub>T</sub><sup>Cut</sup>&gt;2 GeV/<i>c</i></th>
	</tr>
	<tr>
		<th></th>
		<th colspan=2>R=0.4</th>
		<th colspan=2>R=0.2</th>
	</tr>
	<tr>
		<th>&delta;p<sub>T</sub> [GeV/<i>c</i>]</th>
		<th>dN / d&delta;p<sub>T</sub> [(GeV/<i>c</i>)<sup>-1</sup>]</th>
		<th>stat. err.</th>
		<th>dN / d&delta;p<sub>T</sub> [(GeV/<i>c</i>)<sup>-1</sup>]</th>
		<th>stat. err.</th>
	</tr>
EOF2

foreach line4 ( `cat R0.4_dpt.csv` )
    echo '	<tr>' >>ShortAjData.html
    set pt4=`echo "$line4" | cut -d',' -f 1`
    set v4=`echo "$line4" | cut -d',' -f 2`
    set e4=`echo "$line4" | cut -d',' -f 3`

    #grep ^${pt4} R0.2_dpt.csv
    
    set line2 = `grep "^${pt4}" R0.2_dpt.csv`
    set pt2=`echo "$line2" | cut -d',' -f 1`
    set v2=`echo "$line2" | cut -d',' -f 2`
    set e2=`echo "$line2" | cut -d',' -f 3`
    
    echo '		<td>'${pt4}'</td>' >>ShortAjData.html
    echo '		<td>'${v4}'</td>' >>ShortAjData.html
    echo '		<td>'${e4}'</td>' >>ShortAjData.html

    echo '		<td>'${v2}'</td>' >>ShortAjData.html
    echo '		<td>'${e2}'</td>' >>ShortAjData.html

    echo '	</tr>' >>ShortAjData.html
end

echo '</table>' >>ShortAjData.html
echo '<br><hr>' >> ShortAjData.html


echo '' >> ShortAjData.html
echo '<h1><a href="tdptlo.png">Supplement - Figure 2b)</a></h1>' >> ShortAjData.html
echo '<h1><a href="R0.2_tdptlo.png">Supplement - Figure 2d)</a></h1>' >> ShortAjData.html
cat <<EOF3 >>ShortAjData.html
<table border="1" cellpadding="5">
	<tr>
		<th colspan=5>p<sub>T</sub><sup>Cut</sup>&gt;0.2 GeV/<i>c</i>, matched</th>
	</tr>
	<tr>
		<th></th>
		<th colspan=2>R=0.4</th>
		<th colspan=2>R=0.2</th>
	</tr>
	<tr>
		<th>&delta;p<sub>T</sub> [GeV/<i>c</i>]</th>
		<th>dN / d&delta;p<sub>T</sub> [(GeV/<i>c</i>)<sup>-1</sup>]</th>
		<th>stat. err.</th>
		<th>dN / d&delta;p<sub>T</sub> [(GeV/<i>c</i>)<sup>-1</sup>]</th>
		<th>stat. err.</th>
	</tr>
EOF3

foreach line4 ( `cat R0.4_dptlo.csv` )
    echo '	<tr>' >>ShortAjData.html
    set pt4=`echo "$line4" | cut -d',' -f 1`
    set v4=`echo "$line4" | cut -d',' -f 2`
    set e4=`echo "$line4" | cut -d',' -f 3`
    
    set line2 = `grep "^${pt4}" R0.2_dptlo.csv`
    set pt2=`echo "$line2" | cut -d',' -f 1`
    set v2=`echo "$line2" | cut -d',' -f 2`
    set e2=`echo "$line2" | cut -d',' -f 3`
    
    echo '		<td>'${pt4}'</td>' >>ShortAjData.html
    echo '		<td>'${v4}'</td>' >>ShortAjData.html
    echo '		<td>'${e4}'</td>' >>ShortAjData.html

    echo '		<td>'${v2}'</td>' >>ShortAjData.html
    echo '		<td>'${e2}'</td>' >>ShortAjData.html

    echo '	</tr>' >>ShortAjData.html
end

echo '</table>' >>ShortAjData.html
echo '<br><hr>' >> ShortAjData.html


echo '</body>' >> ShortAjData.html
echo '</html>' >> ShortAjData.html

cp ShortAjData.html allfigs/

