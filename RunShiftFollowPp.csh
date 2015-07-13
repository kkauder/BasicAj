#!/usr/bin/env csh

# make sure executable exists
make bin/FollowPicoAj || exit

# arguments
set OutName     = AjResults/ppShiftFollowAj.root
set TriggerName = ppHT
set Files       = Data/ppHT/*.root

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppShiftFollowAj.out
set ErrFile     = logs/ppShiftFollowAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

# trigger jet cuts
set LeadMin=19
set SubLeadMin=8.5
set command = "./bin/FollowPicoAj $OutName $LeadMin $SubLeadMin $TriggerName $Files"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









