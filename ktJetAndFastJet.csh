#!/bin/csh

if(! $?LD_LIBRARY_PATH) setenv LD_LIBRARY_PATH
if(! $?DYLD_LIBRARY_PATH) setenv DYLD_LIBRARY_PATH

setenv BASEDIR /Users/putschke

#change to your root setup!
setenv ROOTSYS /usr/local/root_v5.32_binary_m64/
setenv PATH ${ROOTSYS}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}
setenv DYLD_LIBRARY_PATH ${ROOTSYS}/lib:${DYLD_LIBRARY_PATH}

# FastJet
#setenv FASTJETDIR ${BASEDIR}/fastjet
setenv FASTJETDIR ${BASEDIR}/fastjet3
setenv LD_LIBRARY_PATH ${FASTJETDIR}/lib:${LD_LIBRARY_PATH}

# ktJet settings
if ($#argv == 1) then
    setenv ktJetDIR $1
    #echo 'Setup local ktJet directory : ' $ktJetDIR
   
else
    #echo 'Setup standard ${HOME} ktJet directory ...'
    setenv ktJetDIR ${BASEDIR}/ktJet
endif

#echo ''

setenv PATH ${ktJetDIR}:${PATH}
setenv LD_LIBRARY_PATH ${ktJetDIR}:${LD_LIBRARY_PATH}

# PYTHIA8 directory
setenv PYTHIA8DIR ${BASEDIR}/pythia8100.new
#setenv PYTHIA8DIR ${BASEDIR}/pythia8100
setenv PYTHIA8DATA ${PYTHIA8DIR}/xmldoc

#TStarJetPicoDst structure
setenv STARPICOPATH /Users/kkauder/eventStructuredAu

setenv LD_LIBRARY_PATH ${STARPICOPATH}:${PYTHIA8DIR}/lib:${LD_LIBRARY_PATH}
setenv DYLD_LIBRARY_PATH ${STARPICOPATH}:${PYTHIA8DIR}/lib:${DYLD_LIBRARY_PATH}



if ($?TERM == 0 || $?prompt == 0) exit 0

echo ''
echo 'Setup ROOT, ktJet (including FastJet)'
echo '====================================='
echo ''
echo "<I>---------------Info--------------------<I>"
echo "Setting up the following environments: "
echo "ROOT: " $ROOTSYS
echo "PYTHIA8: " $PYTHIA8DIR
echo "ktJet: " $ktJetDIR
echo "FastJet: " $FASTJETDIR
echo "<I>---------------Info--------------------<I>"
echo ""
