compiler:
	bison -o grammar.cpp -d grammar.ypp
	flex -o lex.c lex.l
	g++ -o compiler main.cpp grammar.cpp compiler.cpp lex.c

clean:
	rm -f grammar.cpp grammar.hpp lex.c

cleanall: clean
	rm -f compiler
