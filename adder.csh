#!/usr/bin/env csh

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
    #foreach Eff ( 0 )
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	# echo $Tow $Eff
	
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Fresh_NicksList_HC100_ppInAuAuAj.root
	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Groom_Fresh_NicksList_HC100_ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Groom_ppInAuAuAj.root

	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Groom_Aj_HT54_HTled_GeantInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Groom_Aj_HT54_HTled_GeantInAuAuAj.root

	# set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Test_Groom_Aj_HT54_HTled_ppInAuAuAj_*.root
	# set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root

	set In  = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Test_Groom_Aj_HT54_HTled_ppInAuAuAj_AuAu14Pico*.root
	set Out = AjResults/Tow${Tow}_Eff${Eff}_${RMod}Y14Test_Groom_Aj_HT54_HTled_ppInAuAuAj.root

	hadd -f ${Out} ${In}
    end
end

