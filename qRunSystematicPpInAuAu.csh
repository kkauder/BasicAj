#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/ppInAuAuAj"

# make sure executable exists
make $Exec || exit

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root

set RMod = ""
#set RMod = R0.2_
# set RMod = "Pt1_"

# Loop over combinations and data
foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff
	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;
	
	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;

	set TowMod = Tow${Tow}
	set EffMod = Eff${Eff}
	set PpName = AjResults/${TowMod}_${EffMod}_${RMod}Fresh_NicksList_HC100_ppAj.root

	foreach input ( ${base}* )
	    # arguments
	    set OutBase=`basename $input | sed 's/.root//g'`
	    set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.root
	    set TriggerName = MB
	    set Files      = ${input}

	    set LogFile     = logs/${TowMod}_${EffMod}_${RMod}Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.out
	    set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.err

	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    
	    set Args = ( $OutName $PpName $TriggerName $Files )

	    echo Submitting:
	    echo $Exec $Args
	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    echo

	    qsub -V -q erhiq -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	end
    end
end













