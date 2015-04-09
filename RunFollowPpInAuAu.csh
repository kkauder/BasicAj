#!/usr/bin/env csh

# make sure executable exists
make bin/PpInAuAuFollowAj || exit

# arguments
set OutName     = AjResults/ppInAuAuFollowAj.root
set ppName      = AjResults/ppFollowAj.root

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppInAuAuFollowAj.out
set ErrFile     = logs/ppInAuAuFollowAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

# trigger jet cuts
set LeadMin=20
set SubLeadMin=10

#command
set command = "./bin/ppInAuAuFollowAj $OutName $ppName $LeadMin $SubLeadMin"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









