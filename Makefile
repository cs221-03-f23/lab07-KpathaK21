OBJS = lab07.o 
PROG = lab07

%.o : %.c
	gcc -c -g -o $@ $<

$(PROG) : $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(OBJS) $(PROG)
