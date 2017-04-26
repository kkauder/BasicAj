#!/usr/bin/env csh

set ExecPath = `pwd`

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}ForResAj_NoEff_TrueMB_NoCut_GeantMc_picoDst
# set outbase = ${RMod}ForResAj_NoEff_TrueMB_NoCut_GeantMc

# set inbase  = Tow0_Eff0_${RMod}ForResAj_HT54_Geant_picoDst
# set outbase = ${RMod}ForResAj_HT54_Geant

# set inbase  = ${RMod}CrossCheckForResAj_HC70_NoEff_HT54_Geant_picoDst
# set outbase = ${RMod}CrossCheckForResAj_HC70_NoEff_HT54_Geant

set inbase  = ${RMod}BetterHT_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
set outbase = ${RMod}BetterHT_Groom_Aj_HT54_HTled_TrueMB_NoEff_Geant

# set inbase  = ${RMod}ForResAj_NoEff_HT54_Geant_picoDst
# set outbase = ${RMod}ForResAj_NoEff_HT54_Geant

# set inbase  = ${RMod}ForResAj_Geant_picoDst
# set outbase = ${RMod}ForResAj_Geant


set binjobs=''
set binouts=''
set ab25binouts=''

foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )
    set In  = AjResults/Pieces/${inbase}_${bin}*.root
    set Out = AjResults/${outbase}_${bin}.root

    # Logfiles.
    set LogFile     = logs/Add_${inbase}_${bin}.out
    set ErrFile     = logs/Add_${inbase}_${bin}.err

    set binjobs=${binjobs}:`qsub -V -p 10 -q erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
    set binouts="${binouts} $Out"

    if ( $bin =~ *25_35* || $bin =~ *35_45* || $bin =~ *45_55* || $bin =~ *55_65*  ) set ab25binouts="${ab25binouts} $Out"
end
	    
# finalize
set Out = AjResults/${outbase}.root
set LogFile     = logs/Add_${inbase}.out
set ErrFile     = logs/Add_${inbase}.err
qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}

# finalize
set Out = AjResults/Above25_${outbase}.root
set LogFile     = logs/Add_Above25_${inbase}.out
set ErrFile     = logs/Add_Above25_${inbase}.err
qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${ab25binouts}

