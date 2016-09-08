#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/TreeWithMc"

# make sure executable exists
make $Exec || exit

set N = 500000
set seed = 1

# set ptstart  = ( 10 13 16 )
# set ptend    = ( 13 16 20 )

# set ptstart  = ( 20 23 26 30 35 )
# set ptend    = ( 23 26 30 35 40 )

# set ptstart  = ( 0 2 4 6 8  )
# set ptend    = ( 2 4 6 8 10 )

# set ptstart  = ( 0 2 4 6 8  10 13 16 20 23 26 30 35 40 50 )
# set ptend    = ( 2 4 6 8 10 13 16 20 23 26 30 35 40 50 60 )
# set ptstart  = ( 40 50 )
# set ptend    = ( 50 60 )

set ptstart  = ( 3 4 5 7  9 11 15 25 35 45 55 )
set ptend    = ( 4 5 7 9 11 15 25 35 45 55 65 )

set submitted=0

foreach i ( `seq 1 $#ptstart` )
    set pts=$ptstart[$i]
    set pte=$ptend[$i]

    set Args = ( $N $seed $pts $pte )
    
    set LogFile     = logs/PythiaOnly_${seed}_${pts}_${pte}.out
    set ErrFile     = logs/PythiaOnly_${seed}_${pts}_${pte}.err

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo

    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N TreeWithMc -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    @ submitted = $submitted + 1
end

echo Submitted $submitted jobs to the grid.




