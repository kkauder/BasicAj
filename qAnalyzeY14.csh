#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/Y14analyzer"

# make sure executable exists
make $Exec || exit

# Input files
set base = Data/HaddedAuAu14/AuAu14Pico_

set TrigName=All
set NameBase=Analyze_Y14

# set TrigName=HT3
# set NameBase=HT3_Analyze_Y14

# set TrigName=HT2
# set NameBase=HT2_Analyze_Y14

set submitted=0
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/${NameBase}_${OutBase}.root

    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${NameBase}_${OutBase}.out
    set ErrFile    = logs/${NameBase}_${OutBase}.err

    set Args = ( $Files $OutName $TrigName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo
    
    qsub -p 10 -V -q erhiq -l mem=7gb -W umask=0022 -N AnalyzeY14 -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    # qsub -V -q mwsuq -l mem=7gb -W umask=0022 -N AnalyzeY14 -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    @ submitted = $submitted + 1
    # echo qsub -V -q erhiq -o $LogFile -e $ErrFile -- $Exec $Args 

end # foreach input

echo Submitted $submitted jobs to the grid.



