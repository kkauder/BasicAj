#!/usr/bin/env csh

# make sure executable exists
make bin/PicoAj || exit

# arguments
set OutName     = AjResults/VarPpAj.root
set TriggerName = ppHT
set Files       = Data/Pico_ppHT/*.root

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out
set LogFile     = logs/VarPpAj.out
set ErrFile     = logs/VarPpAj.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile


set command = "PicoAj $OutName $TriggerName $Files"

# Run in the background
# echo "Executing " $command
( $command > $LogFile ) >& $ErrFile &









