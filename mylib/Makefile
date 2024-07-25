# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Source files
SRCS = koto.c takema.c

# Object files
OBJS = $(SRCS:.c=.o)

# Static library name
LIB = mylib.a

# Targets
all: $(LIB)

# Create static library from object files
$(LIB): $(OBJS)
	ar rcs $@ $^

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f *.o $(LIB)

.PHONY: all clean
