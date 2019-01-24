TAGET:=bin/Exp_Word.out
OBJ:=obj/sql.o
OBJ+=main.c

F:=-Wall -g
CC:=gcc

$(TAGET):$(OBJ)
	$(CC) -o $@ -I./include $^ $(F) -lsqlite3 -lpthread
obj/%.o:src/%.c
	$(CC) -o $@ -I./include -c $^ $(F)

.PHONEY:clean
clean:
		rm -rf $(TAGET)
		rm -rf obj/*.o
		rm -rf *~ *.*~
		rm -rf include/*.*~
		rm -rf src/*.*~

