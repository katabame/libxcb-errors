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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SKIP 77

static int check_request(xcb_errors_context_t *ctx, uint8_t opcode, const char *expected)
{
	const char *actual = xcb_errors_get_name_for_major_code(ctx, opcode);
	if (strcmp(actual, expected) != 0) {
		fprintf(stderr, "For opcode %d: Expected %s, got %s\n", opcode,
				expected, actual);
		return 1;
	}
	return 0;
}

static int check_error(xcb_errors_context_t *ctx, uint8_t error, const char *expected)
{
	const char *actual = xcb_errors_get_name_for_error(ctx, error);
	if (strcmp(actual, expected) != 0) {
		fprintf(stderr, "For error %d: Expected %s, got %s\n", error,
				expected, actual);
		return 1;
	}
	return 0;
}

static int check_event(xcb_errors_context_t *ctx, uint8_t event, const char *expected)
{
	const char *actual = xcb_errors_get_name_for_event(ctx, event);
	if (strcmp(actual, expected) != 0) {
		fprintf(stderr, "For event %d: Expected %s, got %s\n", event,
				expected, actual);
		return 1;
	}
	return 0;
}

static int check_minor(xcb_errors_context_t *ctx, uint8_t major, uint16_t minor, const char *expected)
{
	const char *actual = xcb_errors_get_name_for_minor_code(ctx, major, minor);
	if (actual != expected && strcmp(actual, expected) != 0) {
		fprintf(stderr, "For minor (%d, %d): Expected %s, got %s\n",
				major, minor, expected, actual);
		return 1;
	}
	return 0;
}

static int test_error_connection(void)
{
	xcb_errors_context_t *ctx;
	xcb_connection_t *c;
	int err = 0;

	c = xcb_connect("does-not-exist", NULL);
	if (!xcb_connection_has_error(c)) {
		fprintf(stderr, "Failed to create an error connection\n");
		err = 1;
	}

	if (xcb_errors_context_new(c, &ctx) == 0) {
		fprintf(stderr, "Successfully created context for error connection\n");
		err = 1;
	}

	xcb_errors_context_free(ctx);
	xcb_disconnect(c);

	return err;
}

static int test_randr(xcb_connection_t *c, xcb_errors_context_t *ctx)
{
	struct xcb_query_extension_reply_t *reply;
	int err = 0;

	reply = xcb_query_extension_reply(c,
			xcb_query_extension(c, strlen("RANDR"), "RANDR"), NULL);
	if (!reply || !reply->present) {
		fprintf(stderr, "RANDR not supported by display\n");
		free(reply);
		return SKIP;
	}

	err |= check_request(ctx, reply->major_opcode, "RandR");
	err |= check_error(ctx, reply->first_error + 0, "BadOutput");
	err |= check_error(ctx, reply->first_error + 3, "BadProvider");
	err |= check_event(ctx, reply->first_event + 0, "ScreenChangeNotify");
	err |= check_event(ctx, reply->first_event + 1, "Notify");
	err |= check_minor(ctx, reply->major_opcode, 0, "QueryVersion");
	err |= check_minor(ctx, reply->major_opcode, 1, "Unknown (1)");
	err |= check_minor(ctx, reply->major_opcode, 33, "GetProviderInfo");
	err |= check_minor(ctx, reply->major_opcode, 41, "GetProviderProperty");
	err |= check_minor(ctx, reply->major_opcode, 1337, NULL);
	err |= check_minor(ctx, reply->major_opcode, 0xffff, NULL);

	free(reply);
	return err;
}

static int test_valid_connection(void)
{
	xcb_errors_context_t *ctx;
	xcb_connection_t *c;
	int err = 0;

	c = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(c)) {
		fprintf(stderr, "Failed to connect to X11 server (%d)\n",
				xcb_connection_has_error(c));
		return SKIP;
	}
	if (xcb_errors_context_new(c, &ctx) < 0) {
		fprintf(stderr, "Failed to initialize util-errors\n");
		xcb_disconnect(c);
		return 1;
	}

	err |= check_request(ctx, XCB_CREATE_WINDOW, "CreateWindow");
	err |= check_request(ctx, XCB_NO_OPERATION, "NoOperation");
	err |= check_request(ctx, 126, "Unknown (126)");
	err |= check_minor(ctx, XCB_CREATE_WINDOW, 0, NULL);
	err |= check_minor(ctx, XCB_CREATE_WINDOW, 42, NULL);
	err |= check_minor(ctx, XCB_CREATE_WINDOW, 0xffff, NULL);

	err |= check_error(ctx, XCB_REQUEST, "Request");
	err |= check_error(ctx, XCB_IMPLEMENTATION, "Implementation");
	err |= check_error(ctx, 18, "Unknown (18)");
	err |= check_error(ctx, 127, "Unknown (127)");

	err |= check_event(ctx, XCB_KEY_PRESS, "KeyPress");
	err |= check_event(ctx, XCB_KEY_RELEASE, "KeyRelease");
	err |= check_event(ctx, XCB_GE_GENERIC, "GeGeneric");
	err |= check_event(ctx, 36, "Unknown (36)");

	err |= test_randr(c, ctx);

	xcb_errors_context_free(ctx);
	xcb_disconnect(c);
	return err;
}

int main(void)
{
	int err = 0;
	err |= test_error_connection();
	err |= test_valid_connection();
	return err;
}
