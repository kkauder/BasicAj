#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# Input files
# set base = Data/SmallNPE18/Small_AuAu11PicoNPE18_Cent[678]*.root
# set NameBase=Groom_Run11NPE18

set base = Data/HaddedAuAu11picoNPE25_150526/*.root
set NameBase=Groom_Run11NPE25

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
    set OutName    = AjResults/${rndname}/${RMod}${NameBase}_${OutBase}.root

    # Use this for EtaCone
    set OrigResultName = NONE 
    # set OrigResultName = AjResults/${rndname}/${RMod}${NameBase}_AuAu.root
	
    set TriggerName = HT
    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.out
    set ErrFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.err

    set Args = ( $OutName $TriggerName $Files 0 0 $OrigResultName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -q erhiq -o $LogFile -e $ErrFile -N Run11 -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args    
end # foreach input


