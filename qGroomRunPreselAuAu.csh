#!/usr/bin/env csh

### Combine with
# foreach n ( `seq 1 3` )
# hadd -f AjResults/rndm$n/R0.2_HC30_Presel.root AjResults/rndm$n/R0.2_HC30_Small_Clean8*root
# end
#hadd -f AjResults/Presel_AuAuAj.root AjResults/Presel_AuAuAj_Small_Clean8*root
#hadd -f AjResults/R0.2_HC30_Presel.root AjResults/R0.2_HC30_Small_Clean8*root

set ExecPath = `pwd`
set Exec = "./bin/GroomPicoAj"

# make sure executable exists
make $Exec || exit

# choose R or Pt options
set RMod = ""
# set RMod = R0.2_
# set RMod = Pt1_

# Input files
set base = Data/SmallAuAu/Small_Clean8
# TEST
# set base = Data/CleanAuAuY7/Clean8

# DON'T try this! It will kill the server :)
#foreach randomoff ( `seq 0 9 ` )
#end # foreach randomoff

# you can reasonably run two sets at once

# choose random seed offset
set randomoff = 0
if { ( echo $1 | egrep -q '^[0-9]$' ) } then
    set randomoff = $1
else if ( $#argv > 0 ) then
    echo illegal arguments $argv
    exit -1    
endif

if ( $randomoff < 0  || $randomoff > 9 ) then
    echo illegal randomoff $randomoff
    exit -1
endif

set rndname = ""
if ( $randomoff > 0 ) then
    set rndname = rndm${randomoff}
    mkdir -pv AjResults/${rndname}
    mkdir -pv logs/${rndname}
endif

#set NameBase=Groom_Fresh_NicksList_HC100
# set NameBase=Groom_HT64
# set NameBase=Groom_ConstSub
#set NameBase=Groom_AuAu_HT54_HTled
set NameBase=Groom_AuAu_AjGt3_HT54_HTled
#set NameBase=Groom_AuAu_AjLt3_HT54_HTled

set submitted=0
foreach input ( ${base}* )
    # arguments
    set OutBase=`basename $input | sed 's/.root//g'`
    set OutName    = AjResults/${rndname}/${RMod}${NameBase}_${OutBase}.root

    # Use this for EtaCone
    # set OrigResultName = NONE 
    set OrigResultName = AjResults/${rndname}/${RMod}${NameBase}_AuAu.root
	
    set TriggerName = HT
    set Files      = ${input}
    
    # Logfiles.
    set LogFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.out
    set ErrFile    = logs/${rndname}/${RMod}${NameBase}_${OutBase}.err

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
