#ifndef __XCB_ERRORS_H__
#define __XCB_ERRORS_H__

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

#include <xcb/xcb.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A context for using this library.
 *
 * Create a context with @ref xcb_errors_context_new () and destroy it with @ref
 * xcb_errors_context_free (). Except for @ref xcb_errors_context_free (), all
 * functions in this library are thread-safe and can be called from multiple
 * threads at the same time, even on the same context.
 */
typedef struct xcb_errors_context_t xcb_errors_context_t;

/**
 * Create a new @ref xcb_errors_context_t.
 *
 * @param conn A XCB connection which will be used until you destroy the context
 * with @ref xcb_errors_context_free ().
 * @param ctx A pointer to an xcb_cursor_context_t* which will be modified to
 * refer to the newly created context.
 * @return 0 on success, some other value otherwise.
 */
int xcb_errors_context_new(xcb_connection_t *conn, xcb_errors_context_t **ctx);

/**
 * Freed the @ref xcb_cursor_context_t.
 *
 * @param ctx The context to free.
 */
void xcb_errors_context_free(xcb_errors_context_t *ctx);

/**
 * Get the name corresponding to some major code.
 *
 * @param ctx An errors context, created with @ref xcb_errors_context_new ()
 * @param major_code The major code
 * @return A string allocated in static storage that contains a name for this
 * major code. This will never return NULL, but other functions in this library
 * may.
 */
const char *xcb_errors_get_name_for_major_code(xcb_errors_context_t *ctx,
		uint8_t major_code);

/**
 * Get the name corresponding to some minor code or NULL.
 *
 * @param ctx An errors context, created with @ref xcb_errors_context_new ()
 * @param major_code The major code under which to look up the minor code
 * @param major_code The minor code
 * @return A string allocated in static storage that contains a name for this
 * major code or NULL.
 */
const char *xcb_errors_get_name_for_minor_code(xcb_errors_context_t *ctx,
		uint8_t major_code,
		uint16_t minor_code);

/**
 * Get the name corresponding to some event.
 *
 * @param ctx An errors context, created with @ref xcb_errors_context_new ()
 * @param event_code The response_type of an event.
 * @return A string allocated in static storage that contains a name for this
 * major code. This will never return NULL, but other functions in this library
 * may.
 */
const char *xcb_errors_get_name_for_event(xcb_errors_context_t *ctx,
		uint8_t event_code);

/**
 * Get the name corresponding to some error.
 *
 * @param ctx An errors context, created with @ref xcb_errors_context_new ()
 * @param error_code The error_code of an error reply.
 * @return A string allocated in static storage that contains a name for this
 * major code. This will never return NULL, but other functions in this library
 * may.
 */
const char *xcb_errors_get_name_for_error(xcb_errors_context_t *ctx,
		uint8_t error_code);

#ifdef __cplusplus
}
#endif

#endif /* __XCB_ERRORS_H__ */
