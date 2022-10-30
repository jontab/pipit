CC      ?= gcc
ECHO    ?= echo
MKDIR   ?= mkdir
RM      ?= rm

BIN     ?= .
INC     ?= .
SRC     ?= .

EXE     ?= $(BIN)/pipit

CFLAGS  ?=
CFLAGS  :=

LDFLAGS ?=
LDFLAGS :=

INCS    := $(shell find $(INC) -name '*.h' | sort)
SRCS    := $(shell find $(SRC) -name '*.c' | sort)
OBJS    := $(patsubst %.c, %.o, $(SRCS))

$(EXE): $(OBJS)
	@$(ECHO) "$(CC)\t$@"
	@$(MKDIR) -p $(shell dirname $@)
	@$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c $(INCS)
	@$(ECHO) "$(CC)\t$@"
	@$(MKDIR) -p $(shell dirname $@)
	@$(CC) $< -o $@ -c $(CFLAGS)

clean:
	$(RM) -f $(EXE) $(OBJS)

.PHONY: clean
