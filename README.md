# BasicAj

# install dAuEventstructure
git clone https://github.com/kkauder/eventStructuredAu
cd eventStructuredAu
## The following should produce a lot of warnings but no errors
make clean && make

# In order to use the provided scripts, change your shell to csh #
csh

## Work on the actual package ##
cd path/to/BasicAj

# Create directory structure #
mkdir src/obj
mkdir bin
mkdir AjResults
mkdir logs

## create data symlinks ##
### THIS ONLY WORKS ON rhic21.physics.wayne.edu
ln -s ~putschke/Data
ln -s ~kkauder/JetAnalyzer/CleanAuAu
ln -s ~kkauder/JetAnalyzer/pytest.root

# Setup environment
### YOU NEED TO CUSTOMIZE THIS FILE FIRST!!!
source SetEnvironment.csh

# make and run #
make
source ./RunPp.csh
# or run directly
./binPicoAj


# That also creates doxygen documentation in html/ and latex/

## Files: ##
# src/JetAnalyzer.* is the basic wrapper that derives from $fastjet::ClusterSequenceArea
#AjParameters.hh is a namespace with commonly used parameters
#AjAnalysis.* is the analysis class

## Wrappers: ##
#PicoAj.cxx : Run on any data in TStarJetPicoDst format
#*InAuAu.cxx: Embed pythia or pp into AuAu
#*InMC.cxx: Embed pythia or pp into a toy MC version of AuAu

#Run*.csh: Simple csh scripts to run and redirect the output
#RunAuAu.csh is special because it submits 17 jobs. Combine via
hadd -f AjResults/AuAuAj.root AjResults/AuAuAj8*root

# Quickplot.cxx: Quick and dirty routine to show results.
# You should  use something better :)












