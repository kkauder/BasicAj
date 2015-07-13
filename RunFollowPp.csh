#!/usr/bin/env csh

# make sure executable exists
make bin/FollowPicoAj || exit

# arguments
set OutName     = AjResults/ppFollowAj.root
set TriggerName = ppHT
set Files       = Data/ppHT/*.root

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppFollowAj.out
set ErrFile     = logs/ppFollowAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile


set LeadMin=20
set SubLeadMin=10
set command = "./bin/FollowPicoAj $OutName $LeadMin $SubLeadMin $TriggerName $Files"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









