# *****************************************************************************
# Configuration part. Edit those variables to fit your own system setup.
# *****************************************************************************

#######
# This is the base crossfire data directory.
CROSSFIRE_PATH = /home/gros/games/share/crossfire

#######
# This is the directory under which a crossfire/* directory will be created
# don't forget to edit
HTML_PATH =/var/www/html

# Default libraries required. Edit those to match $(blddir)/Modules/Makefile
# if needed.
LIBS           = -lnsl -ldl -lpthread -lutil -lodbc

# *****************************************************************************
# End of user configuration part. Don't change anything after this line,
# unless things really go wrong with the default setup.
# *****************************************************************************

CC = gcc

SRCS = plugin_logger.c arena.c maps.c creature.c
OBJECTS = plugin_logger.o arena.o maps.o creature.o
INCLUDE = -I./include -I../include -I/usr/include
CFLAGS  = $(INCLUDE) -Wall -g -fPIC

LDFLAGS = -Xlinker -export-dynamic -Xlinker -Bsymbolic
SYSLIBS = -lm
ALLLIBS = $(LIBS) $(SYSLIBS)

all : plugin_logger

clean :
	rm -rf *.o core plugin_logger.so

install :
	echo Installing plugin...
	cp plugin_logger.so $(CROSSFIRE_PATH)/lib/crossfire/plugins
	echo Updating battle arena...
	cp battle_arena/* $(CROSSFIRE_PATH)/share/crossfire/maps/city/misc/ -rf

html_install :
	@echo Copying html and pictures files
	cp html/* $(HTML_PATH)/ -rf
	@echo setting owner to apache.apache
#	chown apache.apache $(HTML_PATH)/crossfire/png/*
	find $(HTML_PATH)/crossfire/png -name '*.png' -exec chown apache.apache '{}' ';'
	chown apache.apache $(HTML_PATH)/crossfire/png
	chown apache.apache $(HTML_PATH)/crossfire/*.php
	chown apache.apache $(HTML_PATH)/crossfire/*.html
	@echo Changing access rights
	chmod a+rx $(HTML_PATH)/crossfire/screenshoot
	chmod a+rx $(HTML_PATH)/crossfire/inventoryshoot

plugin_logger : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,plugin_logger.so $(LDFLAGS) $(OBJECTS) ../common/libcross.a $(ALLLIBS) -o plugin_logger.so

%.o : %.c
	$(CC) $(CFLAGS) -c $<

proto:
	cproto -D__CEXTRACT__ $(INCLUDE)  \
		$(SRCS)  >./include/logger_proto.h
	chmod 644 ./include/logger_proto.h

pictures:
	echo 'finding archetypes picture in ../lib/arch/ ....'
	find ../lib/arch/* -name '*.png' -exec cp -f '{}' html/crossfire/png ';'
