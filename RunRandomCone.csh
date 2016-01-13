#!/usr/bin/env csh

set Exec = "./bin/RandomCone"

# make sure executable exists
make $Exec || exit

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >>CondorFile

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root

# What to embed
set JetsName = AjResults/Fresh_NicksList_HC100_AuAu.root

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set JetsBase=`basename $JetsName | sed 's/.root//g'`
    set OutName    = AjResults/RandomCone_${JetsBase}_${OutBase}.root
    set TriggerName = MB
    set Files      = ${input}

    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/RandomCone_${JetsBase}_${OutBase}.out
    set ErrFile     = logs/RandomCone_${JetsBase}_${OutBase}.err

    ### hand to condor
    set Args = ( $OutName $JetsName $TriggerName $Files )
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

end
condor_submit CondorFile













