#!/usr/bin/env csh

# CHANGE the following to suit your environment
setenv BASEDIR /Users/kkauder/software

### ROOT
setenv ROOTSYS ${BASEDIR}/root/

### FastJet
setenv FASTJETDIR ${BASEDIR}/fastjet3

### PYTHIA8
setenv PYTHIA8DIR ${BASEDIR}/pythia8
setenv PYTHIA8DATA ${PYTHIA8DIR}/xmldoc

### TStarJetPicoDst structure
setenv STARPICOPATH ${BASEDIR}/eventStructuredAu

### RooUnfold
### Optional. You can comment this line out, RooUnfold is not needed
setenv ROOUNFOLD ${BASEDIR}/RooUnfold

#### On rhic21, you can use
if ( `echo $HOST|grep -c rhic21` ) then
    setenv BASEDIR /usr/local
    setenv FASTJETDIR ${BASEDIR}/fastjet3
    setenv ROOTSYS ${BASEDIR}/root_v5.34-00-patches
    setenv PYTHIA8DIR ${BASEDIR}/pythia8
    setenv PYTHIA8DATA ${PYTHIA8DIR}/xmldoc
    setenv STARPICOPATH /Users/kkauder/eventStructuredAu
    # setenv STARPICOPATH /Users/kkauder/old/eventStructuredAu
    setenv ROOUNFOLD /Users/kkauder/RooUnfold-1.1.1
#    setenv BASEDIR /Users/putschke
#    setenv ROOTSYS /usr/local/root_v5.32_binary_m64/
#    setenv PYTHIA8DIR ${BASEDIR}/pythia8100.new
endif



############## Done with indivivual settings.

# ###### Initialize root
# echo 0
# source ${ROOTSYS}/bin/thisroot.csh ${ROOTSYS}
# echo 1

###### Update paths
if(! $?LD_LIBRARY_PATH) setenv LD_LIBRARY_PATH
if(! $?DYLD_LIBRARY_PATH) setenv DYLD_LIBRARY_PATH

setenv PATH ./bin:${ROOTSYS}/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${FASTJETDIR}/lib:${STARPICOPATH}:${PYTHIA8DIR}/lib:${LD_LIBRARY_PATH}
setenv DYLD_LIBRARY_PATH ${ROOTSYS}/lib:${FASTJETDIR}/lib:${STARPICOPATH}:${PYTHIA8DIR}/lib:${DYLD_LIBRARY_PATH}

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
echo "STARPICOPATH: " $STARPICOPATH
if ( $?ROOUNFOLD ) then
    echo "ROOUNFOLD: " $ROOUNFOLD
endif

echo "<I>---------------Info--------------------<I>"
echo ""
