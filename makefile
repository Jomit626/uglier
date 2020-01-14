compiler : compiler.c ./utilities/utility.h ./parse/parser.c ./parse/parse_gen.c
	gcc -g compiler.c ./parse/*.c ./lex/*.c ./utilities/*.c -o compiler

target: compiler main.c data.c grammar
	./compiler < /dev/null
	gcc -g main.c ./parse/*.c ./lex/*.c ./utilities/*.c -o target
	./target > test.out

clean:
	rm ./compiler