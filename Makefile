CC     := gcc
CFLAGS += -Wall -Wextra -Werror -pedantic -Wno-unused-result -pedantic

DEBUG_CFLAGS   ?= -Og -ggdb3 -fsanitize=address,undefined,leak 
RELEASE_CFLAGS ?= -O3 -flto

ifeq ($(DEBUG), 1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

.PHONY: clean all gdb qemu docker strace valgrind

vmd: example.c
	@$(CC) $(CFLAGS) -o $@ $<

gdb: vmd
	@printf $@": "
	@gdb -batch-silent -x gdbscript --args ./$< $(ARGS)

strace:vmd
	@printf $@": "
	@strace ./$< 2>/dev/null

qemu: vmd
	@printf $@": "
	@qemu-x86_64 -cpu max ./$<

docker: vmd
	@printf $@": "
	@docker build -qt $< .
	@docker run -it $<
	@-docker rmi -f $<

valgrind: vmd
	@printf $@": "
	@valgrind ./$< 2>/dev/null

all: vmd strace qemu docker gdb valgrind
	@printf $<": "
	@./$<

clean: vmd
	@-rm -vfr *~ $<
