CFLAGS += -Wall -Wextra -Werror -pedantic -Wno-unused-result -pedantic

DEBUG_CFLAGS   ?= -Og -ggdb3 -fsanitize=address,undefined,leak
RELEASE_CFLAGS ?= -O3 -flto

ifeq ($(DEBUG), 1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

.PHONY: clean test strace ltrace qemu docker gdb valgrind firejail standalone

STRACE_EXISTS := $(shell command -v strace 2> /dev/null)
LTRACE_EXISTS := $(shell command -v ltrace 2> /dev/null)
QEMU_EXISTS := $(shell command -v qemu-x86_64 2> /dev/null)
DOCKER_EXISTS := $(shell command -v docker 2> /dev/null)
GDB_EXISTS := $(shell command -v gdb 2> /dev/null)
VALGRIND_EXISTS := $(shell command -v valgrind 2> /dev/null)
FIREJAIL_EXISTS := $(shell command -v firejail 2> /dev/null)

vmd: example.c vmd.h syscall.h
	@$(CC) $(CFLAGS) -s -o $@ $<

strace:vmd
ifdef STRACE_EXISTS
	@printf $@": "
	@strace ./$< 2>/dev/null
else
	@printf "Skipping test, strace not found\n"
endif

ltrace:vmd
ifdef LTRACE_EXISTS
	@printf $@": "
	@ltrace ./$< 2>/dev/null
else
	@printf "Skipping test, ltrace not found\n"
endif

qemu: vmd
ifdef QEMU_EXISTS
	@printf $@": "
	@qemu-x86_64 -cpu max ./$<
else
	@printf "Skipping test, qemu not found\n"
endif

docker: vmd
ifdef DOCKER_EXISTS
	@printf $@": "
	@docker build -qt $< .>/dev/null
	@docker run -it $<
	@docker rm -fv $$(docker stop $$(docker ps -a|fgrep $<|cut -d' ' -f1))>/dev/null
	@-docker rmi -f $<>/dev/null
else
	@printf "Skipping test, docker not found\n"
endif

gdb: vmd
ifdef GDB_EXISTS
	@printf $@": "
	@gdb -batch-silent -x gdbscript --args ./$< $(ARGS)
else
	@printf "Skipping test, gdb not found\n"
endif

valgrind: vmd
ifdef VALGRIND_EXISTS
	@printf $@": "
	@valgrind ./$< 2>/dev/null
else
	@printf "Skipping test, valgrind not found\n"
endif

firejail: vmd
ifdef FIREJAIL_EXISTS
	@printf $@": "
	@firejail ./$< 2>/dev/null
else
	@printf "Skipping test, firejail not found\n"
endif

standalone: vmd
	@printf $@": "
	@./$<

test: vmd strace ltrace qemu docker gdb valgrind firejail standalone

clean: vmd
	@-rm -vfr *~ $<
