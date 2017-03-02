#!/usr/bin/env csh

set ExecPath = `pwd`


set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}Groom_Aj_HT54_NoEff_Geant_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_NoEff_Geant

# set inbase  = ${RMod}Groom_Aj_HT54_HTled_NoEff_Geant_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_HTled_NoEff_Geant

# set inbase  = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant

# set inbase  = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_HTled_TrueMB_NoEff_GeantMc

set inbase  = ${RMod}Groom_Aj_TrueMB_NoCut_NoEff_GeantMc_picoDst
set outbase = ${RMod}Groom_Aj_TrueMB_NoCut_NoEff_GeantMc

# set inbase  = ${RMod}Groom_Aj_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}Groom_Aj_TrueMB_NoEff_Geant

# set inbase  = ${RMod}Tow0_Eff0_Groom_Aj_HT54_GeantMc_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_GeantMc

# set inbase  = ${RMod}Tow0_Eff0_Groom_Aj_HT54_Geant_picoDst
# set outbase = ${RMod}Tow0_Eff0_Groom_Aj_HT54_Geant


# set inbase  = ${RMod}Tow0_Eff0_Groom_Aj_HT54_GeantMc_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_GeantMc


set binjobs=''
set binouts=''	
foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )

    set In  = AjResults/Pieces/${inbase}_${bin}*.root
    set Out = AjResults/${outbase}_${bin}.root

    set LogFile = logs/Add_${inbase}_${bin}.out
    set ErrFile = logs/Add_${inbase}_${bin}.err
    
    set binjobs=${binjobs}:`qsub -V -q  erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
    # set binjobs=${binjobs}:`qsub -V -q  mwsuq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
    set binouts="${binouts} $Out"

    # hadd -f ${Out} ${In}
end

# finalize
set LogFile     = logs/Add_${inbase}.out
set ErrFile     = logs/Add_${inbase}.err

qsub -W depend=afterok${binjobs} -V -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f AjResults/${outbase}.root ${binouts}
#qsub -W depend=afterok${binjobs} -V -q  mwsuq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f AjResults/${outbase}.root ${binouts}

