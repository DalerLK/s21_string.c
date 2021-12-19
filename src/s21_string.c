#include "s21_string.h"

typedef unsigned long s21_size_t;

int del_check(char c, const char *delim) {
  int res = 0;
  while (*delim != '\0') {
    if (c == *delim) res++;
    delim++;
  }
  return res;
}

char *s21_strtok(char *str, const char *delim) {
  static char *ptr;
  int flag = 0;
  char *inter = s21_NULL, *ret = s21_NULL;
  if (!str) str = ptr;
  if (str) {
    while (1 && flag == 0) {
      if (del_check(*str, delim)) {
        str++;
        continue;
      }
      if (*str == '\0') flag = 1;
      break;
    }
    char *inter = str;
    while (1 && flag == 0) {
      if (*str == '\0') {
        ptr = str;
        ret = inter;
        flag = 1;
      }
      if (del_check(*str, delim)) {
        *str = '\0';
        ptr = str + 1;
        ret = inter;
        flag = 1;
      }
      str++;
    }
  }
  return ret;
}

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  unsigned char *buff1 = (unsigned char *)str1;
  unsigned char *buff2 = (unsigned char *)str2;
  int result = 0;
  for (s21_size_t i = 0; i < n; i++) {
    if (buff1[i] != buff2[i]) {
      if (buff1[i] > buff2[i]) {
        result = 1;
      } else {
        result = -1;
      }
      i = n + 1;
    }
  }
  return result;
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  int res = 0, slen1 = s21_strlen(str1), slen2 = s21_strlen(str2);
  s21_size_t i = 0;
  char *s1 = (char *)calloc(slen1, SIZE_CHAR + 1);
  char *s2 = (char *)calloc(slen1, SIZE_CHAR + 1);
  while (i < n) {
    s1[i] = str1[i];
    s2[i] = str2[i];
    i++;
  }
  res = s21_strcmp(s1, s2);
  free(s1);
  free(s2);
  return res;
}

char *s21_strstr(const char *haystack, const char *needle) {
  char *str = (char *)haystack;
  char *ndl = (char *)needle;
  char *res = s21_NULL;
  if (*ndl == '\0' || ndl == "") res = str;
  for (int i = 0; i < s21_strlen(str); i++) {
    if (str[i] == *ndl) {
      char *tmp = &str[i];
      int tmp_res = s21_strncmp(tmp, ndl, s21_strlen(ndl));
      if (tmp_res == 0) {
        res = &str[i];
        break;
      }
    }
  }
  return res;
}

void *s21_memset(void *str, int c, s21_size_t n) {
  unsigned char *string = (unsigned char *)str;
  unsigned char *res = s21_NULL;
  for (unsigned long i = 0; i != n; i++) {
    string[i] = c;
  }
  res = string;
  return res;
}

void *s21_memchr(const void *str, int c, s21_size_t n) {
  unsigned char *buffer = (unsigned char *)str;
  unsigned char *result = s21_NULL;
  for (s21_size_t i = 0; i < n; i++) {
    if (buffer[i] == c) {
      result = &buffer[i];
      i = n + 1;
    }
  }
  return result;
}

void *s21_memmove(void *dest, const void *src, s21_size_t n) {
  unsigned char *dest_str = (unsigned char *)dest;
  unsigned char *src_str = (unsigned char *)src;
  unsigned char tmp_str[n];
  for (s21_size_t i = 0; i < n; i++) {
    tmp_str[i] = src_str[i];
  }
  for (s21_size_t i = 0; i < n; i++) {
    dest_str[i] = tmp_str[i];
  }
  return dest_str;
}

void *s21_to_upper(const char *str) {
  char *res = s21_NULL;
  if (str) {
    s21_size_t len = s21_strlen(str);
    res = (char *)calloc(len, SIZE_CHAR + 1);
    if (res != s21_NULL) {
      for (s21_size_t i = 0; i < len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
          res[i] = str[i] - 32;
        } else {
          res[i] = str[i];
        }
      }
    }
  }
  return res;
}

void *s21_to_lower(const char *str) {
  char *res = s21_NULL;
  if (str) {
    s21_size_t len = s21_strlen(str);
    res = (char *)calloc(len, SIZE_CHAR + 1);
    if (res != s21_NULL) {
      for (s21_size_t i = 0; i < len; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
          res[i] = str[i] + 32;
        } else {
          res[i] = str[i];
        }
      }
    }
  }
  return res;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  char *res = s21_NULL;
  if (src) {
    if (s21_strlen(src) >= start_index) {
      if (str) {
        s21_size_t src_len = s21_strlen(src);
        s21_size_t str_len = s21_strlen(str);
        s21_size_t len = src_len + str_len;
        res = (char *)calloc(len, SIZE_CHAR);
        if (res != s21_NULL) {
          s21_size_t j = 0;
          for (s21_size_t i = 0; i < len; i++) {
            if (i == start_index) {
              for (s21_size_t str_i = 0; str_i < str_len; str_i++) {
                res[j] = str[str_i];
                j++;
              }
            }
            res[j] = src[i];
            j++;
          }
        }
      }
    }
  }
  return res;
}

void *s21_trim(const char *src, const char *trim_chars) {
  int src_null = src == s21_NULL ? 1 : 0;
  int trim_null = trim_chars == s21_NULL ? 1 : 0;
  char *res = s21_NULL;
  if (!src_null) {
    char *trims;
    if (trim_null || *trim_chars == '\0')
      trims = " \t\n\v\f\r";
    else
      trims = (char *)trim_chars;
    s21_size_t src_len = s21_strlen(src);
    s21_size_t chars_len = s21_strlen(trims);
    res = (char *)calloc(src_len, sizeof(char));
    if (res != s21_NULL) {
      int first_index = get_first_index(src, trims, src_len, chars_len);
      int last_index = get_last_index(src, trims, src_len, chars_len);
      int res_i = 0;
      for (int i = first_index; i <= last_index; i++) {
        res[res_i] = src[i];
        res_i++;
      }
    }
  }
  return res;
}

int get_first_index(const char *src, const char *trim_chars, int src_len,
                    int chars_len) {
  int first_index = 0;
  for (int i = 0; i < src_len; i++) {
    int flag = 0;
    int stop = 0;
    for (int j = 0; j < chars_len; j++) {
      if (chrcmp(src[i], trim_chars[j])) {
        flag = 1;
        break;
      }
      stop++;
    }
    if (stop == chars_len) break;
    if (flag) first_index++;
  }
  return first_index;
}

int get_last_index(const char *src, const char *trim_chars, int src_len,
                   int chars_len) {
  int last_index = src_len - 1;
  for (int i = last_index; i >= 0; i--) {
    int flag = 0;
    int stop = 0;
    for (int j = 0; j < chars_len; j++) {
      if (chrcmp(src[i], trim_chars[j])) {
        flag = 1;
        break;
      }
      stop++;
    }
    if (stop == chars_len) break;
    if (flag) last_index--;
  }
  return last_index;
}

int chrcmp(char c1, char c2) { return c1 == c2; }

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  unsigned char *dest_str = (unsigned char *)dest;
  unsigned char *src_str = (unsigned char *)src;
  if (dest_str != s21_NULL && src_str != s21_NULL) {
    for (s21_size_t i = 0; i < n; i++) {
      dest_str[i] = src_str[i];
    }
  }
  return dest_str;
}

char *s21_strpbrk(const char *str1, const char *str2) {
  char *res = s21_NULL;
  for (s21_size_t i = 0; str1[i] != '\0'; i++) {
    for (s21_size_t j = 0; str2[j] != '\0'; j++) {
      if (str1[i] == str2[j]) {
        res = (char *)&str1[i];
        break;
      }
    }
    if (res != s21_NULL) {
      break;
    }
  }
  return res;
}

char *s21_strcat(char *destination, const char *source) {
  int last = s21_strlen(destination);
  int i = 0;
  while (source[i] != '\0') {
    destination[last] = source[i];
    i++;
    last++;
  }
  destination[last] = '\0';
  return destination;
}

char *s21_strchr(const char *str, int c) {
  char *res = '\0';
  c = (char)c;
  for (int i = 0; i < s21_strlen(str) + 1; i++) {
    if (str[i] == c) {
      res = (char *)&str[i];
      break;
    }
  }
  return res;
}

char *s21_strcpy(char *dest, const char *src) {
  unsigned long i;
  unsigned long len = s21_strlen(src);
  for (i = 0; i < len; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
  return dest;
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  for (unsigned long i = 0; i != n; i++) {
    dest[i] = src[i];
  }
  return dest;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  int last = s21_strlen(dest);
  for (s21_size_t i = 0; i < n; i++) {
    dest[last] = src[i];
    last++;
  }
  dest[last] = '\0';
  return dest;
}

char *s21_strrchr(const char *str, s21_size_t c) {
  int i = 1;
  char *res = s21_NULL;
  while (str[i - 1] != 0) i++;
  while (i-- != 0) {
    if (str[i] == (char)c) {
      res = (char *)str + i;
      break;
    }
  }
  return res;
}

char *s21_strerror(int errnum) {
  static char res[100] = {0};
  ARRAY;
  int flag = 0;
  for (int i = 0; i < ERR_MAX; i++) {
    if (errnum == i) {
      s21_strcpy(res, errlist[i]);
      flag = -1;
    }
  }
  if (flag == 0) {
    char str[100] = ERROR;
    char *errnum_str = s21_convert_from_int(errnum);
    char *tmp = s21_strcat(str, errnum_str);
    s21_strcpy(res, tmp);
  }
  return res;
}

char *s21_convert_from_int(int num) {
  char tmp[20];
  static char result[20];
  int i = 0;
  int n = num;
  while (n != 0) {
    if (n < 0) {
      tmp[i] = (n * (-1) % 10) + '0';
    } else {
      tmp[i] = (n % 10) + '0';
    }
    n /= 10;
    i++;
  }
  if (num < 0) {
    tmp[i++] = '-';
  }
  tmp[i] = '\0';
  s21_reverse_str(result, tmp);
  return result;
}

void s21_reverse_str(char *dest, char *str) {
  s21_size_t len = s21_strlen(str);
  s21_size_t i = 0;
  for (s21_size_t j = len - 1; i < len; i++, j--) {
    dest[i] = str[j];
  }
  dest[i] = '\0';
}

int s21_strcmp(const char *str1, const char *str2) {
  int res = 0;
  while (*str1 == *str2 && (*str1 != '\0' || *str2 != '\0')) {
    str1 += 1;
    str2 += 1;
  }
  if (*str1 != *str2) res = *str1 - *str2;
  return res;
}

s21_size_t s21_strlen(const char *str) {
  s21_size_t len = 0;
  for (; str[len]; len++) {
  }
  return len;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  s21_size_t count = 0;
  for (s21_size_t i = 0; str1[i] != '\0'; i++) {
    for (s21_size_t j = 0; str2[j] != '\0'; j++) {
      if (str1[i] == str2[j]) {
        i = 404;
      }
    }
    if (i == 404) {
      break;
    }
    count++;
  }
  return count;
}

s21_size_t s21_strspn(const char *str1, const char *str2) {
  s21_size_t res = 0;
  for (; *str1; str1++, res++) {
    const char *p;
    p = str2;
    while (*p != *str1 && *p) p++;
    if (!*p) return res;
  }
  return res;
}

void s21_clear(char *str) {
  while (*str) {
    *str = 0;
    str++;
  }
}

int get_format_token(char *format, char *token_str) {
  int i = 0;
  while (*format) {
    if (check_specs(*format)) {
      break;
    }
    token_str[i] = *format;
    format += 1;
    i += 1;
  }
  token_str[i] = *format;
  token_str[i + 1] = '\0';
  return i;
}

int is_dec(char c) { return c >= '0' && c <= '9'; }

int check_specs(char c) {
  int res = 0;
  char *specs = "cdieEfgGosuxXpn%";
  for (int i = 0; i != 16; i++) {
    if (specs[i] == c) {
      res = 1;
      break;
    }
  }
  return res;
}
