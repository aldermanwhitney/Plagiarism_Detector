all: detector

detector: Asst2.c
	gcc -Wall -Werror -fsanitize=address Asst2.c -o detector

clean:
			rm -f detector *.o
