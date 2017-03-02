#!/usr/bin/env csh

set ExecPath = `pwd`

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}SystGroom_Aj_HT54_HTled_Geant_picoDst
# set outbase = ${RMod}SystGroom_Aj_HT54_HTled_Geant
# set inbase  = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant
# set inbase  = ${RMod}PtSmearSystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant_picoDst
# set outbase = ${RMod}PtSmearSystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant

set inbase  = ${RMod}SystGroom_Aj_HT54_HTled_TrueMB_NoEff_Geant.root

foreach Tow ( 0 -1 1 )
    foreach Res ( 0 -1 1 )
    # foreach Res ( 0 -1 1 10 )
	foreach Eff ( 0 -1 )
	    
	    @ TowEff = $Tow * $Eff
	    @ TowRes = $Tow * $Res
	    @ ResEff = $Eff * $Res
	    
	    # Only combinations where at most 1 is !=0
	    # Except also allow Tow=1, Eff=-1
	    # if ( ( $TowEff != 0 && $TowEff != -1 )  || $TowRes != 0 || $ResEff != 0 ) continue;
	    	    
	    set In  = AjResults/AEff${Eff}_PtRes${Res}_ATow${Tow}_${inbase}

	    root -l -b -q MatchGeantToPythia.cxx+\(\"$In\"\)

	end
    end
end
