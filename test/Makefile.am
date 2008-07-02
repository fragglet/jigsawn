
if USE_VALGRIND
TESTS_ENVIRONMENT=./valgrind-wrapper
endif

TESTS =                          \
        test-utf8                

check_PROGRAMS=$(TESTS)

AM_CFLAGS = -I../src -Wall
LDADD = $(top_builddir)/src/libjigsawn.la

EXTRA_DIST=valgrind-wrapper

