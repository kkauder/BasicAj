#!/usr/bin/env csh

# make sure executable exists
make bin/ppInMcAj || exit

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/ppInMcAj.out
set ErrFile     = logs/ppInMcAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile

set command = "ppInMcAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









