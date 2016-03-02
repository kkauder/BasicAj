#!/usr/bin/env csh


set Exec = "./bin/PythiaAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
#set RMod = ""
set RMod = R0.2_
# set RMod = Pt1_

# Input files
# set base = SmallAuAu/Small_Clean8
set base = Data/RhicPythia/*root

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

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >>CondorFile
# Notification doesn't seem to work, and is spammy anyway
# echo "Notification = Complete" >> CondorFile
# echo "Notify_user  = kkauder@gmail.com"  >> CondorFile

set NameBase=Pythia

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/${rndname}/${RMod}${NameBase}_${OutBase}.root

    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.out
    set ErrFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.err

    ### hand to condor
    set Args = ( $OutName $Files )
    echo "" >> CondorFile
    echo "Output       = ${LogFile}" >> CondorFile
    echo "Error        = ${ErrFile}" >> CondorFile
    echo "Arguments    = ${Args}" >> CondorFile
    echo "Queue" >> CondorFile   

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo
    
end # foreach input

condor_submit CondorFile

