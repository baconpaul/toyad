run:	toyad
	./toyad

OBJS=toyad.o

%.o:	%.cpp Node.h
	clang -g -std=c++14 -Wall -Wextra -Werror -c $< -o $@

toyad:	$(OBJS)
	clang -g -std=c++14 -lstdc++ -o toyad toyad.cpp
