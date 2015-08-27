#!/usr/bin/env csh

### Combine with
# foreach n ( `seq 1 3` )
# hadd -f AjResults/rndm$n/R0.2_HC30_Presel.root AjResults/rndm$n/R0.2_HC30_Small_Clean8*root
# end
#hadd -f AjResults/Presel_AuAuAj.root AjResults/Presel_AuAuAj_Small_Clean8*root
#hadd -f AjResults/R0.2_HC30_Presel.root AjResults/R0.2_HC30_Small_Clean8*root

# make sure executable exists
make bin/PicoAj || exit

# choose R or Pt options
set RMod = ""
#set RMod = R0.2_
# set RMod = Pt1_

# Input files
set base = SmallAuAu/Small_Clean8

# DON'T try this! It will kill the server :)
#foreach randomoff ( `seq 0 9 ` )
#end # foreach randomoff

# you can reasonably run two sets at once

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
    #set OutName    = AjResults/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.root
    #set OutName    = AjResults/${rndname}/${RMod}NewCode_HC100_${OutBase}.root
    #set OutName    = AjResults/${rndname}/${RMod}FixedTowers_${OutBase}.root
    set OutName    = AjResults/${rndname}/${RMod}SoftHT0_HC100_${OutBase}.root
	
    set TriggerName = HT
    set Files      = ${input}
    
    # Logfiles.
    # set LogFile     = logs/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.out
    # set ErrFile     = logs/${rndname}/${RMod}Presel_AuAuAj_${OutBase}.err
    # set LogFile     = logs/${rndname}/${RMod}NewCode_HC100_${OutBase}.out
    # set ErrFile     = logs/${rndname}/${RMod}NewCode_HC100_${OutBase}.err
    # set LogFile     = logs/${rndname}/${RMod}FixedTowers_${OutBase}.out
    # set ErrFile     = logs/${rndname}/${RMod}FixedTowers_${OutBase}.err
    set LogFile     = logs/${rndname}/${RMod}SoftHT0_HC100_${OutBase}.out
    set ErrFile     = logs/${rndname}/${RMod}SoftHT0_HC100_${OutBase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    
    set command = "./bin/PicoAj $OutName $TriggerName $Files 0 0"
    
    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end # foreach input


