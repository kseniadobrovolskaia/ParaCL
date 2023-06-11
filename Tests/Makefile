
SOURCES=ParaCL_Test.cpp
DEFINS = $(shell find $(SOURCEDIR) -name '*.cpp')
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ParaCL_Test.exe
Definitions = .././definitions/Global_vars.cpp .././definitions/Lexer.cpp .././definitions/Lex_methods.cpp .././definitions/Scope_table.cpp .././definitions/Throw_exception.cpp .././definitions/Parser_arith.cpp .././definitions/Stmt_methods.cpp .././definitions/Parser_stmts.cpp .././definitions/Sintax_graph.cpp

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(Definitions) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm -rf *.o *.exe

