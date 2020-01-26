compiler : compiler.c ./utilities/utility.h ./parse/parser.c ./parse/parse_gen.c ./utilities/*.c
	gcc  -DDEBUG -g compiler.c ./parse/*.c ./lex/*.c ./utilities/*.c -o compiler

target: compiler main.c data.c mini_grammar ./AST/AST.c
	./compiler mini_grammar
	gcc -g main.c ./parse/*.c ./lex/*.c ./utilities/*.c  ./AST/*.c -o target
	

clean:
	rm ./compiler
