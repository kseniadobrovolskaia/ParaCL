
Inc_Dir = .././include
DEFDIR = $(PWD)/definitions
DEFINS = $(shell find $(DEFDIR) -name '*.cpp')
СXX?= g++
CXXFLAGS?= -Wall -std=c++2a -g -I $(Inc_Dir)

export CXX CXXFLAGS DEFINS


.PHONY: clean


subsystem:
	cd ParaCL && $(MAKE)
	cd Tests && $(MAKE)

clean:
	cd ParaCL && $(MAKE) $@
	cd Tests && $(MAKE) $@
