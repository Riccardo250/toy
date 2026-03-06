objects = test.o parser.o lexer.o ast.o
flags = -MMD -MP
name = a.out

$(name) : $(objects)
	g++ $(debug_flags) -o $(name) $(objects)

debug : $(name)
debug : debug_flags += -ggdb

%.o : %.cpp 
	g++ $(debug_flags) $(flags) -c $< -o $@

-include $(objects:.o=.d)

clean:
	rm -f $(name) $(objects) $(objects:.o=.d)