C=g++ -g -Wall --std=c++11 -Werror -O3
E=.cpp
O=main.o
P=run.exec
SFML= -lsfml-graphics -lsfml-window -lsfml-system
$P:$O
	$C -o $P $O $(SFML)
$E.o:
	$C -c $<  -o $@
clean:
	rm $O $P
.PHONY: clean run
run: $P
	./$P
