#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomppInAuAuAj"

# make sure executable exists
make $Exec || exit

# split into chunks
set base = Data/NewPicoDst_AuAuCentralMB/newpicoDstcentralMB*.root

set RMod = ""
#set RMod = R0.2_
# set RMod = "Pt1_"


set PPbase=Groom_Aj_HT54_HTled_Geant
set outfile=Groom_Aj_HT54_HTled_GeantInAuAuAj

set nMix=1

# Loop over combinations and data
set submitted=0
foreach Tow ( -1 0 1 )
    foreach Eff ( -1 0 1 )
	@ TowEff = $Tow * $Eff
	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;
	
	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;

	set TowMod = Tow${Tow}
	set EffMod = Eff${Eff}
	set PpName = AjResults/${TowMod}_${EffMod}_${RMod}${PPbase}.root

	foreach input ( ${base}* )
	    # arguments
	    set OutBase=`basename $input | sed 's/.root//g'`
	    set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}${outfile}_${OutBase}.root
	    set TriggerName = MB
	    set Files      = ${input}
	    
	    set LogFile     = logs/${TowMod}_${EffMod}_${RMod}${outfile}_${OutBase}.out
	    set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}${outfile}_${OutBase}.err

	    rm -f $OutName $LogFile $ErrFile
	    
	    set Args = ( $OutName $PpName $TriggerName $Files ${nMix})
	    echo Submitting:
	    echo $Exec $Args
	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile

	    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N GeantInAuAuGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    @ submitted = $submitted + 1
	end
    end
end
echo Submitted $submitted jobs to the grid.













