# *****************************************************************************
# Configuration part. Edit those variables to fit your own system setup.
# *****************************************************************************

# This is the base crossfire data directory.
CROSSFIRE_PATH = /home/tchize/develop/share/crossfire

# Default libraries required. Edit those to match $(blddir)/Modules/Makefile
# if needed.
LIBS           = -lnsl -ldl -lpthread -lutil

# Supply your library root directory.

LIBROOT        = /usr/lib

# Supply your include directory.

INCLUDEROOT    = /usr/include

# *****************************************************************************
# End of user configuration part. Don't change anything after this line,
# unless things really go wrong with the default setup.
# *****************************************************************************

OBJECTS   = plugin_animator.o animator_box.o
INCLUDE   = -I./include -I../include -I$(INCLUDEROOT)

CC        = gcc
CFLAGS    = $(INCLUDE) -Wall -g -fPIC

LDFLAGS=	-Xlinker -export-dynamic -rdynamic
SYSLIBS=	-lm
ALLLIBS=	$(LIBS) $(SYSLIBS)

all : plugin_animator

clean :
	rm -rf *.o core plugin_generic.so.0.1

install :
	cp plugin_animator.so.0.1 $(CROSSFIRE_PATH)/plugins

plugin_animator : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,plugin_animator.so.0 $(LDFLAGS) $(OBJECTS) ../common/libcross.a $(ALLLIBS) -o plugin_animator.so.0.1

%.o : %.c
	$(CC) $(CFLAGS) -c $<
