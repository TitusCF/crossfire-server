noinst_LIBRARIES = libtypes.a
libtypes_a_SOURCES = book/book.c \
item_transformer/item_transformer.c \
power_crystal/power_crystal.c \
transport/transport.c \
trigger/trigger.c \
common/describe.c \
legacy/apply.c \
legacy/legacy_describe.c \
legacy/process.c

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

proto:
	cproto -D__CEXTRACT__ -o ../include/typesproto.h $(CPPFLAGS) $(AM_CPPFLAGS) \
		`echo $(SOURCES) " " | $(AWK) -v RS=" " '/.c$$/ { print }'`
	chmod 644 ../include/typesproto.h
