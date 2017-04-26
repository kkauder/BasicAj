#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

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
#set OutBase     = ${RMod}Groom_Aj_MB_GeantMc
#set OutBase     = ${RMod}Groom_Aj_LowPt_GeantMc
#set OutBase     = ${RMod}Groom_Aj_NoMass_HT54_GeantMc

# "Data", not MC!
# No efficiency correction
#set OutBase     = ${RMod}Groom_Aj_HT54_NoEff_Geant
# Normal (pp to Run7 AuAu) efficiency correction
#set OutBase     = ${RMod}Groom_Aj_HT54_HTled_Geant



#set OutBase     = ${RMod}ForResAj_HT54_Geant


# MC branch
# No efficiency correction
#set OutBase     = ${RMod}Groom_Aj_HT54_NoEff_GeantMc

set TriggerName = All

#set noglob
set submitted=0

foreach File ( Data/AddedGeantPythia/pico*root )
    foreach Tow ( 0 -1 1 )
	foreach Eff ( 0 -1 1 )
	    @ TowEff = $Tow * $Eff

	    # Only perpendicluar combinations
	    if ( $TowEff != 0 ) continue;

	
	    set FileBase    = `basename ${File} | sed s/.root//g`
	    set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root
	    
	    # Logfiles.
	    set LogFile     = logs/Tow${Tow}_Eff${Eff}_${OutBase}_${FileBase}.out
	    set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${OutBase}_${FileBase}.err
	    
	    # rm -f ${ResultDir}/Tow${Tow}_Eff${Eff}_${OutBase}_${FileBase}.root
	    rm -f $LogFile $ErrFile

	    set OrigResultName = NONE 
	    set Args = ( $OutName $TriggerName $File $Tow $Eff $OrigResultName )
	    
	    echo Submitting:
	    echo $Exec $Args
	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    # echo to resubmit:
	    # echo qsub -V -p 10 -q  erhiq -l mem=6gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    echo
	    
	    qsub -V -q  erhiq -l mem=6gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    # qsub -V -q  mwsuq -l mem=2gb -W umask=0022 -N SystGeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    @ submitted = $submitted + 1
	end
    end
end
#unset noglob
echo Submitted $submitted jobs to the grid.

