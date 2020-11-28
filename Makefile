all: detector

detector: Asst2.c
	gcc -Wall -Werror -pthread -fsanitize=address Asst2.c -lm -o detector

clean:
			rm -f detector *.o
