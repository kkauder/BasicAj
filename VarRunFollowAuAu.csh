#!/usr/bin/env csh

# make sure executable exists
make bin/FollowPicoAj || exit

# split into chunks
set base = ~putschke/Data/Pico_Eflow/auau_ht

set LeadMin=20
set SubLeadMin=10

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/VarFollowAuAuAj_${OutBase}.root
    set TriggerName = HT
    set Files      = ${input}

    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/VarFollowAuAuAj_${OutBase}.out
    set ErrFile     = logs/VarFollowAuAuAj_${OutBase}.err

    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile

    set command = "./bin/FollowPicoAj $OutName $LeadMin $SubLeadMin $TriggerName $Files"

    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end













