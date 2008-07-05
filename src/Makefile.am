SUBDIRS=include

AM_CFLAGS=-Wall

lib_LTLIBRARIES=libjigsawn.la

libjigsawn_la_SOURCES=                                     \
	input-reader.c         input-reader.h              \
	lexer.c                lexer.h                     \
	parser.c               parser.h                    \
	utf8.c                 utf8.h                      \
	value.c                value.h                     \
	string-buffer.c        string-buffer.h             \
	value-array.c                                      \
	value-boolean.c                                    \
	value-float.c                                      \
	value-int.c                                        \
	value-mapping.c                                    \
	value-null.c                                       \
	value-object.c                                     \
	value-string.c

libjigsawn_la_CFLAGS=-Iinclude

