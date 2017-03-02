#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/SystGeantGroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# standard arguments
# note that outname will be modified by the binary!
set ResultDir   = AjResults/Pieces

## Must contain "Geant". If it contains "GeantMc", the MC tree will be used
#set OutBase     = ${RMod}SystGroom_Aj_MB_GeantMc
#set OutBase     = ${RMod}SystGroom_Aj_LowPt_GeantMc
#set OutBase     = ${RMod}SystGroom_Aj_NoMass_HT54_GeantMc

# "Data", not MC!
# set OutBase     = ${RMod}SystGroom_Aj_HT54_HTled_Geant
set OutBase     = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant
# set OutBase     = ${RMod}PtSmearSystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant
# No efficiency correction supported in this macro!!

set TriggerName = All

#set noglob
set submitted=0

foreach File ( Data/AddedGeantPythia/pico*root )
    foreach Tow ( 0 -1 1 )
    	foreach Res ( 0 -1 1 )
    	# foreach Res ( 0 10 )
    	    foreach Eff ( 0 -1 )
		
    		@ TowEff = $Tow * $Eff
    		@ TowRes = $Tow * $Res
    		@ ResEff = $Eff * $Res
		
    		# Only combinations where at most 1 is !=0
		# Except also allow Tow=1, Eff=-1
    		# if ( ( $TowEff != 0 && $TowEff != -1 )  || $TowRes != 0 || $ResEff != 0 ) continue;
		# Allow all combinations...

		# echo $Tow  $Res  $Eff
		# continue;
    # foreach Tow ( 1 )
    # 	foreach Res ( 0 )
    # 	    foreach Eff ( -1 )
	    
		set FileBase    = `basename ${File} | sed s/.root//g`
		set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root
		
		# Logfiles.
		set LogFile     = logs/AEff${Eff}_PtRes${Res}_ATow${Tow}_${OutBase}_${FileBase}.out
		set ErrFile     = logs/AEff${Eff}_PtRes${Res}_ATow${Tow}_${OutBase}_${FileBase}.err
		
		# rm -f ${ResultDir}/Tow${Tow}_Eff${Eff}_${OutBase}_${FileBase}.root
		rm -f $LogFile $ErrFile

		set OrigResultName = NONE 
		set Args = ( $OutName $TriggerName $File $Tow $Res $Eff $OrigResultName )
		
		echo Submitting:
		echo $Exec $Args
		echo "Logging output to " $LogFile
		echo "Logging errors to " $ErrFile
		# echo to resubmit:
		# echo qsub -V -p 10 -q  erhiq -l mem=6gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
		echo
		
		qsub -V -q  erhiq -l mem=4gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
		# qsub -V -q  mwsuq -l mem=2gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
		@ submitted = $submitted + 1
	    end
	end
    end
end
#unset noglob
echo Submitted $submitted jobs to the grid.

