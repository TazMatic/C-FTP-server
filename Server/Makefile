CFLAGS += -std=c11 -pthread -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -D_DEFAULT_SOURCE
LDFLAGS += -pthread
LDLIBS += -lcrypto -lssl

PROG1 = server

all: $(PROG1)

$(PROG1): src/$(PROG1)
	mv src/$(PROG1) .

src/$(PROG1): src/$(PROG1).o src/Database.o

.PHONY: clean debug $(PROG1)all check profile testall

check: $(PROG1)
check: test/testAll
	./test/testAll

TESTOBJS = $(subst .c,.o,$(wildcard test/test*.c))
test/testAll: $(TESTOBJS) src/Database.o
test/testAll: CFLAGS += -I src -g
test/testAll: LDFLAGS += -L src
test/testAll: LDLIBS += -lcunit


debug: CFLAGS += -g
debug: all
	mv $(PROG1) $(PROG1)_debug


profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: all
	mv $(PROG1) $(PROG1)_profile

clean:
	$(RM) src/*.[io] test/*.[io] $(PROG1)* test/testAll gmon.out src/Database
