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
#include "errors.h"
#include <stdlib.h>
#include <string.h>

#define IS_IN_RANGE(value, begin, length) ((value) >= (begin) && (value) < (begin) + (length))

struct extension_info_t {
	struct extension_info_t *next;
	struct static_extension_info_t static_info;
	uint8_t major_opcode;
	uint8_t first_event;
	uint8_t first_error;
	char *name;
};

struct xcb_errors_context_t {
	struct extension_info_t *extensions;
};

static const char *get_strings_entry(const char *strings, unsigned int index) {
	while (index > 0)
		strings += strlen(strings) + 1;
	return strings;
}

static int register_extension(xcb_errors_context_t *ctx, xcb_connection_t *conn, const char *ext)
{
	struct extension_info_t *info;
	struct static_extension_info_t static_info;
	xcb_query_extension_reply_t *reply;
	char *ext_dup;

	ext_dup = strdup(ext);
	info = calloc(1, sizeof(*info));
	reply = xcb_query_extension_reply(conn,
			xcb_query_extension(conn, strlen(ext), ext), NULL);
	static_info = find_static_info_for_extension(ext);

	if (!info || !reply || !ext_dup || !reply->present || (static_info.num_minor == 0)) {
		free(info);
		free(reply);
		free(ext_dup);
		/* This is used to indicate an unsupported extension */
		if (static_info.num_minor == 0)
			return 0;
		return -1;
	}

	info->name = ext_dup;
	info->static_info = static_info;
	info->major_opcode = reply->major_opcode;
	info->first_event = reply->first_event;
	info->first_error = reply->first_error;

	info->next = ctx->extensions;
	ctx->extensions = info;
	free(reply);

	return 0;
}

int xcb_errors_context_new(xcb_connection_t *conn, xcb_errors_context_t **c)
{
	xcb_errors_context_t *ctx = NULL;
	xcb_list_extensions_reply_t *reply = NULL;
	xcb_str_iterator_t iter;

	if ((*c = calloc(1, sizeof(*c))) == NULL)
		goto error_out;

	ctx = *c;
	ctx->extensions = NULL;

	reply = xcb_list_extensions_reply(conn,
			xcb_list_extensions(conn), NULL);
	if (!reply)
		goto error_out;

	iter = xcb_list_extensions_names_iterator(reply);
	while (iter.rem > 0) {
		int status = register_extension(ctx, conn, xcb_str_name(iter.data));
		if (status < 0)
			goto error_out;
		xcb_str_next(&iter);
	}

	free(reply);
	return 0;

error_out:
	free(reply);
	xcb_errors_context_free(ctx);
	*c = NULL;
	return -1;
}

void xcb_errors_context_free(xcb_errors_context_t *ctx)
{
	struct extension_info_t *info;

	if (ctx == NULL)
		return;

	info = ctx->extensions;
	while (info) {
		struct extension_info_t *prev = info;
		info = info->next;
		free(prev->name);
		free(prev);
	}

	free(ctx);
}

const char *xcb_errors_get_name_for_major_code(xcb_errors_context_t *ctx,
		uint8_t major_code)
{
	struct extension_info_t *info = ctx->extensions;
	const char *result = xproto_get_name_for_major_code(major_code);
	if (result)
		return result;

	while (info && info->major_opcode != major_code)
		info = info->next;

	if (info == NULL)
		return unknown_major_code[major_code];

	return info->name;
}

const char *xcb_errors_get_name_for_minor_code(xcb_errors_context_t *ctx,
		uint8_t major_code,
		uint16_t minor_code)
{
	struct extension_info_t *info = ctx->extensions;

	while (info && info->major_opcode != major_code)
		info = info->next;

	if (info == NULL || minor_code >= info->static_info.num_minor)
		return NULL;

	return get_strings_entry(info->static_info.strings_minor, minor_code);
}

const char *xcb_errors_get_name_for_event(xcb_errors_context_t *ctx,
		uint8_t event_code)
{
	struct extension_info_t *info = ctx->extensions;
	const char *result = xproto_get_name_for_event(event_code);
	if (result)
		return result;

	while (info && !IS_IN_RANGE(event_code, info->first_event, info->static_info.num_events))
		info = info->next;

	if (info == NULL)
		return unknown_event_code[event_code];

	return get_strings_entry(info->static_info.strings_events, event_code - info->first_event);
}

const char *xcb_errors_get_name_for_error(xcb_errors_context_t *ctx,
		uint8_t error_code)
{
	struct extension_info_t *info = ctx->extensions;
	const char *result = xproto_get_name_for_error(error_code);
	if (result)
		return result;

	while (info && !IS_IN_RANGE(error_code, info->first_error, info->static_info.num_errors))
		info = info->next;

	if (info == NULL)
		return unknown_error_code[error_code];

	return get_strings_entry(info->static_info.strings_errors, error_code - info->first_error);
}
