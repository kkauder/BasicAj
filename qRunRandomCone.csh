#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/RandomCone"

# make sure executable exists
make $Exec || exit

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root

#set RMod = ""
set RMod = R0.2_

# What to embed
set JetsName = AjResults/Fresh_NicksList_HC100_AuAu.root

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set JetsBase=`basename $JetsName | sed 's/.root//g'`
    set OutName    = AjResults/${RMod}WithDpt_RandomCone_${JetsBase}_${OutBase}.root
    set TriggerName = MB
    set Files      = ${input}

    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/${RMod}WithDpt_RandomCone_${JetsBase}_${OutBase}.out
    set ErrFile     = logs/${RMod}WithDpt_RandomCone_${JetsBase}_${OutBase}.err

    set Args = ( $OutName $JetsName $TriggerName $Files )
    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N RandomCone -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

end













