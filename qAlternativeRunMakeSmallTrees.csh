#!/usr/bin/env csh

# make sure executable exists
set ExecPath = `pwd`
set Exec = "bin/MakeSmallerTrees"

# make sure executable exists
# make $Exec || exit

set ResultDir   = Data/SmallY14HT
set OutBase     = AuAu14Pico
set TriggerName = HT

# set segments = ( 01 234 56789 )
set segments0 = ( 0123456789 )
set segments1 = ( 0123 456789 )
set segments2 = ( 012 345 6789 )

set noglob
foreach directory ( `find Data/y14_AuAu_HT2/ -maxdepth 1 -name "[1-9]*"` )
    echo
    set tot = `ls -1 $directory/|wc -l`
    echo "$directory : \t\t ${tot}"

    if ( $tot<400 ) then
	set segments = ( ${segments0} )
    else if ( $tot<700 ) then
	set segments = ( ${segments1} )
    else 
    	set segments = ( ${segments2} )
    endif
	
    foreach i ( `seq 1 $#segments` )
	set segment=$segments[$i]

	# echo -n "$segment :  \t"
	#echo -n "$i :  \t"
	# find $directory -name "AuAu14Pico_*_[${segment}]*root" |wc -l
	
	set Files       = ${directory}/AuAu14Pico_*_[${segment}]*root
	set dirbase     = `basename ${directory}`_${i}
	set OutName     = ${ResultDir}/${OutBase}_${dirbase}.root

	set LogFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${dirbase}.out
	set ErrFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${dirbase}.err
        
	set Args = ( $Files $OutName )

	echo Submitting:
	echo $Exec $Args
	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile
	echo

	#-Wsandbox=HOME
	qsub -V -q erhiq -l mem=32gb -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    end
end

