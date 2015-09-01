#!/usr/bin/env csh

# make sure executable exists
make bin/ppInAuAuAj || exit

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root

set RMod = ""
#set RMod = R0.2_
# set RMod = "Pt1_"

foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff
	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;
	
	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;

	set TowMod = Tow${Tow}
	set EffMod = Eff${Eff}
	# set PpName = AjResults/${TowMod}_${EffMod}_${RMod}ppAj.root
	set PpName = AjResults/${TowMod}_${EffMod}_${RMod}NicksList_HC100_ppAj.root

	foreach input ( ${base}* )
	    # arguments
	    set OutBase=`basename $input | sed 's/.root//g'`
	    # set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}ppInAuAuAj_${OutBase}.root
	    set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}NicksList_HC100_ppInAuAuAj_${OutBase}.root	    
	    # set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}MixTest_ppInAuAuAj_${OutBase}.root	    
	    set TriggerName = MB
	    set Files      = ${input}

	    # Logfiles. Thanks cshell for this "elegant" syntax to split err and out
	    # set LogFile     = logs/${TowMod}_${EffMod}_${RMod}ppInAuAuAj_${OutBase}.out
	    # set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}ppInAuAuAj_${OutBase}.err
	    set LogFile     = logs/${TowMod}_${EffMod}_${RMod}NicksList_HC100_ppInAuAuAj_${OutBase}.out
	    set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}NicksList_HC100_ppInAuAuAj_${OutBase}.err
	    # set LogFile     = logs/${TowMod}_${EffMod}_${RMod}MixTest_ppInAuAuAj_${OutBase}.out
	    # set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}MixTest_ppInAuAuAj_${OutBase}.err

	    # set LogFile     = logs/${TowMod}_${EffMod}_${RMod}HP_ppInAuAuAj_${OutBase}.out
	    # set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}HP_ppInAuAuAj_${OutBase}.err

	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    
	    set command = "./bin/ppInAuAuAj $OutName $PpName $TriggerName $Files"
	    
	    # Run in the background
	    echo "Executing " $command
	    ( $command > $LogFile ) >& $ErrFile &
	end
    end
end













