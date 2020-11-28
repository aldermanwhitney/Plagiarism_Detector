all: detector

detector: Asst23.c
	gcc -Wall -Werror -pthread -fsanitize=address Asst23.c -lm -o detector

clean:
			rm -f detector *.o
