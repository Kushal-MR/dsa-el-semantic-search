# C engine build. Works with gcc on Linux, WSL, and MSYS2 MinGW on Windows.
CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2
BUILD   := build

ifeq ($(OS),Windows_NT)
  EXE := .exe
else
  EXE :=
endif

.PHONY: all check clean

all: check

# Step 1 setup check: proves the compiler and flags work on this machine.
check: $(BUILD)/test_setup$(EXE)
	./$(BUILD)/test_setup$(EXE)

$(BUILD)/test_setup$(EXE): tests/test_setup.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@ -lm

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
