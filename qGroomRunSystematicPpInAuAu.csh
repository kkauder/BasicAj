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

#set AjMod =""
#set AjMod = "_AjGt3"
set AjMod = "_AjLt3"

#set PpBase = Groom_Aj_HT54_HTled_NoEff_ppAj.root
set PpBase = Groom_Aj_HT54_HTled_ppAj.root

# Loop over combinations and data
set Effs='-1 0 1'
if ( $PpBase =~ *NoEff* ) then
    set Effs=' 0 '
endif

foreach Tow ( -1 0 1 )
    foreach Eff ( $Effs )
	@ TowEff = $Tow * $Eff
	# Only perpendicluar combinations
	if ( $TowEff != 0 ) continue;
	

	# Uncomment to skip remaking nominal
	# if ( $Tow == 0 && $Eff == 0 ) continue;

	set TowMod = Tow${Tow}
	set EffMod = Eff${Eff}

	set PpName = AjResults/${TowMod}_${EffMod}_${RMod}${PpBase}
	if ( $PpBase =~ *NoEff* ) then
	    set PpName = AjResults/${TowMod}_${RMod}${PpBase}
	    if ( ${TowMod} == Tow0 ) then
		set PpName = AjResults/${RMod}${PpBase}
	    endif
	endif

	foreach input ( ${base}* )
	    # arguments
	    set OutBase=`basename $input | sed 's/.root//g'`
	    #set OutName    = AjResults/${TowMod}_${EffMod}_${RMod}Groom_Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.root
	    set OutName    = `echo ${PpName} | sed "s/ppAj/ppInAuAuAj/g" | sed "s/.root//g"`${AjMod}_${OutBase}.root

	    set TriggerName = MB
	    set Files      = ${input}
	    
	    # set LogFile     = logs/${TowMod}_${EffMod}_${RMod}Groom_Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.out
	    # set ErrFile     = logs/${TowMod}_${EffMod}_${RMod}Groom_Fresh_NicksList_HC100_ppInAuAuAj_${OutBase}.err
	    set LogFile     = `echo ${OutName} | sed "s/AjResults/logs/g" | sed "s/.root/.out/g"`
	    set ErrFile     = `echo ${OutName} | sed "s/AjResults/logs/g" | sed "s/.root/.err/g"`
	    
	    set Args = ( $OutName $PpName $TriggerName $Files )

	    echo Submitting:
	    echo $Exec $Args
	    echo "Logging output to " $LogFile
	    echo "Logging errors to " $ErrFile
	    
	    
	    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N PpInAuAuGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	    # qsub -V -q erhiq -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
	end
    end
end












