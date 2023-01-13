
SOURCES=main.cpp Lexer.hpp Parser.hpp Sintax_graph.hpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ParaCL.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm -rf *.o *.exe