#!/usr/bin/env csh

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# Input files
#set base =  Data/Small_LowMid_Y14HT/
set base =  Data/SmallY14HT/


#set NameBase=Groom_Fresh_NicksList_HC100
# set NameBase=Groom_HT64
# set NameBase=Groom_ConstSub
set NameBase=HiY14Test_Groom_AuAu_HT54_HTled
#set NameBase=Groom_AuAu_AjGt3_HT54_HTled
#set NameBase=Groom_AuAu_AjLt3_HT54_HTled

set submitted=0
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/${RMod}${NameBase}_${OutBase}.root

    # Use this for EtaCone
    # set OrigResultName = NONE 
    set OrigResultName = AjResults/${RMod}${NameBase}_AuAu.root
	
    set TriggerName = HT
    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${RMod}${NameBase}_${OutBase}.out
    set ErrFile    = logs/${RMod}${NameBase}_${OutBase}.err

    set Args = ( $OutName $TriggerName $Files 0 0 $OrigResultName )

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $LogFile
    echo "Logging errors to " $ErrFile
    echo
    
    qsub -V -p 10 -q  erhiq -l mem=4gb -W umask=0022 -N PreselGroom -o $LogFile -e $ErrFile -- ${ExecPath}/qwrap.sh ${ExecPath} $Exec $Args
    @ submitted = $submitted + 1
    # echo qsub -V -q erhiq -o $LogFile -e $ErrFile -- $Exec $Args 

end # foreach input

echo Submitted $submitted jobs to the grid.
