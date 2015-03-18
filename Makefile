FLAGS=-std=gnu89 -Wall -Wextra -lxcb -fpic -shared -Wl,-no-undefined # -Wl,-export-symbols-regex,'^xcb_errors_'

src/libxcb_errors.so: $(wildcard src/*.c) $(wildcard *.h) Makefile src/static_tables.inc syms
	gcc $(FLAGS) -Wl,--retain-symbols-file=syms -o $@ $(wildcard src/*.c)

syms:
	echo xcb_errors_context_new > $@
	echo xcb_errors_context_free > $@
	echo xcb_errors_get_name_for_major_code > $@
	echo xcb_errors_get_name_for_minor_code > $@
	echo xcb_errors_get_name_for_event > $@
	echo xcb_errors_get_name_for_error > $@

src/static_tables.inc:
	for x in $$(seq 0 255) ; do echo "ENTRY($$x)" ; done > $@
