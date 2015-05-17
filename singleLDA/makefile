GNU_CC = g++
GNU_CFLAGS = -O3 -march=core-avx2 -pthread -std=c++11 -c
GNU_LFLAGS = -O3 -march=core-avx2 -pthread -std=c++11

INTEL_CC = icc
INTEL_CFLAGS = -O2 -DNDEBUG -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2 -c
INTEL_LFLAGS = -O2 -DNDEBUG -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2
INTEL_TFLAGS = -fast -xCORE-AVX2 -DNDEBUG -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=3

ICL =  C:\"Program Files (x86)"\Intel\"Composer XE 2015"\bin\Intel64\icl.exe
ICL_FLAGS = /Qipo /Qinline-factor=500 /Qinline-max-size- /Qinline-max-total-size- /Quse-intel-optimized-headers /Qparallel /Qopt-prefetch:4 /W3 /O2 /Oi /Ot /Ob2 /GS /Gy /arch:AVX

all: intellinux

intellinux: CC=$(INTEL_CC)
intellinux: CFLAGS=$(INTEL_CFLAGS)
intellinux: LFLAGS=$(INTEL_LFLAGS)
intellinux: simpleLDA unifLDA sparseLDA aliasLDA FTreeLDA forestLDA lightLDA
 
gcc: CC=$(GNU_CC)
gcc: CFLAGS=$(GNU_CFLAGS)
gcc: LFLAGS=$(GNU_LFLAGS)
gcc: simpleLDA unifLDA sparseLDA aliasLDA FTreeLDA forestLDA lightLDA
 
intel:
	$(ICL) $(ICL_FLAGS) source0.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"simpleLDA.exe"
	$(ICL) $(ICL_FLAGS) source1.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"unifLDA.exe"
	$(ICL) $(ICL_FLAGS) source2.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"sparseLDA.exe"
	$(ICL) $(ICL_FLAGS) source3.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"aliasLDA.exe"
	$(ICL) $(ICL_FLAGS) source4.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"FTreeLDA.exe"
	$(ICL) $(ICL_FLAGS) source5.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"forestLDA.exe"
	$(ICL) $(ICL_FLAGS) source6.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"lightLDA.exe"

windows:
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source0.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"simpleLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source1.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"unifLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source2.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"sparseLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source3.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"aliasLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source4.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"FTreeLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source5.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"forestLDA.exe"
	CL /W3 /sdl- /O2 /Oi /Ot /Ob2 /MP /GS /Qpar /GL /EHsc /Gy source6.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp /link /OUT:"lightLDA.exe"	
	
simpleLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source0.o 
	$(CC) $(LFLAGS) source0.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "simpleLDA"

unifLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source1.o
	$(CC) $(LFLAGS) source1.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "unifLDA"

sparseLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source2.o
	$(CC) $(LFLAGS) source2.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "sparseLDA"

aliasLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source3.o
	$(CC) $(LFLAGS) source3.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "aliasLDA"
	
FTreeLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source4.o
	$(CC) $(LFLAGS) source4.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "FTreeLDA"

forestLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source5.o
	$(CC) $(LFLAGS) source5.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "forestLDA"
	
lightLDA : strtokenizer.o dataset.o fTree.o forest.o vose.o utils.o model.o lda.o source6.o
	$(CC) $(LFLAGS) source6.o lda.o model.o fTree.o forest.o vose.o utils.o strtokenizer.o dataset.o -o "lightLDA"

source0.o : source0.cpp lda.h
	$(CC) $(CFLAGS) source0.cpp
	
source1.o : source1.cpp lda.h
	$(CC) $(CFLAGS) source1.cpp
	
source2.o : source2.cpp lda.h
	$(CC) $(CFLAGS) source2.cpp
	
source3.o : source3.cpp lda.h
	$(CC) $(CFLAGS) source3.cpp

source4.o : source4.cpp lda.h
	$(CC) $(CFLAGS) source4.cpp

source5.o : source5.cpp lda.h
	$(CC) $(CFLAGS) source5.cpp
	
source6.o : source5.cpp lda.h
	$(CC) $(CFLAGS) source6.cpp

lda.o : model.h fTree.h forest.h vose.h utils.h constants.h lda.h lda.cpp
	$(CC) $(CFLAGS) lda.cpp

model.o : dataset.h utils.h model.h model.cpp
	$(CC) $(CFLAGS) model.cpp

fTree.o : fTree.h fTree.cpp utils.h
	$(CC) $(CFLAGS) fTree.cpp
	
forest.o : forest.h forest.cpp utils.h
	$(CC) $(CFLAGS) forest.cpp	

vose.o : vose.h vose.cpp utils.h
	$(CC) $(CFLAGS) vose.cpp

dataset.o : dataset.h dataset.cpp strtokenizer.h
	$(CC) $(CFLAGS) dataset.cpp

utils.o : utils.h utils.cpp strtokenizer.h
	$(CC) $(CFLAGS) utils.cpp
	
strtokenizer.o : strtokenizer.h strtokenizer.cpp
	$(CC) $(CFLAGS) strtokenizer.cpp

inteltogether:
	$(INTEL_CC) $(INTEL_TFLAGS) source0.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "simpleLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source1.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "unifLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source2.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "sparseLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source3.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "aliasLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source4.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "FTreeLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source5.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "forestLDA"
	$(INTEL_CC) $(INTEL_TFLAGS) source6.cpp fTree.cpp forest.cpp vose.cpp dataset.cpp lda.cpp model.cpp strtokenizer.cpp utils.cpp -o "lightLDA" 
	
bestlinux: CC=$(INTEL_CC)
bestlinux: CFLAGS=$(INTEL_CFLAGS)
bestlinux: LFLAGS=$(INTEL_LFLAGS)
bestlinux: inteltogether aliasLDA
	
clean:
	rm -rf *.o simpleLDA unifLDA sparseLDA aliasLDA FTreeLDA forestLDA lightLDA *.obj simpleLDA.exe unifLDA.exe sparseLDA.exe aliasLDA.exe FTreeLDA.exe forestLDA.exe lightLDA.exe
