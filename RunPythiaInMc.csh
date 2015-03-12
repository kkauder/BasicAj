#!/usr/bin/env csh

# make sure executable exists
make bin/PythiaInMcAj || exit


# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/PythiaInMcAj.out
set ErrFile     = logs/PythiaInMcAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile


set command = "PythiaInMcAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









