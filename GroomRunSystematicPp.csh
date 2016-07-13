#!/usr/bin/env csh

set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
#set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
set OutName     = AjResults/${RMod}Groom_Fresh_NicksList_HC100_ppAj.root
set TriggerName = ppHT

set noglob
set Files       = Data/ppHT/*.root

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >>CondorFile
# Notification doesn't seem to work, and is spammy anyway
# echo "Notification = Complete" >> CondorFile
# echo "Notify_user  = kkauder@gmail.com"  >> CondorFile

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff

	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;

	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;
	
	# Logfiles.
	# set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}ppgAj.out
	# set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}ppAj.err
	set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppAj.out
	set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppAj.err	
	# set LogFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}OldList_HC100_ppAj.out
	# set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${RMod}OldList_HC100_ppAj.err	

	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile

	set OrigResultName = NONE 
	set Args = ( $OutName $TriggerName $Files $Tow $Eff $OrigResultName )

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
end
unset noglob

condor_submit CondorFile

