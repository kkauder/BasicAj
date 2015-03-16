#!/bin/csh

if(! $?LD_LIBRARY_PATH) setenv LD_LIBRARY_PATH
if(! $?DYLD_LIBRARY_PATH) setenv DYLD_LIBRARY_PATH

setenv BASEDIR /Users/kkauder/software

#change to your root setup!
setenv ROOTSYS ${BASEDIR}/root/
setenv PATH ${ROOTSYS}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}
setenv DYLD_LIBRARY_PATH ${ROOTSYS}/lib:${DYLD_LIBRARY_PATH}

# FastJet
#setenv FASTJETDIR ${BASEDIR}/fastjet
setenv FASTJETDIR ${BASEDIR}/fastjet3
setenv LD_LIBRARY_PATH ${FASTJETDIR}/lib:${LD_LIBRARY_PATH}

# PYTHIA8 directory
setenv PYTHIA8DIR ${BASEDIR}/pythia8
setenv PYTHIA8DATA ${PYTHIA8DIR}/xmldoc

#TStarJetPicoDst structure
setenv STARPICOPATH ${BASEDIR}/eventStructuredAu

setenv LD_LIBRARY_PATH ${STARPICOPATH}:${PYTHIA8DIR}/lib:${LD_LIBRARY_PATH}
setenv DYLD_LIBRARY_PATH ${STARPICOPATH}:${PYTHIA8DIR}/lib:${DYLD_LIBRARY_PATH}

setenv PATH ./bin:${PATH}


if ($?TERM == 0 || $?prompt == 0) exit 0

echo ''
echo 'Setup ROOT, ktJet (including FastJet)'
echo '====================================='
echo ''
echo "<I>---------------Info--------------------<I>"
echo "Setting up the following environments: "
echo "ROOT: " $ROOTSYS
echo "PYTHIA8: " $PYTHIA8DIR
echo "FastJet: " $FASTJETDIR
echo "<I>---------------Info--------------------<I>"
echo ""
