noinst_PROGRAMS = perf_mapload perf_mapsave

AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\" \
	      -DPLUGIN_SUFFIX=\".so\"

SERVER_LDADD = ../../server/libserver.a ../../common/libcross.a ../../random_maps/librandom_map.a \
		../../socket/libsocket.a ../../types/libtypes.a

perf_mapload_LDADD = $(SERVER_LDADD)

perf_mapsave_LDADD = $(SERVER_LDADD)
