set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	# echo $Tow $Eff
	
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}ppInAuAuAj.root
	set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}NewCode_HC100_ppInAuAuAj_*.root
	set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}NewCode_HC100_ppInAuAuAj.root
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}MixTest_ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}MixTest_ppInAuAuAj.root
	hadd -f ${Out} ${In}
    end
end

