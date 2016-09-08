#!/usr/bin/env csh

set ExecPath = `pwd`

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}Groom_Aj_HT54_HTled_ppInAuAuAj_newpicoDstcentralMB
# set outbase = ${RMod}Groom_Aj_HT54_HTled_ppInAuAuAj

set inbase  = ${RMod}Groom_Aj_HT54_HTled_NoEff_ppInAuAuAj_newpicoDstcentralMB
set outbase = ${RMod}Groom_Aj_HT54_HTled_NoEff_ppInAuAuAj

# Loop over combinations and data
set Effs='-1 0 1'
if ( $inbase =~ *NoEff* ) then
    set Effs=' 0 '
endif

foreach Tow ( -1 0 1 )
    foreach Eff ( $Effs )
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	
	if ( $inbase =~ *NoEff* ) then
	    if ( Tow${Tow} == Tow0 ) then
		set In  = AjResults/${inbase}*.root
		set Out = AjResults/${outbase}.root
	    else
		set In  = AjResults/Tow${Tow}_${inbase}*.root
		set Out = AjResults/Tow${Tow}_${outbase}.root
	    endif
	else
	    set In  = AjResults/Tow${Tow}_Eff${Eff}_${inbase}*.root
	    set Out = AjResults/Tow${Tow}_Eff${Eff}_${outbase}.root
	endif
	
	set LogFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}.out
	set ErrFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}.err

	qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}
	
    end
end

