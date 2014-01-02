CC := gcc -Wall -g -march=native
CFLAGS := -c
SRCDIR := src
OBJDIR := objs
INCLUDES := -Iinclude 
LIBS := -lmpfr -lgmp -lm
DEFINES = -DUSE_MPFR

SOURCES := 	  $(SRCDIR)/termtree.c \
		  $(SRCDIR)/string_allocator.c \
		  $(SRCDIR)/tables.c \
		  $(SRCDIR)/commands.c \
		  $(SRCDIR)/functions.c \
		  $(SRCDIR)/string_manip.c \
		  $(SRCDIR)/fp_t.c \
		  $(SRCDIR)/wlist.c \
		  $(SRCDIR)/rl_emul.c \
		  $(SRCDIR)/ud_constants_tree.c 

OBJECTS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

BINARIES=calc result_test

all: $(BINARIES)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEFINES) $< -o $@ $(INCLUDES)

calc: $(OBJECTS) $(SOURCES) $(SRCDIR)/calc.c
	$(CC) $(OBJECTS) $(DEFINES) $(SRCDIR)/calc.c -o $@ $(INCLUDES) $(LIBS)

result_test: $(SOURCES) $(SRCDIR)/test/result_test.c
	$(CC) $(OBJECTS) $(DEFINES) $(SRCDIR)/test/result_test.c -o $@ $(INCLUDES) $(LIBS)
	
clean:
	rm -fR $(OBJDIR) $(BINARIES)

install:
	install -d /usr/local/bin
	install -m 755 calc /usr/local/bin

uninstall:
	rm -f /usr/local/bin/calc
