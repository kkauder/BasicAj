#!/usr/bin/env csh

# make sure executable exists
set ExecPath = `pwd`
set Exec = "bin/MakeSmallerTrees"

# make sure executable exists
# make $Exec || exit

set DataDir   = Data/HaddedAuAu14

set ResultDir   = Data/SmallY14HT
set OutBase     = AuAu14Pico
set TriggerName = HT


set daygroups = ( 101-105 106-110 111-115 116-120 121-125 126-130 131-135 136-140 141-145 146-150 151-155 156-160 161-167 86-90 91-95 96-100 )
#set daygroups = ( 101-105 )
# set daygroups = ( 106-110 111-115 116-120 121-125 126-130 131-135 136-140 141-145 146-150 151-155 156-160 161-167 86-90 91-95 96-100 )

# Everything above 9 will be ONE group
set subgroups = ( 012 345 6789 )

set noglob
set submitted=0
foreach daygroup ( $daygroups )
    echo "Group: $daygroup"
    
    # only work for combining single digit files!
    foreach subgroup ( $subgroups )
	echo Subgroup: ${subgroup}
	set pattern = AuAu14Pico_${daygroup}_[${subgroup}].root
	set tmp=`find ${DataDir} -name "${pattern}" `
	set ntmp = $#tmp
	if ( $ntmp > 0 ) then
	    # echo $ntmp
	    du -csh $tmp |grep total
	    set Files       = ${DataDir}/$pattern
	    set OutName     = ${ResultDir}/${OutBase}_${daygroup}_${subgroup}.root

	    set LogFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${daygroup}_${subgroup}.out
	    set ErrFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${daygroup}_${subgroup}.err

	    set Args = ( $Files $OutName )
	    echo Submitting:
	    echo $Exec $Args
	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    echo
	    
	    #-Wsandbox=HOME
	    # qsub -V -q erhiq -l mem=4gb -W umask=0022 -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    @ submitted = $submitted + 1
	    
	endif
    end

    # Soak up the rest
    echo Subgroup: Two+ digits
    set pattern = AuAu14Pico_${daygroup}_[0-9][0-9]*.root
    set tmp=`find ${DataDir} -name "${pattern}" `
    set ntmp = $#tmp
    if ( $ntmp > 0 ) then
	du -csh $tmp |grep total

	set Files       = ${DataDir}/$pattern
	set OutName     = ${ResultDir}/${OutBase}_${daygroup}_rest.root
	
	set LogFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${daygroup}_rest.out
	set ErrFile     = /nfs/rhi/kolja/logs/MakeSmallerTrees_${OutBase}_${daygroup}_rest.err
	
	set Args = ( $Files $OutName )
	echo Submitting:
	echo $Exec $Args
	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile
	echo
	
	#-Wsandbox=HOME
	# qsub -V -q erhiq -l mem=4gb -W umask=0022 -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	@ submitted = $submitted + 1
    endif
    
    echo ======================================================
    echo
    
end

echo Submitted $submitted jobs to the grid.

