CC = gcc
CFLAGS = -Wall -Werror -std=c99 -Iinclude
TARGET = memsim
OBJDIR = obj
SRCDIRS = src src/domain src/application src/infrastructure

SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))
vpath %.c $(SRCDIRS)

ifeq ($(OS),Windows_NT)
	EXE = .exe
	MKDIR = if not exist $(OBJDIR) mkdir $(OBJDIR)
	CLEAN = if exist $(OBJDIR) rmdir /S /Q $(OBJDIR) & if exist $(TARGET)$(EXE) del /Q $(TARGET)$(EXE)
else
	EXE =
	MKDIR = mkdir -p $(OBJDIR)
	CLEAN = rm -rf $(OBJDIR) $(TARGET)$(EXE)
endif

.PHONY: all clean run

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)$(EXE)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	$(MKDIR)

clean:
	-$(CLEAN)

run: $(TARGET)$(EXE)
	./$(TARGET)$(EXE) $(ARGS)
