
SOURCES=main.cpp
DEFINS = $(shell find $(DEFDIR) -name '*.cpp')
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ParaCL.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(DEFINS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm -rf *.o *.exe