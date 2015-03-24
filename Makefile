FLAGS=-std=gnu89 -Wall -Wextra -ggdb3
SHARED_FLAGS=$(FLAGS) -lxcb -fpic -shared -Wl,-no-undefined # -Wl,-export-symbols-regex,'^xcb_errors_'
TEST_FLAGS=$(FLAGS) -Isrc

.PHONY: check

all: src/libxcb-errors.so check

check: tests/test
	LD_LIBRARY_PATH=src tests/test

src/libxcb-errors.so: $(wildcard src/*.c) $(wildcard *.h) src/extensions.c Makefile syms
	gcc $(SHARED_FLAGS) -Wl,--retain-symbols-file=syms -o $@ $(wildcard src/*.c)

src/extensions.c: src/extensions.py
	PYTHONPATH=/home/psychon/projects/proto/ src/extensions.py $@ /home/psychon/projects/proto/src/*xml

tests/test: tests/test.c src/libxcb-errors.so
	gcc $(TEST_FLAGS) -lxcb -Lsrc -lxcb-errors -o $@ $<

syms:
	echo xcb_errors_context_new > $@
	echo xcb_errors_context_free > $@
	echo xcb_errors_get_name_for_major_code > $@
	echo xcb_errors_get_name_for_minor_code > $@
	echo xcb_errors_get_name_for_event > $@
	echo xcb_errors_get_name_for_error > $@
