#!/usr/bin/env csh

# make sure executable exists
make bin/PythiaAj || exit


# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/PythiaAj.out
set ErrFile     = logs/PythiaAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile


set command = "./bin/PythiaAj"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









