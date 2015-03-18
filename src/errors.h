#ifndef __ERRORS_H__
#define __ERRORS_H__

/* Copyright © 2015 Uli Schlachter
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or their
 * institutions shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the authors.
 */

#include "xcb_errors.h"

struct static_extension_info_t {
	uint16_t num_minor;
	uint8_t num_events;
	uint8_t num_errors;
	const char *strings_minor;
	const char *strings_events;
	const char *strings_errors;
};

extern const char unknown_major_code[];
extern const char unknown_error_code[];
extern const char unknown_event_code[];

const char *xproto_get_name_for_major_code(uint8_t major_code);
const char *xproto_get_name_for_event(uint8_t event_code);
const char *xproto_get_name_for_error(uint8_t error_code);
struct static_extension_info_t find_static_info_for_extension(const char *name);

#endif /* __ERRORS_H__ */
