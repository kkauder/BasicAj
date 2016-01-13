#!/usr/bin/env csh

set Exec = "./bin/TreeWithMc"

# make sure executable exists
make $Exec || exit

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >>CondorFile

set N = 100000
set seed = 1

# set ptstart  = ( 10 13 16 )
# set ptend    = ( 13 16 20 )

# set ptstart  = ( 20 23 26 30 35 )
# set ptend    = ( 23 26 30 35 40 )

set ptstart  = ( 0 2 4 6 8  )
set ptend    = ( 2 4 6 8 10 )

foreach i ( `seq 1 $#ptstart` )
    set pts=$ptstart[$i]
    set pte=$ptend[$i]

    set Args = ( $N $seed $pts $pte )
    
    set LogFile     = logs/TreeWithMc_${seed}_${pts}_${pte}.out
    set ErrFile     = logs/TreeWithMc_${seed}_${pts}_${pte}.err

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



