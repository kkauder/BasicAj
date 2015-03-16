#!/usr/bin/env csh

# make sure executable exists
make bin/ppInAuAuAj || exit

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppInAuAuAj.out
set ErrFile     = logs/ppInAuAuAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

set command = "./bin/ppInAuAuAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









