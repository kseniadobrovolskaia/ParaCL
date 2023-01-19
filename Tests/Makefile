
SOURCES=ParaCL_Test.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ParaCL_Test.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm -rf *.o *.exe

