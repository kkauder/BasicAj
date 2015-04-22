#!/usr/bin/env csh

# make sure executable exists
make bin/ppInAuAuAj || exit

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/ppInAuAuAj_${OutBase}.root
    set TriggerName = MB
    set Files      = ${input}

    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/ppInAuAuAj_${OutBase}.out
    set ErrFile     = logs/ppInAuAuAj_${OutBase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile

    set command = "./bin/ppInAuAuAj $OutName $TriggerName $Files"

    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end













