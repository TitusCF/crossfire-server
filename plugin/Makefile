# *****************************************************************************
# Configuration part. Edit those variables to fit your own system setup.
# *****************************************************************************

# This is the base crossfire data directory.
CROSSFIRE_PATH = /home/gros/games/share/crossfire

# Default libraries required. Edit those to match $(blddir)/Modules/Makefile
# if needed.
LIBS           = -lnsl -ldl -lpthread -lutil

# Supply the Python version used. In most cases, this will be either 2.0 or 2.1

PYTHONVER      = 2.1

# Supply your library root directory. In case you have more than one (for
# example, /usr/lib and /usr/local/lib), choose the one containing Python.

LIBROOT        = /usr/lib

# Supply your include directory. In case you have more than one (for example,
# /usr/include and /usr/local/include), choose the one containing Python.

INCLUDEROOT    = /usr/include

# *****************************************************************************
# End of user configuration part. Don't change anything after this line,
# unless things really go wrong with the default setup.
# *****************************************************************************

OBJECTS   = plugin_python.o
INCLUDE   = -I./include -I../include -I$(INCLUDEROOT) -I$(INCLUDEROOT)/python$(PYTHONVER)

CC        = gcc
CFLAGS    = $(INCLUDE) -Wall -g -fPIC

LIBPYTHON = $(LIBROOT)/python$(PYTHONVER)/config/libpython$(PYTHONVER).a

LDFLAGS=	-Xlinker -export-dynamic -rdynamic -Xlinker -Bsymbolic
SYSLIBS=	-lm
ALLLIBS=	$(LIBPYTHON) $(LIBS) $(SYSLIBS)

all : plugin_python

clean :
	rm -rf *.o core plugin_python.so.0.1

install :
	cp plugin_python.so.0.1 $(CROSSFIRE_PATH)/plugins

plugin_python : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,plugin_python.so.0 $(LDFLAGS) $(OBJECTS) ../common/libcross.a $(ALLLIBS) -o plugin_python.so.0.1

plugin_python.o : plugin_python.c
	$(CC) $(CFLAGS) -c plugin_python.c
