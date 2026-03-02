objects = test.o parser.o lexer.o ast.o
flags = -MMD -MP
name = a.out

$(name) : $(objects)
	g++ -o $(name) $(objects)

%.o : %.cpp 
	g++ $(flags) -c $< -o $@

-include $(objects:.o=.d)

clean:
	rm -f $(name) $(objects) $(objects:.o=.d)