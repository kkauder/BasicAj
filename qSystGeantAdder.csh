#!/usr/bin/env csh

set ExecPath = `pwd`

#set RMod = ""
set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}SystGroom_Aj_HT54_HTled_Geant_picoDst
# set outbase = ${RMod}SystGroom_Aj_HT54_HTled_Geant
# set inbase  = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant
# set inbase  = ${RMod}PtSmearSystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}PtSmearSystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant

set inbase  = ${RMod}ForResAj_HT54_Geant_picoDst
set outbase = ${RMod}ForResAj_HT54_Geant

foreach Tow ( 0 -1 1 )
    foreach Eff ( 0 -1 1 )
	@ TowEff = $Tow * $Eff

	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;

# foreach Tow ( 0 -1 1 )
#     foreach Res ( 0 -1 1 )
#     # foreach Res ( 0 10 )
# 	foreach Eff ( 0 -1 )
	    
# 	    @ TowEff = $Tow * $Eff
# 	    @ TowRes = $Tow * $Res
# 	    @ ResEff = $Eff * $Res
	    
# 	    # Only combinations where at most 1 is !=0
# 	    # Except also allow Tow=1, Eff=-1
# 	    # if ( ( $TowEff != 0 && $TowEff != -1 )  || $TowRes != 0 || $ResEff != 0 ) continue;
# 	    # Allow all combinations...
	    	    
	    set binjobs=''
	    set binouts=''
	    set ab25binouts=''
	    foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )
		# echo $bin
		# set In  = AjResults/Pieces/AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}_${bin}*.root
		# set Out = AjResults/AEff${Eff}_PtRes${Res}_ATow${Tow}_${outbase}_${bin}.root

		# # Logfiles.
		# set LogFile     = logs/Add_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}_${bin}.out
		# set ErrFile     = logs/Add_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}_${bin}.err

		set In  = AjResults/Pieces/Tow${Tow}_Eff${Eff}_${inbase}_${bin}*.root
		set Out = AjResults/Tow${Tow}_Eff${Eff}_${outbase}_${bin}.root

		# Logfiles.
		set LogFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}_${bin}.out
		set ErrFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}_${bin}.err

		set binjobs=${binjobs}:`qsub -V -p 10 -q erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`		
		set binouts="${binouts} $Out"

		if ( $bin =~ *25_35* || $bin =~ *35_45* || $bin =~ *45_55* || $bin =~ *55_65*  ) set ab25binouts="${ab25binouts} $Out"
	    end
	    
	    # # finalize
	    # set Out = AjResults/AEff${Eff}_PtRes${Res}_ATow${Tow}_${outbase}.root
	    # set LogFile     = logs/Add_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}.out
	    # set ErrFile     = logs/Add_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}.err
	    # qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}

	    # # finalize
	    # set Out = AjResults/Above25_AEff${Eff}_PtRes${Res}_ATow${Tow}_${outbase}.root
	    # set LogFile     = logs/Add_Above25_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}.out
	    # set ErrFile     = logs/Add_Above25_AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}.err
	    # qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${ab25binouts}

	    # finalize
	    set Out = AjResults/Tow${Tow}_Eff${Eff}_${outbase}.root
	    set LogFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}.out
	    set ErrFile     = logs/Tow${Tow}_Eff${Eff}_${inbase}.err
	    qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${binouts}

	    # finalize
	    set Out = AjResults/Above25_Tow${Tow}_Eff${Eff}_${outbase}.root
	    set LogFile     = logs/Add_Above25_Tow${Tow}_Eff${Eff}_${inbase}.out
	    set ErrFile     = logs/Add_Above25_Tow${Tow}_Eff${Eff}_${inbase}.err
	    qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${ab25binouts}
	end
    end
end
