#set RMod = ""
set RMod = R0.2_
#set RMod = Pt1_

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	# echo $Tow $Eff
	
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj.root
	set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}HC30_ppInAuAuAj_*.root
	set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}HC30_ppInAuAuAj.root
	hadd -f ${Out} ${In}
    end
end

