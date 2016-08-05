os = $(shell uname -s)

INCFLAGS      = -I$(ROOTSYS)/include -I$(FASTJETDIR)/include -I$(PYTHIA8DIR)/include -I$(PYTHIA8DIR)/include/Pythia8/ -I$(PYTHIA8DIR)/include/Pythia8Plugins/ -I$(STARPICOPATH)
INCFLAGS      += -I./src


ifeq ($(os),Linux)
CXXFLAGS      = -O2 -fPIC -pipe -Wall -std=c++11
CXXFLAGS     += -Wno-unused-variable
CXXFLAGS     += -Wno-unused-but-set-variable
CXXFLAGS     += -Wno-sign-compare
else
CXXFLAGS      = -O -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
CXXFLAGS     += -Wno-return-type-c-linkage
## for debugging:
# CXXFLAGS      = -g -O0 -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
endif

ifeq ($(os),Linux)
#LDFLAGS       = -g
#LDFLAGSS      = -g --shared 
LDFLAGS       = 
LDFLAGSS      = --shared 
else
LDFLAGS       = -O -Xlinker -bind_at_load -flat_namespace
LDFLAGSS      = -flat_namespace -undefined suppress
LDFLAGSSS     = -bundle
endif

ifeq ($(os),Linux)
CXX          = g++ 
else
CXX          = clang
endif

# # uncomment for debug info in the library
# CXXFLAGS     += -g


ROOTLIBS      = $(shell root-config --libs)

LIBPATH       = $(ROOTLIBS) -L$(FASTJETDIR)/lib -L$(PYTHIA8DIR)/lib -L$(STARPICOPATH)
LIBS          = -lfastjet -lfastjettools -lpythia8  -lTStarJetPico -lRecursiveTools
LIBS         += -lConstituentSubtractor

## fun with pythia :-/
## make is a horrible horrible tool. Do not touch these lines, any whitespace will make it break
dummy := "$(shell find $(PYTHIA8DIR)/lib/ -name liblhapdfdummy\*)"
ifneq ("",$(dummy))
LIBS         += -llhapdfdummy
endif

# for cleanup
SDIR          = src
ODIR          = src/obj
BDIR          = bin


###############################################################################
################### Remake when these headers are touched #####################
###############################################################################
INCS = $(SDIR)/JetAnalyzer.hh $(SDIR)/AjParameters.hh

###############################################################################
# standard rules
$(ODIR)/%.o : $(SDIR)/%.cxx $(INCS)
	@echo 
	@echo COMPILING
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

$(BDIR)/%  : $(ODIR)/%.o 
	@echo 
	@echo LINKING
	$(CXX) $(LDFLAGS) $(LIBPATH) $^ $(LIBS) -o $@

###############################################################################

###############################################################################
############################# Main Targets ####################################
###############################################################################
all    : $(BDIR)/PicoAj  $(BDIR)/ppInAuAuAj  \
	 $(BDIR)/PythiaAj  \
	 $(BDIR)/RandomCone  \
	 $(BDIR)/TreeWithMc \
	 $(BDIR)/JustMc \
	 $(BDIR)/MakeSmallerTrees \
	 lib/libMyJetlib.so \
	 $(BDIR)/GroomPicoAj \
	 $(BDIR)/GroomppInAuAuAj \
	 $(BDIR)/Y14analyzer
#	 doxy

#	 $(BDIR)/ScanTree \
#	 $(BDIR)/AlternateEmbedding  \

# lib/libMyJetlib.a \

# $(BDIR)/FollowPythiaAj 
# 	 $(BDIR)/ppInMcAj $(BDIR)/PythiaInMcAj \

$(SDIR)/dict.cxx 		: $(SDIR)/ktTrackEff.hh
	cd $(SDIR); rootcint -f dict.cxx -c -I. ./ktTrackEff.hh

$(ODIR)/dict.o 		: $(SDIR)/dict.cxx
$(ODIR)/ktTrackEff.o 	: $(SDIR)/ktTrackEff.cxx $(SDIR)/ktTrackEff.hh



# $(BDIR)/AreaTest : 	 $(ODIR)/AreaTest.o
# $(BDIR)/AreaTest.o : 	 $(SDIR)/AreaTest.cxx

lib/libMyJetlib.so	: $(ODIR)/JetAnalyzer.o $(ODIR)/dict.o $(ODIR)/ktTrackEff.o
	@echo 
	@echo MAKING LIBRARY
	$(CXX) -shared $(LDFLAGS) $(LIBPATH) $(LIBS) $^ -o $@

#	g++ -flat_namespace -undefined suppress dict.o -L/usr/local/root_v5.32_binary_m64/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,/usr/local/root_v5.32_binary_m64/lib -lm -ldl -o libKtJet.so
# ar -rcs $@ $^

# lib/libMyJetlib.a	: $(ODIR)/JetAnalyzer.o $(ODIR)/dict.o $(ODIR)/ktTrackEff.o
# 	@echo 
# 	@echo MAKING LIBRARY
# 	ar -rcs $@ $^

$(ODIR)/JetAnalyzer.o 		: $(SDIR)/JetAnalyzer.cxx $(INCS)
$(ODIR)/AjAnalysis.o 	 	: $(SDIR)/AjAnalysis.cxx $(INCS) $(SDIR)/AjAnalysis.hh


#Aj
$(BDIR)/PicoAj		: $(ODIR)/PicoAj.o		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/ppInAuAuAj 	: $(ODIR)/ppInAuAuAj.o 		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/ppInMcAj	: $(ODIR)/ppInMcAj.o		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/PythiaAj	: $(ODIR)/PythiaAj.o 		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/PythiaInAuAuAj	: $(ODIR)/PythiaInAuAuAj.o 	$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/PythiaInMcAj	: $(ODIR)/PythiaInMcAj.o 	$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/TestCountPythia	: $(ODIR)/TestCountPythia.o 	$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so

$(BDIR)/TreeWithMc      : $(ODIR)/TreeWithMc.o		lib/libMyJetlib.so
$(BDIR)/JustMc		: $(ODIR)/JustMc.o
$(BDIR)/RandomCone	: $(ODIR)/RandomCone.o		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so

$(BDIR)/GroomPicoAj	: $(ODIR)/GroomPicoAj.o		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so
$(BDIR)/GroomppInAuAuAj	: $(ODIR)/GroomppInAuAuAj.o	$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so

#$(BDIR)/AlternateEmbedding 	: $(ODIR)/AlternateEmbedding.o 		$(ODIR)/AjAnalysis.o	 	lib/libMyJetlib.so


# helper
$(BDIR)/Y14analyzer	: $(ODIR)/Y14analyzer.o		$(ODIR)/AjAnalysis.o		lib/libMyJetlib.so

$(BDIR)/MakeSmallerTrees	: $(ODIR)/MakeSmallerTrees.o	 	lib/libMyJetlib.so
$(BDIR)/ScanTree		: $(ODIR)/ScanTree.o	 		lib/libMyJetlib.so


###############################################################################
##################################### MISC ####################################
###############################################################################


doxy: html/index.html

html/index.html : $(INCS) src/* Doxyfile
#	doxygen
	@echo 
	@echo Updating documentation
	( cat Doxyfile ; echo "QUIET=YES" ) | doxygen -

clean :
	@echo 
	@echo CLEANING
	rm -vf $(ODIR)/*.o
	rm -rvf $(BDIR)/*dSYM
	rm -rvf lib/*dSYM	
	rm -vf $(BDIR)/*
	rm -vf lib/*
	rm -vf $(SDIR)/dict.cxx $(SDIR)/dict.h

.PHONY : clean doxy
