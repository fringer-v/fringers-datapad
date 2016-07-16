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

#ifndef __CSUTIL_H__
#define __CSUTIL_H__

#include <stddef.h>

//const char	*yv_version();
//const char	*yv_dot_version();

void		yv_strcpy(size_t size, char *d, const char *s, size_t len);
void		yv_strcpy(size_t size, char *d, const char *s);
void		yv_strmove(size_t size, char *to, const char *from);
void		yv_strcpy_dottt(size_t size, char *d, const char *s, size_t len);
void		yv_strcpy_left(size_t size, char *d, const char *s, char ch);
void		yv_strcpy_right(size_t size, char *d, const char *s, char ch);
void		yv_strcat(size_t size, char *d, const char *s);
void		yv_strcat_left(size_t size, char *d, const char *s, char ch);
void		yv_strcat_right(size_t size, char *d, const char *s, char ch);
void		yv_strcat(size_t size, char *d, char ch);
bool		yv_strcat(char **to, const char *to_cat);
bool		yv_strcat(char **to, const char *to_cat, size_t cat_len);
void		yv_strcat(size_t size, char *to, int i);
void		yv_strcat(size_t size, char *to, uint32_t i);
void		yv_strcat(size_t size, char *to, uint64_t i);
void		yv_strcat(size_t size, char *to, int64_t i);
void		yv_strcat_hex(size_t size, char *to, uint64_t i);
void		yv_strcat_byte_size(size_t size, char *to, int64_t i);
void		yv_format_context(size_t size, char *buffer, const char *func, const char *file, int line);
int			yv_path_depth(const char *path);
bool		yv_fixed_pattern(const char *path);
bool		yv_match_pattern(const char *pattern, const char *str, bool ignore_case);
char		*yv_last_name_of_path(const char *path, int count);
int			yv_path_component_count(char *path);
char		*yv_last_name_of_path(const char *path);
char		*yv_last_directory_of_path(const char *path);
bool		yv_nth_component_of_path(size_t index, const char *path, char *comp, size_t csize);
const char	*yv_find_extension(const char *file_name);
void		yv_remove_extension(char *file_name);
bool		yv_is_extension(const char *file_name, const char *ext);
bool		yv_remove_dir_char(char *dir_name);
void		yv_remove_last_name_of_path(char *path);
bool		yv_add_dir_char(size_t max, char *path);
bool		yv_is_absolute(const char *path);
void		yv_add_name_to_path(size_t max, char *path, const char *name);
const char	*yv_next_name_of_path(const char *path);
void		yv_adjust_path(size_t max, char *path, const char *rel_path);
char		*yv_strdup(const char *in_str);
char		*yv_strdup(int value);
char		*yv_strdup(const char *in_str, size_t len);
bool		yv_starts_with(const char *cstr, const char *w_cstr);
bool		yv_ends_with(const char *cstr, const char *w_cstr);
void		yv_chop(char *cstr, size_t clen);
void		yv_make_absolute_path(size_t max, char *path, const char *rel_path, const char *cwd);
void		yv_replace_string(size_t size, char *into, const char *find_str, const char *str);
void		yv_replace_string(size_t size, char *into, const char ch, const char *str);
void		yv_replace_all(char *str, const char from, const char to);
bool		yv_is_number(const char *cstr, bool can_have_sign = true, bool must_be_positive = false);
uint64_t	yv_str_to_word8(const char *ptr, bool *overflow = NULL);
int64_t		yv_str_to_int8(const char *ptr, bool *overflow);
uint32_t	yv_str_to_word4(const char *ptr, bool *overflow);
bool		yv_str_to_bool(const char *ptr, bool *invalid);
int64_t		yv_byte_size_to_int8(const char *ptr, bool *invalid = NULL);
int64_t		yv_sec_time_to_int8(const char *ptr, bool *invalid = NULL);
size_t		yv_hex_to_bin(size_t size, void *bin, const char *hex);
size_t		yv_hex_to_bin(size_t size, void *bin, size_t len, const char *hex);
// NOTE: yv_bin_to_hex() Always null terminates the result.
void		yv_bin_to_hex(size_t size, char *hex, size_t len, const void *bin);
void		yv_str_toupper(char *ptr);
void		yv_str_tolower(char *ptr);
uint32_t	yv_str_hash(const char* str, size_t len = (size_t) -1);
uint32_t	yv_str_hash_ci(const char* str, size_t len = (size_t) -1);
uint32_t	yv_str_hash_djb2(const char* str, size_t len = (size_t) -1);
uint32_t	yv_str_hash_sdbm(const char* str, size_t len = (size_t) -1);
int			yv_toupper(char ch);
int			yv_tolower(char ch);
int			yv_strcmp_ci(const char *pa, const char* pb, size_t len = (size_t) -1);
bool		yv_isspace(const char *ptr);
bool		yv_str_to_value(const char *ptr, uint64_t *value, uint8_t base);
void		yv_strcpy_elided(size_t size, char* to_str, const char* from_str);
bool		yv_get_str_value(const char *str, const char *name, char term, size_t len, char* out_str);
int64_t		yv_get_int_value(const char *str, const char *name, char term);
int64_t		yv_get_int_value_between(const char *str, const char *start, char end);
int64_t		yv_get_byte_value(const char *str, const char *name, char term);
bool		yv_get_bool_value(const char *str, const char *name, char term);
int64_t		yv_get_sec_value(const char *str, const char *name, char term);

#endif
