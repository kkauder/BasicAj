#!/usr/bin/env csh

# make sure executable exists
make bin/PpInAuAuFollowAj || exit

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppInAuAuFollowAj.out
set ErrFile     = logs/ppInAuAuFollowAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

set command = "./bin/ppInAuAuFollowAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









