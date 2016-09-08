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
#set OutBase     = ${RMod}Groom_Aj_TrueMB_NoEff_Geant
set OutBase     = ${RMod}Groom_Aj_HT54_HTled_NoEff_Geant
#set OutBase     = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant

# Normal (pp to Run7 AuAu) efficiency correction
#DONT USE
#set OutBase     = ${RMod}Groom_Aj_HT54_HTled_Geant

# MC branch
# No efficiency correction
# set OutBase     = ${RMod}Groom_Aj_HT54_NoEff_GeantMc
# set OutBase     = ${RMod}Groom_Aj_MB_NoEff_GeantMc
set OutBase     = ${RMod}Groom_Aj_TrueMB_NoEff_GeantMc
# set OutBase     = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc

set TriggerName = All

#set noglob
set submitted=0
# foreach directory ( `find Data/GeantPythia/ -maxdepth 1 -name "[1-9]*"` )
#     set Files       = ${directory}/*.root
#     set dirbase     = `basename ${directory}`
#     set OutName     = ${ResultDir}/${OutBase}_${dirbase}.root

    # # Logfiles.
    # set LogFile     = logs/${OutBase}_${dirbase}.out
    # set ErrFile     = logs/${OutBase}_${dirbase}.err

foreach File ( Data/AddedGeantPythia/pico*root )
    set FileBase    = `basename ${File} | sed s/.root//g`
    set OutName     = ${ResultDir}/${OutBase}_${FileBase}.root

    # Logfiles.
    set LogFile     = logs/${OutBase}_${FileBase}.out
    set ErrFile     = logs/${OutBase}_${FileBase}.err

    #rm -f $OutName $LogFile $ErrFile 
    
    set OrigResultName = NONE 
    set Args = ( $OutName $TriggerName $File 0 0 )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo to resubmit:
    echo qsub -V -q  mwsuq -l mem=6gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    echo

    qsub -V -p 10 -q  erhiq -l mem=6gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    ## PBS test:
    # qsub -V -p 10 -q  mwsuq -l mem=6gb -W umask=0022 -N GeantGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    
    @ submitted = $submitted + 1
end
#unset noglob
echo Submitted $submitted jobs to the grid.

