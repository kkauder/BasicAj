#!/usr/bin/env csh

# make sure executable exists
make bin/PythiaInAuAuAj || exit


# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/PythiaInAuAuAj.out
set ErrFile     = logs/PythiaInAuAuAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile


set command = "PythiaInAuAuAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









