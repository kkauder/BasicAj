#!/usr/bin/env csh

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# foreach Tow ( -1 0 1 )
#     foreach Eff ( -1 0 1 )
# 	@ TowEff = $Tow * $Eff

# 	if ( $TowEff != 0 ) continue;
# 	# echo $Tow $Eff

set inbase  = ${RMod}Groom_Aj_MB_GeantMc_picoDst
set outbase = ${RMod}Groom_Aj_MB_GeantMc

foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )
    # echo $bin
    set In  = AjResults/Pieces/${inbase}_${bin}*.root
    set Out = AjResults/${outbase}_${bin}.root
    hadd -f ${Out} ${In}
end

# finalize
hadd -f AjResults/${outbase}.root AjResults/${outbase}_*.root

