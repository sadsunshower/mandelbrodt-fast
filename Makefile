build:
	gcc -Wall -std=c11 -O1 -mavx -o mandel mandel.c bitmap.c bitmap.h -lm

build-debug:
	gcc -Wall -std=c11 -g -mavx -o mandel mandel.c bitmap.c bitmap.h -lm

clean:
	rm mandel
