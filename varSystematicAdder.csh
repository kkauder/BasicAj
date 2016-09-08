#!/usr/bin/env csh

set ExecPath = `pwd`

set RMod = ""
#set RMod = R0.2_
#set RMod = Pt1_

# set inbase  = ${RMod}Groom_Aj_HT54_HTled_Geant_picoDst
# set outbase = ${RMod}Groom_Aj_HT54_HTled_Geant

set inbase  = ${RMod}Groom_Aj_HT54_HTled_Geant_picoDst
set outbase = ${RMod}Groom_Aj_HT54_HTled_Geant

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff

	if ( $TowEff != 0 ) continue;
	# echo $Tow $Eff

	set binjobs=''
	set binouts=''	
	foreach bin ( `find Data/GeantPythia/ -name '[0-9]*' -exec basename {} \; ` )
	    # echo $bin
	    set In  = AjResults/Pieces/Tow${Tow}_Eff${Eff}_${inbase}_${bin}*.root
	    set Out = AjResults/Tow${Tow}_Eff${Eff}_${outbase}_${bin}.root
	    set LogFile = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}_${bin}.out
	    set ErrFile = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}_${bin}.err
	    #set binjobs=${binjobs}:`qsub -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`
	    set binjobs=${binjobs}:`qsub -V -p 10 -q mwsuq -l mem=2gb -W umask=0022 -N AddGeantPart -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f ${Out} ${In}`

	    set binouts="${binouts} $Out"
	end
	
	# finalize
	set LogFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}.out
	set ErrFile     = logs/Add_Tow${Tow}_Eff${Eff}_${inbase}.err
	# qsub -W depend=afterok${binjobs} -V -p 10 -q  erhiq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f AjResults/Tow${Tow}_Eff${Eff}_${outbase}.root ${binouts}
	qsub -W depend=afterok${binjobs} -V -p 10 -q mwsuq -l mem=2gb -W umask=0022 -N AddGeant -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} hadd -f AjResults/Tow${Tow}_Eff${Eff}_${outbase}.root ${binouts}

    end
end
