#!/usr/bin/env csh

# make sure executable exists
set ExecPath = `pwd`
set Exec = "bin/MakeSmallerTrees"

# make sure executable exists
make $Exec || exit

#set ResultDir   = Data/SmallY14HT
set ResultDir   = Data/Small_LowMid_Y14HT

set OutBase     = AuAu14Pico
set TriggerName = HT

set input = Data/HaddedAuAu14Low
#set input = Data/HaddedAuAu14Mid

set submitted=0
foreach File ( ${input}/*.root )
    
    set OutName     = ${ResultDir}/Small`basename ${File}`

    set LogFile     = logs/MakeSmallerTrees_Small`basename ${File}`.out
    set ErrFile     = logs/MakeSmallerTrees_Small`basename ${File}`.err
        
    set Args = ( $File $OutName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    # qsub -V -q erhiq -l mem=8gb -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N SmallTrees -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    @ submitted = $submitted + 1

end

echo Submitted $submitted jobs to the grid.

