#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/PicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
#set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
set OutName     = AjResults/${RMod}Fresh_NicksList_HC100_ppAj.root
set TriggerName = ppHT

set noglob
set Files       = Data/ppHT/*.root

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff

	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;

	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;
	
	# Logfiles.
	set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppAj.out
	set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppAj.err	

	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile

	set OrigResultName = NONE 
	set Args = ( $OutName $TriggerName $Files $Tow $Eff $OrigResultName )

	echo Submitting:
	echo $Exec $Args
	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile
	echo

	qsub -V -q erhiq -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    end
end
unset noglob

