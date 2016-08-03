#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
set ResultDir   = AjResults
set OutBase     = ${RMod}Groom_GeantAj
set TriggerName = All
# set ChainName   = JetTreeMc

set noglob

foreach directory ( `find Data/GeantPythia/ -maxdepth 1 -name "[1-9]*"` )
    set Files       = ${directory}/*.root
    set dirbase     = `basename ${directory}`
    set OutName     = ${ResultDir}/${OutBase}_${dirbase}.root

    # Logfiles.
    set LogFile     = logs/${OutBase}_${dirbase}.out
    set ErrFile     = logs/${OutBase}_${dirbase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile

    set OrigResultName = NONE 
    set Args = ( $OutName $TriggerName $Files 0 0 )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    
end
unset noglob

