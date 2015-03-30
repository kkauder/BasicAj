#!/usr/bin/env csh

# make sure executable exists
make bin/FollowPicoAj || exit

# split into chunks
set base = CleanAuAu/Clean8
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/FollowAuAuAj_${OutBase}.root
    set TriggerName = HT
    set Files      = ${input}

    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/FollowAuAuAj_${OutBase}.out
    set ErrFile     = logs/FollowAuAuAj_${OutBase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile

    set command = "./bin/FollowPicoAj $OutName $TriggerName $Files"

    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end













