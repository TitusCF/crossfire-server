# *****************************************************************************
# Configuration part. Edit those variables to fit your own system setup.
# *****************************************************************************

# This is the base crossfire data directory.
CROSSFIRE_PATH = /home/gros/games/share/crossfire

# Default libraries required. Edit those to match $(blddir)/Modules/Makefile
# if needed.
LIBS           = -lnsl -ldl -lpthread -lutil -lodbc

# *****************************************************************************
# End of user configuration part. Don't change anything after this line,
# unless things really go wrong with the default setup.
# *****************************************************************************

CC = gcc

SRCS    = plugin_logger.c arena.c
OBJECTS = plugin_logger.o arena.o
INCLUDE = -I./include -I../include -I/usr/include
CFLAGS  = $(INCLUDE) -Wall -g -fPIC

LDFLAGS = -Xlinker -export-dynamic
SYSLIBS = -lm
ALLLIBS = $(LIBS) $(SYSLIBS)

all : plugin_logger

clean :
	rm -rf *.o core plugin_logger.so.0.1

install :
	cp plugin_logger.so.0.1 $(CROSSFIRE_PATH)/plugins

plugin_logger : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,plugin_logger.so.0 $(LDFLAGS) $(OBJECTS) ../common/libcross.a $(ALLLIBS) -o plugin_logger.so.0.1

%.o : %.c
	$(CC) $(CFLAGS) -c $<

proto:
	cproto -D__CEXTRACT__ $(INCLUDE)  \
		$(SRCS)  >./include/logger_proto.h
	chmod 644 ./include/logger_proto.h

