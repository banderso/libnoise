src = $(wildcard ./src/*.c)
obj = $(subst src,obj,$(src:.c=.o))

CFLAGS = -g -Wall -pedantic
LDFLAGS =

libnoise.a: obj $(obj)
	$(AR) rcs $@ $(obj)

obj:
	mkdir obj

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

.PHONY: clean print-% check-syntax
clean:
	rm -f $(obj) libnoise.a

print-%:
	@echo $*=$($*)

check-syntax:
	$(CC) -o /dev/null -S $(CHK_SOURCES)
