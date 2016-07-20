#!/usr/bin/env csh

# make sure executable exists
make bin/MakeSmallerTrees || exit

# split into chunks
#set base = CleanAuAu/Clean8
#set base = /data3/AuAu11picoNPE15_150424/AuAu11Pico_
#set base = /data4/AuAu11picoNPE15_150424/AuAu11Pico_
#set base = /data4/SmallAuAu11NPE25/AuAu11Pico_*
set base = /data3/HaddedAuAu11picoNPE18/AuAu11PicoNPE18_Cent5_*.root

foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set Files      = ${input}
    
    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
    set LogFile     = logs/MakeSmallerTrees_${OutBase}.out
    set ErrFile     = logs/MakeSmallerTrees_${OutBase}.err
    
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile

    set command = "./bin/MakeSmallerTrees $Files"

    # Run in the background
    echo "Executing " $command
    ( $command > $LogFile ) >& $ErrFile &
end





