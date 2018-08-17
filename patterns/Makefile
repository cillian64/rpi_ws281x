main: dots all_on bounce scan bubbles

all_on: all_on.c
	gcc -o all_on -Wall -Werror all_on.c ../libws2811.a

dots: dots.c
	gcc -o dots -Wall -Werror dots.c ../libws2811.a

bounce: bounce.c
	gcc -o bounce -Wall -Werror bounce.c ../libws2811.a -lm

scan: scan.c
	gcc -o scan -Wall -Werror scan.c ../libws2811.a

bubbles: bubbles.c
	gcc -o bubbles -Wall -Werror bubbles.c ../libws2811.a
