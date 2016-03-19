CDIR = ../commons

DEBUG = -DNDEBUG

INTEL_CC = icc
INTEL_CFLAGS = $(DEBUG) -I"$(CDIR)" -O3 -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2 -pthread -c
INTEL_LFLAGS = $(DEBUG) -I"$(CDIR)" -O3 -march=core-avx2 -std=c++11 -inline-factor=500 -no-inline-max-size -no-inline-max-total-size -use-intel-optimized-headers -parallel -opt-prefetch=4 -opt-mem-layout-trans=2 -pthread

GNU_CC = g++
GNU_CFLAGS = $(DEBUG) -I"$(CDIR)" -O3 -march=corei7 -pthread -std=c++11 -c
GNU_LFLAGS = $(DEBUG) -I"$(CDIR)" -O3 -march=corei7 -pthread -std=c++11

CC = $(GNU_CC)
CFLAGS = $(GNU_CFLAGS)
LFLAGS = $(GNU_LFLAGS)

SOURCEDIR = .
BUILDDIR = ../../build
EXECUTABLE = ../../dist/analyser

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

COMMONS = $(wildcard $(SOURCEDIR)/../commons/*.cpp)
COBJECT = $(patsubst $(SOURCEDIR)/../commons/%.cpp,$(BUILDDIR)/%.o,$(COMMONS))

all: $(EXECUTABLE)

gcc: $(EXECUTABLE)

intel: CC=$(INTEL_CC)
intel: CFLAGS=$(INTEL_CFLAGS)
intel: LFLAGS=$(INTEL_LFLAGS)
intel: $(EXECUTABLE)

$(EXECUTABLE): $(COBJECT)  $(OBJECTS)
	$(CC) $(LFLAGS) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(COBJECT): $(BUILDDIR)/%.o : $(SOURCEDIR)/../commons/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)/*
	rm -rf $(EXECUTABLE)
