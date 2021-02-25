CC     := gcc
CFLAGS += -Wall -Wextra -Werror -Wno-unused-result -pedantic

DEBUG_CFLAGS   ?= -Og -ggdb3 -fsanitize=address,undefined,leak 
RELEASE_CFLAGS ?= -O3 -flto

ifeq ($(DEBUG), 1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

.PHONY: clean

vmd: example.c
	$(CC) $(CFLAGS) -o $@ $< 

clean:
	rm -vfr *~ vmd
