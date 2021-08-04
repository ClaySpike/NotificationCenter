#SRCS shows which c files to compile
SRCS := main.c

#OBJS shows which object files to compile
OBJS := $(SRCS:.c=.o)

#COMPILER_FLAGS shows which flags to add to the compiler
COMPILER_FLAGS := -g -Wall

#LINKER_FLAGS shows which libraries to link against
LINKER_FLAGS := -I include -lSDL2-2.0.0 -lSDL_ttf

#EXEC_FILE shows which name to make the executable
EXEC_FILE := main.out

#CC which compiler to use
CC := gcc

$(EXEC_FILE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LINKER_FLAGS)

%.o: %.c %.h
	$(CC) -c $< $(COMPILER_FLAGS) -o $@

clean:
	rm -f $(EXEC_FILE) $(OBJS)

all: $(EXEC_FILE)