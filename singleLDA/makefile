CDIR = ../commons

GNU_CC = g++
GNU_CFLAGS = -I"$(CDIR)" -O3 -march=core-avx2 -pthread -std=c++11 -c
GNU_LFLAGS = -I"$(CDIR)" -O3 -march=core-avx2 -pthread -std=c++11

INTEL_CC = icc
INTEL_CFLAGS = -I"$(CDIR)" -O2 -DNDEBUG -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2 -c
INTEL_LFLAGS = -I"$(CDIR)" -O2 -DNDEBUG -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2
INTEL_TFLAGS = -I"$(CDIR)" -fast -xCORE-AVX2 -DNDEBUG -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=3

ICL =  C:\"Program Files (x86)"\Intel\"Composer XE 2015"\bin\Intel64\icl.exe
ICL_FLAGS = /Qipo /I"$(CDIR)" /Qinline-factor=500 /Qinline-max-size- /Qinline-max-total-size- /Quse-intel-optimized-headers /Qparallel /Qopt-prefetch:4 /W3 /O2 /Oi /Ot /Ob2 /GS /Gy /arch:AVX

all: intellinux

intellinux: CC=$(INTEL_CC)
intellinux: CFLAGS=$(INTEL_CFLAGS)
intellinux: LFLAGS=$(INTEL_LFLAGS)
intellinux: LDA
 
gcc: CC=$(GNU_CC)
gcc: CFLAGS=$(GNU_CFLAGS)
gcc: LFLAGS=$(GNU_LFLAGS)
gcc: LDA
 
intelwindows:
	$(ICL) $(ICL_FLAGS) source.cpp lda.cpp model.cpp $(CDIR)/fTree.cpp $(CDIR)/forest.cpp $(CDIR)/vose.cpp $(CDIR)/dataset.cpp $(CDIR)/stringtokenizer.cpp $(CDIR)/utils.cpp /link /OUT:"LDA.exe"

visualstudio:
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy /I"$(CDIR)" source.cpp lda.cpp model.cpp $(CDIR)/fTree.cpp $(CDIR)/forest.cpp $(CDIR)/vose.cpp $(CDIR)/dataset.cpp $(CDIR)/stringtokenizer.cpp $(CDIR)/utils.cpp /link /OUT:"LDA.exe"
	
LDA : stringtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source.o 
	$(CC) $(LFLAGS) source.o lda.o model.o fTree.o forest.o vose.o utils.o stringtokenizer.o dataset.o -o "LDA"

source.o : source.cpp lda.h
	$(CC) $(CFLAGS) source.cpp

lda.o : lda.h lda.cpp model.h $(CDIR)/fTree.h $(CDIR)/forest.h $(CDIR)/vose.h $(CDIR)/utils.h
	$(CC) $(CFLAGS) lda.cpp

model.o : model.h model.cpp $(CDIR)/dataset.h $(CDIR)/utils.h
	$(CC) $(CFLAGS) model.cpp

fTree.o : $(CDIR)/fTree.h $(CDIR)/fTree.cpp $(CDIR)/utils.h
	$(CC) $(CFLAGS) $(CDIR)/fTree.cpp
	
forest.o : $(CDIR)/forest.h $(CDIR)/forest.cpp $(CDIR)/utils.h
	$(CC) $(CFLAGS) $(CDIR)/forest.cpp	

vose.o : $(CDIR)/vose.h $(CDIR)/vose.cpp $(CDIR)/utils.h
	$(CC) $(CFLAGS) $(CDIR)/vose.cpp

dataset.o : $(CDIR)/dataset.h $(CDIR)/dataset.cpp $(CDIR)/stringtokenizer.h
	$(CC) $(CFLAGS) $(CDIR)/dataset.cpp

utils.o : $(CDIR)/utils.h $(CDIR)/utils.cpp $(CDIR)/stringtokenizer.h
	$(CC) $(CFLAGS) $(CDIR)/utils.cpp
	
stringtokenizer.o : $(CDIR)/stringtokenizer.h $(CDIR)/stringtokenizer.cpp
	$(CC) $(CFLAGS) $(CDIR)/stringtokenizer.cpp

inteltogether:
	$(INTEL_CC) $(INTEL_TFLAGS) source.cpp lda.cpp model.cpp $(CDIR)/fTree.cpp $(CDIR)/forest.cpp $(CDIR)/vose.cpp $(CDIR)/dataset.cpp $(CDIR)/stringtokenizer.cpp $(CDIR)/utils.cpp -o "LDA"
	
clean:
	rm -rf *.o LDA *.obj LDA.exe
