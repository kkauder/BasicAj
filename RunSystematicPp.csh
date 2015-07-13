#!/usr/bin/env csh

# make sure executable exists
make bin/PicoAj || exit

# choose R or Pt options
set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
set OutName     = AjResults/${RMod}ppAj.root
# set OutName     = AjResults/${RMod}HP_ppAj.root
set TriggerName = ppHT

set noglob
set Files       = Data/ppHT/*.root

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff
	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;

	# Comment out to remake nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;
	
	# Logfiles.
	# set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}ppAj.out
	# set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}ppAj.err
	set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}HP_ppAj.out
	set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}HP_ppAj.err

	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile

	set command = "./bin/PicoAj $OutName $TriggerName $Files $Tow $Eff"

	# Run in the background
	echo "Executing " $command
	( $command > $LogFile ) >& $ErrFile &
    end
end
	
unset noglob
