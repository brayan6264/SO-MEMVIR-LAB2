CC = gcc
CFLAGS = -Wall -Werror -std=c99 -Iinclude
TARGET = memsim

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
	EXE = .exe
	RM = del /Q
	CLEAN_FILES = $(subst /,\,$(OBJS)) $(TARGET)$(EXE)
else
	EXE =
	RM = rm -f
	CLEAN_FILES = $(OBJS) $(TARGET)$(EXE)
endif

.PHONY: all clean run

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)$(EXE)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(CLEAN_FILES)

run: $(TARGET)$(EXE)
	./$(TARGET)$(EXE) $(ARGS)