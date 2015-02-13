OBJS := prime_path.o print.o
BIN := prime_path
CFLAGS := -Wall -std=c99 -g `sdl-config --cflags` -c
LFLAGS := -Wall -std=c99 -g `sdl-config --libs`

$(BIN) : $(OBJS)
	gcc $(LFLAGS) $^ -o $@

.PHONY : clean rebuild
clean :
	-rm $(BIN) $(OBJS)
rebuild : clean $(BIN)

prime_path.o : prime_path.c
	gcc $(CFLAGS) $< -o $@

print.o : print.c
	gcc $(CFLAGS) $< -o $@
