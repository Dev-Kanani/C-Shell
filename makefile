# # Compiler and flags
# CC = gcc
# CFLAGS = -Wall -Wextra -g 

# # Source files
# SRCS = main.c queue.c reveal.c log.c proclore.c seek.c foreground.c parse_command.c

# # Object files (generated from source files)
# OBJS = $(SRCS:.c=.o)

# # Executable name
# TARGET = a

# # Default target to build the executable
# all: $(TARGET)

# # Rule to build the executable by linking object files
# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# # Rule to build object files from s	ource files
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # Clean up object files and the executable
# clean:
# 	rm -f $(OBJS) $(TARGET) $(SRCS:.c=.d)

# # Include dependency files (generated automatically by gcc)
# -include $(SRCS:.c=.d)

# # Rule to generate dependency files
# %.d: %.c
# 	@$(CC) $(CFLAGS) -MM $< > $@.tmp
# 	@sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@
# 	@rm -f $@.tmp

# # Phony targets (not real files)
# .PHONY: all clean


# Compiler and flags
CC = gcc
CFLAGS = -g -Wno-implicit-function-declaration

# Source files
SRCS = MP1/main.c MP1/queue.c MP1/reveal.c MP1/log.c MP1/proclore.c MP1/seek.c MP1/foreground.c MP1/parse_command.c

# Object files (generated from source files)
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = a.out

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable by linking object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET) $(SRCS:.c=.d)

# Include dependency files (generated automatically by gcc)
-include $(SRCS:.c=.d)

# Rule to generate dependency files
%.d: %.c
	@$(CC) $(CFLAGS) -MM $< > $@.tmp
	@sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.tmp > $@
	@rm -f $@.tmp

# Phony targets (not real files)
.PHONY: all clean
