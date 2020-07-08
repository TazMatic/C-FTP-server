CFLAGS += -std=c11 -pthread -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -D_DEFAULT_SOURCE
LDFLAGS += -pthread

PROG1 = dispatcher
PROG2 = listener

all: $(PROG1) $(PROG2)

$(PROG1): src/$(PROG1)
	mv src/$(PROG1) .

$(PROG2): src/$(PROG2)
	mv src/$(PROG2) .

src/$(PROG1): src/$(PROG1).o src/utils.o src/sig_handle.o
src/$(PROG2): src/$(PROG2).o src/utils.o src/sig_handle.o

.PHONY: clean debug $(PROG1) $(PROG2) all check profile

check: test/testAll
	./$^

TESTOBJS = $(subst .c,.o,$(wildcard test/test*.c))
test/testAll: $(TESTOBJS)
test/testAll: CFLAGS += -g -I./src
test/testAll: LDFLAGS += -L./src
test/testAll: LDLIBS += -lcunit


debug: CFLAGS += -g
debug: all
	mv $(PROG1) $(PROG1)_debug
	mv $(PROG2) $(PROG2)_debug


profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: all
	mv $(PROG1) $(PROG1)_profile
	mv $(PROG2) $(PROG2)_profile

clean:
	$(RM) src/*.[io] test/*.[io] $(PROG1)* $(PROG2)* test/testAll gmon.out
