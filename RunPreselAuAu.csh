#!/usr/bin/env csh

### Combine with
# foreach n ( `seq 1 9` )
# hadd -f AjResults/rndm$n/Presel_AuAuAj.root AjResults/rndm$n/Presel_AuAuAj_Small_Clean8*root
# end
#hadd -f AjResults/Presel_AuAuAj.root AjResults/Presel_AuAuAj_Small_Clean8*root

# make sure executable exists
make bin/PicoAj || exit

# choose R or Pt options
# set RMod = R0.2_
# set RMod = Pt1_
set RMod = ""

# Input files
set base = SmallAuAu/Small_Clean8
#set base = Data/Pico_Eflow/auau_ht_pico*
#set base = CleanAuAu/Clean8
#set base = /data4/SmallAuAu11/Small_AuAu11Pico_*
#set base = /data4/SmallAuAu11NPE25/Small_AuAu11Pico_*

# DON'T try this! It will kill the server :)
# you can reasonably run two sets at once
#foreach randomoff ( `seq 0 9 ` )
#end # foreach randomoff

# choose random seed offset
set randomoff = 0
if { ( echo $1 | egrep -q '^[0-9]$' ) } then
    set randomoff = $1
else if ( $#argv > 0 ) then
    echo illegal arguments $argv
    exit -1    
endif

if ( $randomoff < 0  || $randomoff > 9 ) then
    echo illegal randomoff $randomoff
    exit -1
endif

set rndname = ""
if ( $randomoff > 0 ) then
    set rndname = rndm${randomoff}
    mkdir -pv AjResults/${rndname}
    mkdir -pv logs/${rndname}
endif

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    #set OutName    = AjResults/${rndname}/${RMod}VroniTest_${OutBase}.root
    #set OutName    = AjResults/${rndname}/${RMod}NPE25_TEST_${OutBase}.root
    set OutName    = AjResults/${rndname}/${RMod}HP_TEST_${OutBase}.root
    #set OutName    = AjResults/${rndname}/${RMod}HP_Presel_AuAuAj_${OutBase}.root
    #set OutName    = AjResults/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.root
	
    set TriggerName = HT
    set Files      = ${input}
    
    # Logfiles.
    # set LogFile     = logs/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.out
    # set ErrFile     = logs/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.err
    set LogFile     = logs/${rndname}/${RMod}test_${OutBase}.out
    set ErrFile     = logs/${rndname}/${RMod}test_${OutBase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    
    set command = "./bin/PicoAj $OutName $TriggerName $Files 0 0"
    
    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end # foreach input


