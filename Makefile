
Inc_Dir = .././include
СXX?= g++
CXXFLAGS?= -Wall -std=c++2a -g -I $(Inc_Dir)

export CXX CXXFLAGS


.PHONY: clean


subsystem:
	cd ParaCL && $(MAKE)
	cd Tests && $(MAKE)

clean:
	cd ParaCL && $(MAKE) $@
	cd Tests && $(MAKE) $@