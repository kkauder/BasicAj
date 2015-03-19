os = $(shell uname -s)

#INCFLAGS      = -I$(ROOTSYS)/include -I$(FASTJETDIR)/include -I$(PYTHIA8DIR)/include -I$(STARPICOPATH)
INCFLAGS      = -I$(ROOTSYS)/include -I$(FASTJETDIR)/include -I$(PYTHIA8DIR)/include -I$(PYTHIA8DIR)/include/Pythia8/ -I$(STARPICOPATH)
INCFLAGS      += -I./src

ifeq ($(os),Linux)
CXXFLAGS      = 
else
CXXFLAGS      = -O -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
## for debugging:
# CXXFLAGS      = -g -O0 -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
endif

ifeq ($(os),Linux)
LDFLAGS       = -g
LDFLAGSS      = -g --shared 
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


ROOTLIBS      = $(shell root-config --libs)

LIBPATH       = $(ROOTLIBS) -L$(FASTJETDIR)/lib -L$(PYTHIA8DIR)/lib -L$(STARPICOPATH)
LIBS          = -lfastjet -lfastjettools -lpythia8  -llhapdfdummy -lTStarJetPico

LIBPATH       += -L./lib
LIBS	      += -lMyJetlib

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
	$(CXX) $(LDFLAGS) $(LIBPATH) $(LIBS) $^ -o $@

###############################################################################

###############################################################################
############################# Main Targets ####################################
###############################################################################
all    : $(BDIR)/PicoAj  $(BDIR)/ppInAuAuAj  $(BDIR)/ppInMcAj \
	 $(BDIR)/PythiaAj $(BDIR)/PythiaInAuAuAj $(BDIR)/PythiaInMcAj \
	 $(BDIR)/SimpleTree \
	 lib/libMyJetlib.a \
	lib/libMyJetlib.a \
	doxy


lib/libMyJetlib.a	: $(ODIR)/JetAnalyzer.o
	@echo 
	@echo MAKING LIBRARY
	ar -rcs $@ $^

$(ODIR)/JetAnalyzer.o 	: $(SDIR)/JetAnalyzer.cxx $(INCS)

#Aj
$(BDIR)/PicoAj		: $(ODIR)/PicoAj.o		$(ODIR)/AjAnalysis.o
$(BDIR)/ppInAuAuAj 	: $(ODIR)/ppInAuAuAj.o 		$(ODIR)/AjAnalysis.o
$(BDIR)/ppInMcAj	: $(ODIR)/ppInMcAj.o		$(ODIR)/AjAnalysis.o
$(BDIR)/PythiaAj	: $(ODIR)/PythiaAj.o 		$(ODIR)/AjAnalysis.o
$(BDIR)/PythiaInAuAuAj	: $(ODIR)/PythiaInAuAuAj.o 	$(ODIR)/AjAnalysis.o
$(BDIR)/PythiaInMcAj	: $(ODIR)/PythiaInMcAj.o 	$(ODIR)/AjAnalysis.o


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
	rm -vf $(BDIR)/*

.PHONY : clean doxy
