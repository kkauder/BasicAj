#!/usr/bin/env csh

set Exec = "./bin/JustMc"

# make sure executable exists
make $Exec || exit

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >>CondorFile

foreach seed ( `seq 1 5` )
    set Args = ( $seed )
    
    set LogFile     = logs/McOnly_${seed}.out
    set ErrFile     = logs/McOnly_${seed}.err

    ### hand to condor
    echo "" >> CondorFile
    echo "Output       = ${LogFile}" >> CondorFile
    echo "Error        = ${ErrFile}" >> CondorFile
    echo "Arguments    = ${Args}" >> CondorFile
    echo "Queue" >> CondorFile   

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

end
condor_submit CondorFile



