#set base = /data4/AuAu11picoNPE25_150526/AuAu11Pico_C2711C7EE0A85E03AAE22755201F9CE0
set base = /data4/AuAu11picoNPE25_150526/AuAu11Pico_D221F6DD142F2D996EB5927195514FE6

foreach a ( `seq 1 9` )

    set target = /data4/SmallAuAu11NPE25/AuAu11Pico_D_${a}00-${a}49.root
    set source = ""
    foreach b ( `seq 0 4` )
	foreach c ( `seq 0 9` )	    
	    set source = "$source ${base}_${a}${b}${c}.root"	   
	end
    end
    hadd -k -f $target $source >& logs/AuAu11Pico_D_${a}00-${a}49.out 

    set target = /data4/SmallAuAu11NPE25/AuAu11Pico_D_${a}50-${a}99.root
    set source = ""
    foreach b ( `seq 5 9` )
	foreach c ( `seq 0 9` )	    
	    set source = "$source ${base}_${a}${b}${c}.root"	   
	end
    end
    hadd -k -f $target $source >& logs/AuAu11Pico_D_${a}50-${a}99.out 
    
end

