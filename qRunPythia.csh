#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/PythiaAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# Input files
#set base = Data/RhicPythia/*root
set base = Data/AlternateRhicPythia/*root

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

set NameBase=HT54_NoEff

set submitted=0
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/${rndname}/${RMod}${NameBase}_${OutBase}.root

    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.out
    set ErrFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.err

    # Arguments
    set Args = ( $OutName $Files )
    
    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N PythiaAj -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    @ submitted = $submitted + 1
end # foreach input

echo Submitted $submitted jobs to the grid.


