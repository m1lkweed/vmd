CC     := gcc
CFLAGS += -Wall -Wextra -Werror -Wno-unused-result -pedantic

DEBUG_CFLAGS   ?= -Og -ggdb3 -fsanitize=address,undefined,leak 
RELEASE_CFLAGS ?= -O3 -flto

ifeq ($(DEBUG), 1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

.PHONY: clean all gdb qemu docker

vmd: example.c
	@$(CC) $(CFLAGS) -o $@ $<

gdb: vmd
	@printf $@": "
	@gdb -q -x gdbscript ./$<

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

all: vmd strace qemu docker gdb
	@printf $<": "
	@./$<

clean: vmd
	@-rm -vfr *~ $<
	@-docker rmi -f $<
