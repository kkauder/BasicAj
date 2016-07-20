#!/usr/bin/env csh

# make sure executable exists
set ExecPath = `pwd`
set Exec = "bin/MakeSmallerTrees"

# make sure executable exists
make $Exec || exit

set ResultDir   = Data/SmallY14HT
set OutBase     = AuAu14Pico
set TriggerName = HT

set noglob
foreach directory ( `find Data/y14_AuAu_HT2/ -maxdepth 1 -name "[1-9]*"` )
    
    set Files       = ${directory}/*.root
    set dirbase     = `basename ${directory}`
    set OutName     = ${ResultDir}/${OutBase}_${dirbase}.root

    set LogFile     = logs/MakeSmallerTrees_${OutBase}_${dirbase}.out
    set ErrFile     = logs/MakeSmallerTrees_${OutBase}_${dirbase}.err
        
    set Args = ( $Files $OutName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -q erhiq -l mem=8gb -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

end

