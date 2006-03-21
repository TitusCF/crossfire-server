BUILT_SOURCES=../../logs/unit/socket
noinst_LIBRARIES = libstubs.a
libstubs_a_SOURCES=stubs_socket.c
INCLUDES = -I../../../include -I../../include
AM_CPPFLAGS = -DRESSOURCES=\"$(srcdir)/../../resources\" -DLOGDIR=\"$(srcdir)/../../logs\"
LDADD = ../../../common/libcross.a  ../../../socket/libsocket.a libstubs.a $(LIBDL) @CHECK_LIBS@

../../logs/unit/socket:
	(mkdir ../../logs; mkdir ../../logs/unit; mkdir ../../logs/unit/socket)
../../../common/libcross.a:
	( cd ../../../common ; make libcross.a )
../../../socket/libsocket.a:
	( cd ../../../socket ; make libsocket.a )


TESTS= check_image check_info check_init check_item check_loop check_lowlevel check_metaserver check_request check_sounds

noinst_PROGRAMS=$(TESTS)
