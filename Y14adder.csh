#!/usr/bin/env csh

foreach group (`ls -1 -d Data/y14_AuAu_HT2/[0-9]*` )
    set group=`basename $group`
    set base = Data/y14_AuAu_HT2/${group}/AuAu14Pico_

    # Combine into larger chunks
    set combine=100
    set list = ${base}*

    @ total = ( $#list / $combine ) 

    foreach count ( `seq 0 $total ` )
	@ start = ( $count * $combine )
	@ end = ( $count * $combine + $combine - 1 )
	if ( $end >= $#list ) then
	    set end = $#list
	endif
	echo $count : $start -- $end
	
	set target = Data/HaddedAuAu14/AuAu14Pico_${group}_${count}.root
	set source = ""
	foreach filecounter ( `seq $start $end` )
	    set source = "$source $list[$filecounter]"
	end
	hadd -k -f $target $source
    end
end

