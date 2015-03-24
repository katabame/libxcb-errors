FLAGS=-std=gnu89 -Wall -Wextra -lxcb -fpic -shared -Wl,-no-undefined # -Wl,-export-symbols-regex,'^xcb_errors_'

all: src/libxcb_errors.so

src/libxcb_errors.so: $(wildcard src/*.c) $(wildcard *.h) src/extensions.c Makefile syms
	gcc $(FLAGS) -Wl,--retain-symbols-file=syms -o $@ $(wildcard src/*.c)

src/extensions.c: src/extensions.py
	PYTHONPATH=/home/psychon/projects/proto/ src/extensions.py $@ /home/psychon/projects/proto/src/*xml

syms:
	echo xcb_errors_context_new > $@
	echo xcb_errors_context_free > $@
	echo xcb_errors_get_name_for_major_code > $@
	echo xcb_errors_get_name_for_minor_code > $@
	echo xcb_errors_get_name_for_event > $@
	echo xcb_errors_get_name_for_error > $@
