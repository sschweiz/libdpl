lib_LTLIBRARIES = libdpl.la
libdpl_la_SOURCES = dpl_cache.c dpl_func.c dpl_thread.c dpl_extra.c dpl_omp.c
libdpl_la_CFLAGS = -Wall -Werror
libdpl_la_LDFLAGS = -version-info 1:0:0
libdpl_la_LIBADD = -lpthread
include_HEADERS = dpl.h
libdplincludedir = ${includedir}/dpl
libdplinclude_HEADERS = dpl_func.h dpl_cache.h dpl_types.h dpl_thread.h dpl_extra.h dpl_omp.h
