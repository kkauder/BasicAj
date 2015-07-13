#!/usr/bin/env csh

# make sure executable exists
make bin/ppInAuAuFollowAj || exit

# arguments
set OutName     = AjResults/ppInAuAuShiftFollowAj.root
set ppName      = AjResults/ppShiftFollowAj.root

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppInAuAuShiftFollowAj.out
set ErrFile     = logs/ppInAuAuShiftFollowAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

# trigger jet cuts
set LeadMin=19
set SubLeadMin=8.5

#command
set command = "./bin/ppInAuAuFollowAj $OutName $ppName $LeadMin $SubLeadMin"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









