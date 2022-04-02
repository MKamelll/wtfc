FLAGS=-Wall -Wextra -pedantic

main: wtf.c
	mkdir -p build
	$(CC) ${FLAGS} -o build/$@ $^
