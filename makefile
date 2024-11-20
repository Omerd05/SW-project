CC = gcc
CFLAGS = -std=c90 -Wall -Wextra -Werror -pedantic-errors $(shell python3-config --includes)
LDFLAGS = -lm $(shell python3-config --ldflags)

TARGET = symnmf
SRCS = symnmf.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET) symnmfmodule

# Build the symnmf executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Build the Python module using setup.py
symnmfmodule:
	python3 setup.py build_ext --inplace

# Rules for object files
%.o: %.c symnmf.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build symnmfmodule.so
