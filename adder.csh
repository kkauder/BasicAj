# set RMod = R0.2_
#set RMod = Pt1_
set RMod = ""

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	# set OutBase = AjResults/${TowMod}_${EffMod}_${RMod}ppInAuAuAj
	# echo hadd -f ${OutBase}.root ${OutBase}_*.root
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	# echo $Tow $Eff
	
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj.root
	set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}HP_ppInAuAuAj_*.root
	set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}HP_ppInAuAuAj.root
	hadd -f ${Out} ${In}
    end
end

