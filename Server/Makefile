CFLAGS += -std=c11 -pthread -Wall -Wextra -Wpedantic -Waggregate-return -Wwrite-strings -Wvla -Wfloat-equal -D_DEFAULT_SOURCE
LDFLAGS += -pthread
LDLIBS += -lcrypto -lssl

PROG1 = server

all: $(PROG1)

$(PROG1): src/$(PROG1)
	mv src/$(PROG1) .

src/$(PROG1): src/$(PROG1).o src/Database.o

.PHONY: clean debug $(PROG1)all check profile

check:
	test/test.sh

debug: CFLAGS += -g
debug: all
	mv $(PROG1) $(PROG1)_debug


profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: all
	mv $(PROG1) $(PROG1)_profile

clean:
	$(RM) src/*.[io] test/*.[io] $(PROG1)* test/testAll gmon.out src/Database
