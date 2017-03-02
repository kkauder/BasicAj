#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
#set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
#set OutName     = AjResults/${RMod}Groom_Fresh_NicksList_HC100_ppAj.root
#set OutName     = AjResults/${RMod}Groom_HT64_ppAj.root
#set OutName     = AjResults/${RMod}Groom_Aj_HT54_NoEff_ppAj.root

#set OutName     = AjResults/${RMod}Groom_Aj_HT54_HTled_NoEff_ppAj.root
#set OutName     = AjResults/${RMod}Groom_Aj_HT54_HTled_NoEff_ppAj.root
set OutName     = AjResults/${RMod}Test_Groom_Aj_HT54_HTled_ppAj.root
# set OutName     = AjResults/${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_ppAj.root

set OutName     = AjResults/${RMod}Delme_Groom_Aj_HT54_HTled_ppAj.root

set logbase = `basename $OutName | sed 's/.root//g'`

set TriggerName = ppHT

set noglob
set Files       = Data/ppHT/*.root




# foreach Tow ( -1 0 1 )
#     foreach Eff ( -1 0 1 )
foreach Tow ( 0 )
    foreach Eff ( 0 )
 	@ TowEff = $Tow * $Eff

	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;

	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;
	
	# Logfiles.
	set LogFile     = logs/Tow${Tow}_Eff${Eff}_${logbase}.out
	set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${logbase}.err
	
	set OrigResultName = NONE 
	set Args = ( $OutName $TriggerName $Files $Tow $Eff $OrigResultName )

	echo Submitting:
	echo $Exec $Args
	echo "Logging output to " $LogFile
	echo "Logging errors to " $ErrFile
	echo

	qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N PpGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	# qsub -V -q erhiq -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args

    end
end
unset noglob

