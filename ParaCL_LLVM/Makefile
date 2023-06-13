
Inc_Dir = .././include -I /usr/lib/llvm-13/include
DEFDIR = $(PWD)/definitions
DEFINS = $(shell find $(DEFDIR) -name '*.cpp')
СXX?= g++
CXXFLAGS?= -g -I $(Inc_Dir)

export CXX CXXFLAGS DEFINS


.PHONY: clean


subsystem:
	cd ParaCL && $(MAKE)
	

clean:
	cd ParaCL && $(MAKE) $@
	
