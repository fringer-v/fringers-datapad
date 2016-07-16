/* Copyright (c) 2008 PrimeBase Technologies GmbH, Germany
 *
 * PrimeBase Media Stream for MySQL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Original author: Paul McCullagh (H&G2JCtL)
 * Continued development: Barry Leslie
 *
 * 2007-05-21
 *
 * CORE SYSTEM:
 * Simple utility functions.
 *
 */

#include <inttypes.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef OS_WINDOWS
#include <fnmatch.h>
#endif

#include "StrUtil.h"

#define YV_DIR_CHAR		'/'
#define IS_DIR_CHAR(x)	((x) == YV_DIR_CHAR)

void yv_strcpy(size_t size, char *to, const char *from, size_t len)
{
	if (size > 0) {
		size--;
		if (len > size)
			len = size;
		memcpy(to, from, len);
		to[len] = 0;
	}
}

void yv_strcpy(size_t size, char *to, const char *from)
{
	if (size > 0) {
		size--;
		while (*from && size--)
			*to++ = *from++;
		*to = 0;
	}
}

void yv_strmove(size_t size, char *to, const char *from)
{
	size_t len = strlen(from);
	
	if (len > size-1)
		len = size-1;
	memmove(to, from, len);
	to[len] = 0;
}

/* This function adds '...' to the end of the string.
 * if it does not fit!
 */
void yv_strcpy_dottt(size_t size, char *d, const char *s, size_t len)
{
	if (len+1 <= size) {
		yv_strcpy(size, d, s, len);
		return;
	}
	if (size < 5) {
		/* Silly, but anyway... */
		yv_strcpy(size, d, "...");
		return;
	}
	memcpy(d, s, size-4);
	memcpy(d+size-4, "...", 3);
	d[size-1] = 0;
}

void yv_strcpy_left(size_t size, char *to, const char *from, char ch)
{
	if (size > 0) {
		size--;
		while (*from && size-- && *from != ch)
			*to++ = *from++;
		*to = 0;
	}
}

void yv_strcpy_right(size_t size, char *to, const char *from, char ch)
{
	if (size > 0) {
		size--;
		while (*from && *from != ch)
			from++;
		if (*from == ch)
			from++;
		while (*from && size--)
			*to++ = *from++;
		*to = 0;
	}
}

void yv_strcat(size_t size, char *to, const char *from)
{
	while (*to && size--) to++;
	yv_strcpy(size, to, from);
}

void yv_strcat(size_t size, char *to, char ch)
{
	while (*to && size--) to++;
	if (size >= 1) {
		*to = ch;
		*(to+1) = 0;
	}
}

bool yv_strcat(char **to, const char *to_cat)
{
	size_t	len = strlen(*to) + strlen(to_cat) + 1;
	char	*ptr;
	
	if (!(ptr = (char*) realloc((void *) *to, len)))
		return false;
	*to = ptr;
	strcat(*to, to_cat);
	return true;
}

bool yv_strcat(char **to, const char *to_cat, size_t cat_len)
{
	char *ptr;

	if (cat_len == (size_t) -1)
		cat_len = strlen(to_cat);
	size_t to_len = strlen(*to);
	size_t new_len = to_len + cat_len + 1;
	
	if (!(ptr = (char*) realloc((void *) *to, new_len)))
		return false;
	*to = ptr;
	memcpy((*to) + to_len, to_cat, cat_len);
	*((*to) + to_len + cat_len) = 0;
	return true;
}

void yv_strcat(size_t size, char *to, int i)
{
	char buffer[20];
	
	snprintf(buffer, 20, "%d", i);
	yv_strcat(size, to, buffer);
}

void yv_strcat(size_t size, char *to, uint32_t i)
{
	char buffer[20];

#ifdef PRIu32
	snprintf(buffer, 20, "%" PRIu32, i);
#else
	snprintf(buffer, 20, "%lu", (unsigned long) i);
#endif
	yv_strcat(size, to, buffer);
}

void yv_strcat(size_t size, char *to, uint64_t i)
{
	char buffer[40];

#ifdef PRIu64
	snprintf(buffer, 40, "%" PRIu64, i);
#else
	snprintf(buffer, 40, "%llu", (unsigned long long) i);
#endif
	yv_strcat(size, to, buffer);
	yv_strcat(size, to, buffer);
}

void yv_strcat(size_t size, char *to, int64_t i)
{
	char buffer[40];
	
#ifdef PRId64
	snprintf(buffer, 40, "%" PRId64, i);
#else
	snprintf(buffer, 40, "%lld", (long long) i);
#endif
	yv_strcat(size, to, buffer);
}

void yv_strcat_left(size_t size, char *to, const char *from, char ch)
{
	while (*to && size--) to++;
	yv_strcpy_left(size, to, from, ch);
}

void yv_strcat_right(size_t size, char *to, const char *from, char ch)
{
	while (*to && size--) to++;
	yv_strcpy_right(size, to, from, ch);
}

void yv_strcat_hex(size_t size, char *to, uint64_t i)
{
	char buffer[80];
	
#ifdef PRIx64
	snprintf(buffer, 80, "%" PRIx64, i);
#else
	snprintf(buffer, 80, "%llx", (unsigned long long) i);
#endif
	yv_strcat(size, to, buffer);
}

void yv_strcat_byte_size(size_t size, char *to, int64_t value)
{
	const char* unit = NULL;

	if ((value % 1024) == 0 && (value / 1024) > 0) {
		value /= 1024;
		unit = "K";
		if ((value % 1024) == 0 && (value / 1024) > 0) {
			value /= 1024;
			unit = "M";
			if ((value % 1024) == 0 && (value / 1024) > 0) {
				value /= 1024;
				unit = "G";
				if ((value % 1024) == 0 && (value / 1024) > 0) {
					value /= 1024;
					unit = "T";
					if ((value % 1024) == 0 && (value / 1024) > 0) {
						value /= 1024;
						unit = "P";
					}
				}
			}
		}
	}

	yv_strcat(size, to, value);
	if (unit)
		yv_strcat(size, to, unit);
}

void yv_format_context(size_t size, char *buffer, const char *func, const char *file, int line)
{
	char *ptr;

	if (func) {
		yv_strcpy(size, buffer, func);
		// If the "pretty" function includes parameters, remove them:
		if ((ptr = strchr(buffer, '(')))
			*ptr = 0;
		yv_strcat(size, buffer, "(");
	}
	else
		*buffer = 0;
	if (file) {
		yv_strcat(size, buffer, yv_last_name_of_path(file));
		if (line) {
			yv_strcat(size, buffer, ":");
			yv_strcat(size, buffer, line);
		}
	}
	if (func)
		yv_strcat(size, buffer, ")");
}

int yv_path_depth(const char *path)
{
	int count = 0;
	while (*path) {
		if (IS_DIR_CHAR(*path))
			count++;

		path++;
	}
	return count;
}

static const char *find_wildcard(const char *pattern)
{
	bool escaped = false;
	while (*pattern) {
		if ((*pattern == '*' || *pattern == '?' ) && !escaped)
			return pattern;
			
		if (*pattern == '\\')
			escaped = !escaped;
		else
			escaped = false;
			
		pattern++;
	}
	
	return NULL;
}

// Check if the path contains any variable components.
bool yv_fixed_pattern(const char *str)
{
	return (find_wildcard(str) == NULL);
}

#ifdef OS_WINDOWS
/* 
bool yv_match_pattern(const char *pattern, const char *str, bool ignore_case)
{
	bool escaped = false;
	
	while (*pattern && *str) {
		if ((*pattern == '*' || *pattern == '?' ) && !escaped) {
			if (*pattern == '?') {
				pattern++;
				str++;	
				continue;			
			}
			
			while (*pattern == '*' || *pattern == '?' ) pattern++; // eat the pattern matching characters.
			
			if (!*pattern) // A * at the end of the pattern matches everything.
				return true;
				
			// This is where it gets complicted.
			
			coming soon!
			
		}
					
		if (*pattern == '\\')
			escaped = !escaped;
		else
			escaped = false;
			
		if (escaped)
			pattern++;
		else {
			if (ignore_case) {
				if (yv_toupper(*pattern) != yv_toupper(*str))
					return false;
			} else if (*pattern != *str)
				return false;
			pattern++;
			str++;				
		}
		
	}
	
	return ((!*pattern) && (!*str));
}
*/
#else
bool yv_match_pattern(const char *pattern, const char *str, bool ignore_case)
{
	return fnmatch(pattern, str, ignore_case ? FNM_CASEFOLD : 0) == 0;
}
#endif

int yv_path_component_count(char *path)
{
	char	*end = path;
	int		count = 0;

	while (end) {
		path = end;
		if (IS_DIR_CHAR(*path))
			path++;
		if (!*path)
			break;
		end = strchr(path, YV_DIR_CHAR);
		count++;
	}
	return count;
}

/* This function returns "" if the path ends with a dir char */
char *yv_last_name_of_path(const char *path, int count)
{
	size_t		length;
	const char	*ptr;

	length = strlen(path);
	if (!length)
		return((char *) path);
	ptr = path + length - 1;
	while (ptr != path) {
		if (IS_DIR_CHAR(*ptr)) {
			count--;
			if (!count)
				break;
		}
		ptr--;
	}
	if (IS_DIR_CHAR(*ptr)) ptr++;
	return((char *) ptr);
}

char *yv_last_name_of_path(const char *path)
{
	return yv_last_name_of_path(path, 1);
}

/* This function returns the last name component, even if the path ends with a dir char */
char *yv_last_directory_of_path(const char *path)
{
	size_t	length;
	const char	*ptr;

	length = strlen(path);
	if (!length)
		return((char *)path);
	ptr = path + length - 1;
	if (IS_DIR_CHAR(*ptr))
		ptr--;
	while (ptr != path && !IS_DIR_CHAR(*ptr)) ptr--;
	if (IS_DIR_CHAR(*ptr)) ptr++;
	return((char *)ptr);
}

/* 1 is the first component. */
bool yv_nth_component_of_path(size_t index, const char *path, char *comp, size_t csize)
{
	const char	*end = path;
	size_t		i;

	for (i = 0; i<index && end; i++) {
		path = end;
		if (IS_DIR_CHAR(*path))
			path++;
		end = strchr(path, YV_DIR_CHAR);
	}

	if (i == index) {
		if (end)
			yv_strcpy(csize, comp, path, (size_t) (end - path));
		else
			yv_strcpy(csize, comp, path);
		return true;
	}

	*comp = 0;
	return false;
}

const char *yv_find_extension(const char *file_name)
{
	const char	*ptr;

	for (ptr = file_name + strlen(file_name) - 1; ptr >= file_name; ptr--) {
		if (IS_DIR_CHAR(*ptr))
			break;
		if (*ptr == '.')
			return ptr + 1;
	}
	return NULL;
}

void yv_remove_extension(char *file_name)
{
	char *ptr = (char *) yv_find_extension(file_name);

	if (ptr)
		*(ptr - 1) = 0;
}

bool yv_is_extension(const char *file_name, const char *ext)
{
	const char *ptr;

	if ((ptr = yv_find_extension(file_name)))
		return strcmp(ptr, ext) == 0;
	return false;
}

/*
 * Optionally remove a trailing directory delimiter (If the directory name consists of one
 * character, the directory delimiter is not removed).
 */
bool yv_remove_dir_char(char *dir_name)
{
	size_t length;
	
	length = strlen(dir_name);
	if (length > 1) {
		if (IS_DIR_CHAR(dir_name[length - 1])) {
			dir_name[length - 1] = '\0';
			return true;
		}
	}
	return false;
}

void yv_remove_last_name_of_path(char *path)
{
	char *ptr;

	if ((ptr = yv_last_name_of_path(path)))
		*ptr = 0;
}

static void yv_remove_last_directory_of_path(char *path)
{
	char *ptr;

	if ((ptr = yv_last_directory_of_path(path)))
		*ptr = 0;
}

bool yv_add_dir_char(size_t max, char *path)
{
	size_t slen = strlen(path);

	if (slen >= max)
		return false;

	if (slen == 0) {
		/* If no path is given we will be at the current working directory, under UNIX we must
		 * NOT add a directory delimiter character:
		 */
		return false;
	}

	if (!IS_DIR_CHAR(path[slen - 1])) {
		path[slen] = YV_DIR_CHAR;
		path[slen + 1] = '\0';
		return true;
	}
	return false;
}

bool yv_is_absolute(const char *path)
{
	return IS_DIR_CHAR(*path);
}

void yv_add_name_to_path(size_t max, char *path, const char *name)
{
	char *end_ptr = path + max - 1;

	yv_add_dir_char(max, path);
	path = path + strlen(path);

	if (IS_DIR_CHAR(*name))
		name++;
	while (*name && !IS_DIR_CHAR(*name) && path < end_ptr)
		*path++ = *name++;
	*path = 0;
}

const char *yv_next_name_of_path(const char *path)
{
	if (IS_DIR_CHAR(*path))
		path++;
	while (*path && !IS_DIR_CHAR(*path))
		path++;
	if (IS_DIR_CHAR(*path))
		path++;
	return path;
}

static void yv_adjust_absolute_path(size_t max, char *path, const char *rel_path)
{
	while (*rel_path) {
		if (*rel_path == '.') {
			if (*(rel_path + 1) == '.') {
				if (!*(rel_path + 2) || IS_DIR_CHAR(*(rel_path + 2))) {
					/* ..: move up one: */
					yv_remove_last_directory_of_path(path);
					goto loop;
				}
			}
			else {
				if (!*(rel_path + 1) || IS_DIR_CHAR(*(rel_path + 1)))
					/* .: stay here: */
					goto loop;
			}
		}

		/* Change to this directory: */
		yv_add_name_to_path(max, path, rel_path);
		loop:
		rel_path = yv_next_name_of_path(rel_path);
	}
}

void yv_make_absolute_path(size_t max, char *path, const char *rel_path, const char *cwd)
{
	if (yv_is_absolute(rel_path))
		yv_strcpy(max, path, rel_path);
	else {
		/* Path is relative to the current directory */
		yv_strcpy(max, path, cwd);
		yv_adjust_absolute_path(max, path, rel_path);
	}
	yv_remove_dir_char(path);
}

char *yv_strdup(const char *in_str)
{
	char *str;
	
	if (!in_str)
		return NULL;

	if (!(str = (char *) malloc(strlen(in_str) + 1)))
		return NULL;
	strcpy(str, in_str);
	return str;
}

char *yv_strdup(int i)
{
	char buffer[20];
	char *str;

	snprintf(buffer, 20, "%d", i);
	if (!(str = (char *) malloc(strlen(buffer) + 1)))
		return NULL;
	strcpy(str, buffer);
	return str;
}

char *yv_strdup(const char *in_str, size_t len)
{
	char *str;
	
	if (!in_str)
		return NULL;

	if (!(str = (char *) malloc(len + 1)))
		return NULL;

	// Allow for allocation of an oversized buffer.
	size_t str_len = strlen(in_str);
	if (len > str_len)
		len = str_len;
		
	memcpy(str, in_str, len);
	str[len] = 0;
	return str;
}

bool yv_starts_with(const char *cstr, const char *w_cstr)
{
	while (*cstr && *w_cstr) {
		if (*cstr != *w_cstr)
			return false;
		cstr++;
		w_cstr++;
	}
	return *cstr || !*w_cstr;
}

bool yv_ends_with(const char *cstr, const char *w_cstr)
{
	size_t		len = strlen(cstr);
	size_t		w_len = strlen(w_cstr);
	const char	*ptr = cstr + len - 1;
	const char	*w_ptr = w_cstr + w_len - 1;
	
	if (w_len > len)
		return false;

	if (w_len == 0)
		return false;

	while (w_ptr >= w_cstr) {
		if (*w_ptr != *ptr)
			return false;
		w_ptr--;
		ptr--;
	}

	return true;
}

void yv_chop(char *cstr, size_t clen)
{
	size_t len = strlen(cstr);
	
	if (clen > len)
		clen = len;
	len -= clen;
	cstr[len] = 0;
}

void yv_replace_string(size_t size, char *into, const char *find_str, const char *str)
{
	char *ptr;

	if ((ptr = strstr(into, find_str))) {
		size_t len = strlen(into);
		size_t len2 = strlen(str);
		size_t len3 = strlen(find_str);
		
		if (len + len2 + len3 >= size)
			len2 = size - len;
		
		memmove(ptr+len2, ptr+len3, len - (ptr + len3 - into));
		memcpy(ptr, str, len2);
		into[len + len2 - len3] = 0;
	}
}

void yv_replace_string(size_t size, char *into, const char ch, const char *str)
{
	char *ptr;

	if ((ptr = strchr(into, ch))) {
		size_t len = strlen(into);
		size_t len2 = strlen(str);
		
		if ((len + len2) > size)
			len2 = size - len;
		
		memmove(ptr+1, ptr+len2, len - (ptr - into + 1));
		memcpy(ptr, str, len2);
		into[len + len2 - 1] = 0;
	}
}

void yv_replace_all(char *str, const char from, const char to)
{
	char* ptr;

	while ((ptr = strchr(str, from))) {
		   *ptr = to;
	}
}

bool yv_is_number(const char *ptr, bool can_have_sign, bool must_be_positive)
{
	if (!*ptr)
		return false;

	if (can_have_sign) {
		if ((!must_be_positive && *ptr == '-') || *ptr == '+')
			ptr++;
	}

	if (!isdigit(*ptr))
		return false;

	while (isdigit(*ptr))
		ptr++;

	return !*ptr;
}

uint64_t yv_str_to_word8(const char *ptr, bool *overflow)
{
	uint64_t value = 0;

	if (overflow)
		*overflow = false;
	while (*ptr == '0') ptr++;
	if (!*ptr)
		value = (uint64_t) 0;
	else {
#ifdef PRIu64
		sscanf(ptr, "%" PRIu64, &value);
#else
		unsigned long long ull;
		sscanf(ptr, "%llu", &ull);
		value = ull;
#endif
		if (!value && overflow)
			*overflow = true;
	}
	return value;
}

int64_t yv_str_to_int8(const char *ptr, bool *overflow)
{
	int64_t value = 0;

	if (overflow)
		*overflow = false;
	while (*ptr == '0') ptr++;
	if (!*ptr)
		value = (int64_t) 0;
	else {
#ifdef PRId64
		sscanf(ptr, "%" PRId64, &value);
#else
		long long ul;
		sscanf(ptr, "%lld", &ul);
		value = ul;
#endif
		if (!value && overflow)
			*overflow = true;
	}
	return value;
}

uint32_t yv_str_to_word4(const char *ptr, bool *overflow)
{
	uint32_t value = 0;

	if (overflow)
		*overflow = false;
	while (*ptr == '0') ptr++;
	if (!*ptr)
		value = (uint32_t) 0;
	else {
#ifdef PRIu32
		sscanf(ptr, "%" PRIu32, &value);
#else
		unsigned long ul;
		sscanf(ptr, "%lu", &ul);
		value = ul;
#endif
		if (!value && overflow)
			*overflow = true;
	}
	return value;
}

bool yv_str_to_bool(const char *ptr, bool *invalid)
{
	bool val = false;
	
	if (invalid)
		*invalid = false;
	
	switch (*ptr) {
		case 'y':
		case 'Y':
		case 't':
		case 'T':
		case '1':
			val = true;
			break;
		case 'n':
		case 'N':
		case 'f':
		case 'F':
		case '0':
			val = false;
			break;
			
		default:
			if (invalid)
				*invalid = true;
	}

	return val;
}

int64_t yv_byte_size_to_int8(const char *ptr, bool *invalid)
{
	char	*end_ptr;
	double	size;

	if (invalid)
		*invalid = false;

	while (*ptr && isspace(*ptr))
		ptr++;

	if (!isdigit(*ptr) && *ptr != '.')
		goto failed;

	size = strtod(ptr, &end_ptr);

	ptr = end_ptr;
	while (*ptr && isspace(*ptr))
		ptr++;
	
	switch (yv_toupper(*ptr)) {
		case 'P':
			size *= 1024.0;
		case 'T':
			size *= 1024.0;
		case 'G':
			size *= 1024.0;
		case 'M':
			size *= 1024.0;
		case 'K':
			size *= 1024.0;
			ptr++;
			break;
		case '\0':
			break;
		default:
			if (yv_toupper(*ptr) !=  'B' && !isspace(*ptr))
				goto failed;
			break;
	}
	
	// KB, MB, etc.
	if (yv_toupper(*ptr) == 'B')
		ptr++;

	while (*ptr && isspace(*ptr))
		ptr++;

	if (*ptr)
		goto failed;

	return (int64_t) size;

	failed:
	if (invalid)
		*invalid = true;
	return 0;
}

int64_t yv_sec_time_to_int8(const char *ptr, bool *invalid)
{
	char	*end_ptr;
	double	size;

	if (invalid)
		*invalid = false;

	while (*ptr && isspace(*ptr))
		ptr++;

	if (!isdigit(*ptr) && *ptr != '.')
		goto failed;

	size = strtod(ptr, &end_ptr);

	ptr = end_ptr;
	while (*ptr && isspace(*ptr))
		ptr++;
	
	switch (yv_toupper(*ptr)) {
		case 'D':
			size *= 24.0;
		case 'H':
			size *= 60.0;
		case 'M':
			size *= 60.0;
		case 'S':
			ptr++;
			break;
		case '\0':
			break;
		default:
			if (!isspace(*ptr))
				goto failed;
			break;
	}

	while (*ptr && isspace(*ptr))
		ptr++;

	if (*ptr)
		goto failed;

	return (int64_t) size;

	failed:
	if (invalid)
		*invalid = true;
	return 0;
}

static uint32_t yv_hex_value(char ch)
{
	unsigned char uch = (unsigned char) ch;

	if (uch >= '0' && uch <= '9')
		return uch - '0';
	if (uch >= 'A' && uch <= 'F')
		return uch - 'A' + 10; 
	if (uch >= 'a' && uch <= 'f')
		return uch - 'a' + 10;
	return 0;
}

size_t yv_hex_to_bin(size_t size, void *v_bin, size_t len, const char *hex)
{
	size_t	tot_size = size;
	uint32_t	val = 0;
	size_t	shift = 0;
	unsigned char *bin = (unsigned char *) v_bin;

	if (len & 1)
		shift = 1;
	for (size_t i=shift; i<len+shift && size > 0; i++) {
		if (i & 1) {
			val = val | yv_hex_value(*hex);
			*bin = val;
			bin++;
			size--;
		}
		else
			val = yv_hex_value(*hex) << 4;
		hex++;
	}
	return tot_size - size;
}

size_t yv_hex_to_bin(size_t size, void *bin, const char *hex)
{
	return yv_hex_to_bin(size, bin, strlen(hex), hex);
}

#define HEX_DIGIT(x)	((x) <= 9 ? '0' + (x) : 'A' + ((x) - 10))

// NOTE: yv_bin_to_hex() Always null terminates the result.
void yv_bin_to_hex(size_t size, char *hex, size_t len, const void *v_bin)
{
	const unsigned char *bin = (unsigned char *) v_bin;
	if (size == 0)
		return;
	size--;
	for (size_t i=0; i<len && size > 0; i++) {
		*hex = HEX_DIGIT(*bin >> 4);
		hex++;
		size--;
		if (size == 0)
			break;
		*hex = HEX_DIGIT(*bin & 0x0F);
		hex++;
		size--;
		bin++;
	}
	*hex = 0;
}	

void yv_str_toupper(char *ptr)
{
	while (*ptr) {
		*ptr = yv_toupper(*ptr);
		ptr++;
	}
}

void yv_str_tolower(char *ptr)
{
	while (*ptr) {
		*ptr = tolower(*ptr);
		ptr++;
	}
}

uint32_t yv_str_hash(const char* str, size_t len)
{
	uint32_t h = 0, g;

	for (size_t i=0; i<len && str[i]; i++) {
		h = (h << 4) + (uint32_t) ((unsigned char*) str)[i];
		if ((g = (h & 0xF0000000)))
			h = (h ^ (g >> 24)) ^ g;
	}

	return (h);
}

uint32_t yv_str_hash_ci(const char* str, size_t len)
{
	uint32_t h = 0, g;

	for (size_t i=0; i<len && str[i]; i++) {
		h = (h << 4) + (uint32_t) yv_toupper(str[i]);
		if ((g = (h & 0xF0000000)))
			h = (h ^ (g >> 24)) ^ g;
	}

	return (h);
}

uint32_t yv_str_hash_djb2(const char* str, size_t len)
{
	uint32_t hash = 5381;
	int c;

	while ((c = *((const unsigned char*) str)) && len > 0) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		str++;
		len--;
	}

	return hash;
}

uint32_t yv_str_hash_sdbm(const char* str, size_t len)
{
	uint32_t hash = 0;
	int c;

	while ((c = *((const unsigned char*) str)) && len > 0) {
		hash = c + (hash << 6) + (hash << 16) - hash;
		str++;
		len--;
	}

	return hash;
}


// This does toupper() just for US-ASCII
// This is to ensure that the strings work with UTF-8
int yv_toupper(char ch)
{
	if (ch & 0x80)
		return (int) (unsigned char) ch;
	return toupper(ch);
}

int yv_tolower(char ch)
{
	if (ch & 0x80)
		return (int) (unsigned char) ch;
	return tolower(ch);
}

int yv_strcmp_ci(const char *pa, const char* pb, size_t len)
{
	int r = 0;

	if (pa && pb) {
		while (*pa && *pb && len) {
			r = yv_toupper(*pa) - yv_toupper(*pb);
			if (r != 0)
				break;
			pa++;
			pb++;
			len--;
		}
		if (len)
			r = yv_toupper(*pa) - yv_toupper(*pb);
	}
	return r;
}

bool yv_isspace(const char *ptr)
{
	while (*ptr) {
		if (!isspace(*ptr))
			return false;
	}
	return true;
}

/*
 * Return failed if this is not a valid number.
 */
bool yv_str_to_value(const char *ptr, uint64_t *value, uint8_t base)
{
	char *endptr;

	*value = (uint64_t) strtoull(ptr, &endptr, base);
	return *endptr ? false : true;
}

// Adds ... to the end of the string
void yv_strcpy_elided(size_t size, char* to_str, const char* from_str)
{
	if (size <= 5) {
		yv_strcpy(size, to_str, from_str);
		return;
	}
	size--;
	size_t len = strlen(from_str);
	size_t pre;
	if (len <= size) {
		strcpy(to_str, from_str);
		return;
	}
	size -= 3;
	pre = size/2;
	memcpy(to_str, from_str, pre);
	memcpy(to_str+pre, "...", 3);
	memcpy(to_str+pre+3, from_str + (len - (size - pre)), size - pre + 1);
}

bool yv_get_str_value(const char *str, const char *name, char term, size_t len, char* out_str)
{
	const char *ptr;
	
	if (!(ptr = strstr(str, name))) {
		*out_str = 0;
		return false;
	}
	ptr += strlen(name);
	while (isspace(*ptr))
		ptr++;
	if (*ptr != '=') {
		*out_str = 0;
		return true;
	}
	ptr++;
	yv_strcpy_left(len, out_str, ptr, term);
	return true;
}

int64_t yv_get_int_value(const char *str, const char *name, char term)
{
	char number[100];

	yv_get_str_value(str, name, term, 100, number);
	return yv_str_to_int8(number, NULL);
}

int64_t yv_get_int_value_between(const char *str, const char *start, char end)
{
	const char *ptr;
	char number[100];

	while (str && *str) {
		if (start && *start) {
			if (!(ptr = strstr(str, start)))
				return 0;
			ptr += strlen(start);
		}
		else
			ptr = (char*) str;

		if (end)
			yv_strcpy_left(100, number, ptr, end);
		else
			yv_strcpy(100, number, ptr);
		if (yv_is_number(number))
			break;

		str = ptr;
		if (!start || !*start)
			break;
	}
	return yv_str_to_int8(number, NULL);
}

int64_t yv_get_byte_value(const char *str, const char *name, char term)
{
	const char *ptr;
	char number[100];
	
	if (!(ptr = strstr(str, name)))
		return 0;
	ptr += strlen(name);
	while (isspace(*ptr))
		ptr++;
	if (*ptr != '=')
		return 0;
	ptr++;
	yv_strcpy_left(100, number, ptr, term);
	return yv_byte_size_to_int8(number, NULL);
}

bool yv_get_bool_value(const char *str, const char *name, char term)
{
	char val[10];
	
	yv_get_str_value(str, name, term, 10, val);
	
	return yv_str_to_bool(val, NULL);
}

int64_t yv_get_sec_value(const char *str, const char *name, char term)
{
	const char *ptr;
	char number[100];
	
	if (!(ptr = strstr(str, name)))
		return 0;
	ptr += strlen(name);
	while (isspace(*ptr))
		ptr++;
	if (*ptr != '=')
		return 0;
	ptr++;
	yv_strcpy_left(100, number, ptr, term);
	return yv_sec_time_to_int8(number, NULL);
}

