
Inc_Dir = .././include
DEFDIR = .././definitions
СXX?= g++
CXXFLAGS?= -Wall -std=c++2a -g -I $(Inc_Dir)


export CXX CXXFLAGS DEFDIR


.PHONY: clean


subsystem:
	cd ParaCL && $(MAKE)
	cd Tests && $(MAKE)

clean:
	cd ParaCL && $(MAKE) $@
	cd Tests && $(MAKE) $@
