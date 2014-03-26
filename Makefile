UNAME := $(shell uname | tr "A-Z" "a-z")

SRCS := main.c mersenne.c

EXTRA_FLAGS := -Wall -Wextra -std=c99

ifeq ($(UNAME),darwin)
    EXTRA_FLAGS := $(EXTRA_FLAGS) -DOS_DARWIN=1
endif
ifeq ($(UNAME),linux)
    EXTRA_FLAGS := $(EXTRA_FLAGS) -DOS_LINUX=1 -D_GNU_SOURCE
endif

.PHONY: all clean

all: mem-reordering mem-reordering-mfence

mem-reordering: $(SRCS)
	gcc $(EXTRA_FLAGS) -o $@ -O2 $^ -lpthread

mem-reordering-mfence: $(SRCS)
	gcc $(EXTRA_FLAGS) -DUSE_FENCE -o $@ -O2 $^ -lpthread

ifneq ($(UNAME),darwin)
all: mem-reordering-single

mem-reordering-single: $(SRCS)
	gcc $(EXTRA_FLAGS) -DUSE_SINGLE_CPU -o $@ -O2 $^ -lpthread
endif

clean:
	@rm -f mem-reordering mem-reordering-mfence mem-reordering-single
