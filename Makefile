# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -I./mylib

# Directories
SRCDIR = .
OBJDIR = obj
LIBDIR = mylib

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(LIBDIR)/*.c)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(filter %.c, $(SRCS)))

# Executable names
EXEC = server client

# Targets
all: $(EXEC)

# Create executable for server
server: $(filter-out $(OBJDIR)/client.o, $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^

# Create executable for client
client: $(filter-out $(OBJDIR)/server.o, $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(LIBDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up build files
clean:
	rm -rf $(OBJDIR) $(EXEC)

.PHONY: all clean
