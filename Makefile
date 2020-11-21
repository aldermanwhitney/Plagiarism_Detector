all: detector

detector: Asst22.c
	gcc -Wall -Werror -pthread -fsanitize=address Asst22.c -o detector

clean:
			rm -f detector *.o
