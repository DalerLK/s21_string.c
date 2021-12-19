#include "s21_string.h"

struct Token {
    int width;
    int skip_arg;
    int length;
} Token;

int s21_sscanf(const char *str, const char *format, ...) {
  int length = 0, counter = 0;
  va_list params;
  va_start(params, format);
  char *f = (char*) format;
  char *s = (char*) str;
  int len  = s21_strlen(s);
  while (*f != '\0') {
    if (*f == '%') {
      char token_str[FORMAT_MAX];
      f += 1;
      check_specifiers(f);
      f += get_format_token(f, token_str);
      Token.skip_arg = token_str[0] == '*' ? 1 : 0;
      int displace = start_processing(s, token_str, params, &counter);
      if (displace == -1)
        break;
      counter += displace;
      s += displace;
      if (!Token.skip_arg) length += 1;
    }
    f += 1;
  }
  va_end(params);
  return len != 0 ? length : -1;
}

int start_processing(char *str, char*format, va_list params, int *counter) {
  Token.width = get_width(format);
  Token.length = check_length(format);
  int displace = read_specifier(str, format, params, counter);
  return displace;
}

int read_specifier(char *str, char *format, va_list params, int *counter) {
  int displace = 0;
  s21_size_t len = s21_strlen(format);

  if (format[len - 1] == 'd')
    displace = read_int_d(str, format, params);

  if (format[len - 1] == 'i')
    displace = read_int_i(str, format, params);

  if (format[len - 1] == 'x' || format[len - 1] == 'X')
    displace = read_int_x(str, format, params);

  if (format[len - 1] == 'o')
    displace = read_int_o(str, format, params);

  if (is_spec_float(format[len - 1]))
    displace = read_float(str, format, params);

  if (format[len - 1] == 'u')
    displace = read_int_u(str, format, params);

  if (format[len - 1] == 'c')
    displace = read_char(str, params);

  if (format[len - 1] == 's')
    displace = read_str(str, format, params);

  if (format[len - 1] == 'n')
    displace = read_spec_n(params, counter);

  if (format[len - 1] == 'p')
    displace = read_spec_p(str, params);

  if (format[len - 1] == '%')
    displace = read_spec_percent(str);

  return displace;
}

int read_int_d(char *str, char *format, va_list parameters) {
  int displace, minus;
  str += skip_elements(str, &displace, &minus);
  long res;
  int len = 0;
  if (Token.width) {
    char copy[12];
    copy_dec_str(copy, str);
    len = s21_strlen(copy);
    res = convert_from_int(copy, &displace);
  } else {
      len = s21_strlen(str);
      res = convert_from_int(str, &displace);
  }
  res = minus ? res *(-1) : res;
  write_signed_int(format, parameters, res, len);
  return displace;
}

int read_int_i(char *str, char *format, va_list parameters) {
  int displace = skip_spaces(str);
  str += displace;
  long res;
  int len;
  if (Token.width) {
    char copy[20];
    copy_hex_str(copy, str);
    len = s21_strlen(copy);
    res = read_int_i_str(copy, &displace);
  } else {
    len = s21_strlen(str);
    res = read_int_i_str(str, &displace);
  }
  write_signed_int(format, parameters, res, len);
  return displace;
}

int read_int_i_str(char *str, int *displace) {
  long res;
  int minus = *str == '-' ? 1 : 0;
  str += skip_sign(str, displace);
  if (*str == '0' && (*(str + 1) != 'x' && *(str +1) != 'X')) {
    int len = 0;
    len = get_octnum_length(str + 1);
    res = convert_from_oct(str + 1, len, displace);
  } else if (*str == '0' && (*(str + 1) == 'x' || *(str +1) == 'X')) {
    int len = 0;
    str += 2;
    len = get_hexnum_length(str);
    res = convert_from_hex(str, len, displace);
  } else {
    res = convert_from_int(str, displace);
  }
  return minus ? res * (-1) : res;
}

int read_int_x(char *str, char *format, va_list params) {
  int displace, minus;
  str += skip_elements(str, &displace, &minus);
  long unsigned res = 0;
  int len_flag = 0;
  if ((*str == '0' && *(str+1) == 'X') || (*str == '0' && *(str+1) == 'x')) {
    if (Token.width <= 2 && Token.width) {
      len_flag = 1;
    }
      str += 2;
  }
  int len = get_hexnum_length(str);
  if (Token.width < len && Token.width) {
    char copy[20];
    copy_hex_str(copy, str);
    int copy_len = get_copy_hex_len(copy);
    res = convert_from_hex(copy, copy_len, &displace);
  } else {
    res = convert_from_hex(str, len, &displace);
  }
  if (!len_flag) write_uint(format, params, res);
  return len != 0 ? displace : -1;
}

int read_int_o(char *str, char *format, va_list params) {
  int displace, minus;
  str += skip_elements(str, &displace, &minus);
  unsigned res = 0;
  int len = get_octnum_length(str);
  if (Token.width < len && Token.width) {
    char copy[20];
    copy_oct_str(copy, str);
    int copy_len = get_copy_oct_len(copy);
    res = convert_from_oct(copy, copy_len, &displace);
  } else {
    res = convert_from_oct(str, len, &displace);
  }
  write_uint(format, params, res);
  return len == 0 ? - 1 : displace;
}

int read_int_u(char *str, char *format, va_list parameters) {
  int displace, minus;
  str += skip_elements(str, &displace, &minus);
  unsigned long res;
  if (Token.width) {
    char copy[12];
    copy_dec_str(copy, str);
    res = convert_from_unsigned(copy, &displace);
  } else {
    res = convert_from_unsigned(str, &displace);
  }
  write_uint(format, parameters, res);
  return displace;
}

int read_float(char *str, char *format, va_list parameters) {
  long double res = 0;
  int displace = skip_spaces(str);
  str += displace;
  int len = 0, index = 0;
  char copy[20];
  if (*str == '-' || *str == '+') {
    copy[index] = *str;
    str += 1;
    len += 1;
    index++;
  }
  len += get_expo_len(str);
  if (!is_dec(*str)) len = 0;
  if (Token.width < len && Token.width) {
    copy_expo_str(copy, str, index);
    int copy_len = get_copy_expo_len(copy);
    res = convert_from_expo(copy, copy_len, &displace);
  } else {
    if (index != 0) {
      res = convert_from_expo(str -1, len, &displace);
    } else {
      res = convert_from_expo(str, len, &displace);
    }
  }
  write_float(format, parameters, res);
  return len != 0 ? displace : -1;
}

int read_char(const char *str, va_list parameters) {
  int displace = 1;
  if (!Token.skip_arg && !Token.length) {
    char *ch = va_arg(parameters, char*);
    *ch = *str;
  }
  if (!Token.skip_arg && Token.length) {
    wchar_t *ch = va_arg(parameters, wchar_t*);
    *ch = *str;
  }
  return displace;
}

int read_str(char *str, char *format, va_list params) {
  int displace = skip_spaces(str);
  str += displace;
  if (Token.width && !Token.skip_arg) {
    displace += write_copy_str(str, format, params, displace);
  } else if (!Token.width && !Token.skip_arg) {
    displace += write_str(str, format, params, displace);
  } else {
    while (*str != ' ' && *str) {
      str += 1;
      displace += 1;
    }
  }
  return displace;
}

int read_spec_n(va_list params, const int *counter) {
  int displace = 0;
  if (!Token.skip_arg) {
    int *c = va_arg(params, int*);
    *c = *counter;
    if (*c == 0) displace = -1;
  }
  return displace;
}

int read_spec_p(char *str, va_list params) {
  int displace = skip_spaces(str);
  str += displace;
  if ((*str == '0' && *(str+1) == 'X') || (*str == '0' && *(str +1) == 'x')) {
    if (Token.width) Token.width -= 2;
    str += 2;
  }
  char num_arr[20];
  if (!Token.skip_arg && !Token.width) {
    get_hex_num(num_arr, str);
  } else {
    copy_hex_str(num_arr, str);
  }
  int len = (int)s21_strlen(num_arr);
  long unsigned res = convert_from_hex(num_arr, len, &displace);
  if (!Token.skip_arg) {
    void **p = va_arg(params, void**);
    *p = (void*)(0x0 + res);
  }
  return len != 0 ? displace : -1;
}

int read_spec_percent(char *str) {
  int displace = skip_spaces(str);
  str += displace;
  displace = *str == '%' ? displace + 1 : -1;
  return displace;
}

void write_signed_int(char *format, va_list params, long res, int len) {
  if (!Token.skip_arg && !Token.length) {
    int *res_pointer = va_arg(params, int*);
    int tmp = *res_pointer;
    *res_pointer = len == 0 ? tmp : res;
  }
  if (!Token.skip_arg && Token.length) {
    int f_len = s21_strlen(format);
    if (format[f_len - 2] == 'l') {
      long *res_pointer = va_arg(params, long*);
      long tmp = *res_pointer;
      *res_pointer = len == 0 ? tmp : res;
    }
    if (format[f_len - 2] == 'h') {
      short* res_pointer = va_arg(params, short*);
      short tmp = *res_pointer;
      *res_pointer = len == 0 ? tmp : res;
    }
  }
}

void write_uint(char *format, va_list params, long unsigned res) {
  if (!Token.skip_arg && !Token.length) {
    unsigned int *res_pointer = va_arg(params, unsigned int*);
    *res_pointer = res;
  }
  if (!Token.skip_arg && Token.length) {
    int form_len = (int)s21_strlen(format);
    if (format[form_len - 2] == 'l') {
      long unsigned *res_pointer = va_arg(params, long unsigned*);
      *res_pointer = res;
    }
    if (format[form_len - 2] == 'h') {
      short unsigned *res_pointer = va_arg(params, short unsigned*);
      *res_pointer = res;
    }
  }
}

void write_float(char *format, va_list params, long double res) {
  if (!Token.skip_arg && !Token.length) {
    float *res_pointer = va_arg(params, float*);
    *res_pointer = (float)res;
  }
  if (!Token.skip_arg && Token.length) {
    int form_len = (int)s21_strlen(format);
    if (format[form_len - 2] == 'L') {
      long double *res_pointer = va_arg(params, long double*);
      *res_pointer = res;
    }
  }
}

int write_str(char *str, char *format, va_list params, int displace) {
  int index = 0;
  if (Token.length) {
    int len = s21_strlen(format);
    if (format[len - 2] == 'l') {
      index += write_wchar_str(str, params);
    }
  } else {
    index += write_char_str(str, params);
  }
  displace += index - 1;
  return displace;
}

int write_wchar_str(char *str, va_list params) {
  int index = 0;
  wchar_t *s_pointer = va_arg(params, wchar_t*);
  while (*str != ' ' && *str) {
    s_pointer[index] = *str;
    str += 1;
    index++;
  }
  s_pointer[index] = '\0';
  return index;
}

int write_char_str(char *str, va_list params) {
  int index = 0;
  char *s_pointer = va_arg(params, char*);
  while (!esli_razdelitel(*str) && *str) {
    s_pointer[index] = *str;
    str += 1;
    index++;
  }
  s_pointer[index] = '\0';
  return index;
}

int write_copy_str(char *str, char *format, va_list params, int displace) {
  s21_size_t kStrLen = s21_strlen(str);
  char copy[kStrLen];
  copy_string_str(copy, str);
  int copy_len = s21_strlen(copy);
  if (Token.length) {
    int len = s21_strlen(format);
    if (format[len - 2] == 'l') {
      write_copy_wchar_str(copy, params, copy_len);
    }
  } else {
    write_copy_char_str(copy, params, copy_len);
  }
  displace += copy_len;
  return displace - 1;
}

void write_copy_wchar_str(const char *str, va_list params, int len) {
  wchar_t *s_pointer = va_arg(params, wchar_t*);
  int i = 0;
  for (; i < len; i++) {
    s_pointer[i] = str[i];
  }
  s_pointer[i] = '\0';
}

void write_copy_char_str(const char *str, va_list params, int len) {
  char *s_pointer = va_arg(params, char*);
  int i = 0;
  for (; i < len; i++) {
    s_pointer[i] = str[i];
  }
  s_pointer[i] = '\0';
}

long convert_from_int(char *str, int *displace) {
  int len = get_number_length(str);
  long res = 0;
  res = len != 0  ? atol(str) : res;
  *displace += len;
  if (len == 0) *displace = -1;
  return res;
}

unsigned long convert_from_unsigned(char *str, int *displace) {
  int len = get_number_length(str);
  unsigned long res = 0;
  res = len != 0 ? strtoul(str, &str, 10) : res;
  *displace += len;
  if (len == 0) *displace = -1;
  return res;
}

unsigned long convert_from_hex(const char *arr, int len, int *displace) {
  long unsigned res = 0;
  long unsigned base = 1;
  if (len > 16) {
    res = 0xffffffffffffffff;
  } else if (len != 0) {
      int numbers = 48, bigletters = 55, smallletters = 87;
      for (int i = len - 1; i > -1; i -= 1) {
        if (arr[i] >= '0' && arr[i] <= '9') {
            res += (arr[i] - numbers) * base;
        } else if (arr[i] >= 'A' && arr[i] <= 'F') {
            res += (arr[i] - bigletters) * base;
        } else if (arr[i] >= 'a' && arr[i] <= 'f') {
            res += (arr[i] - smallletters) * base;
        }
        base *= 16;
      }
  }
  *displace += len;
  return res;
}

unsigned int convert_from_oct(const char *arr, int len, int *displace) {
  unsigned int res = 0;
  unsigned int base = 1;
  int numbers = 48;
  for (int i = len - 1; i >= 0; i -= 1) {
    if (arr[i] >= '0' && arr[i] <= '7') {
      res += (arr[i] - numbers) * base;
      base *= 8;
    }
  }
  *displace += len;
  return res;
}

long double convert_from_expo(const char *arr, int len, int *displace) {
  long double res = 0;
  double number = 0;
  int rank = 0;
  int index = 0;
  char copy_arr[20] = {0};
  if (*arr == '-' || *arr == '+') {
    copy_arr[index] = *arr;
    index++;
  }
  while (index < len && (is_float(arr[index]))) {
    copy_arr[index] = arr[index];
    index++;
  }
  number = atof(copy_arr);
  char rank_arr[10];
  int rank_index = 0;
  if (arr[index] == 'e' || arr[index] == 'E') {
    index++;
    while (index < len && (is_dec(arr[index]) || arr[index] == '-' || arr[index] == '+')) {
      rank_arr[rank_index] = arr[index];
      index++;
      rank_index++;
    }
  }
  rank_arr[rank_index] = '\0';
  rank = atoi(rank_arr);
  res = convert_to_expo(number, rank);
  *displace += len;
  return res;
}


char* get_hex_num(char *dest, char *str) {
  int i = 0;
  while (is_hex(*str)) {
    dest[i] = *str;
    str += 1;
    i++;
  }
  dest[i] = '\0';
  return dest;
}

int get_number_length(char *str) {
  int len = 0;
  while (is_dec(*str)) {
    len++;
    str += 1;
  }
  return len;
}

int get_hexnum_length(char *str) {
  int len = 0;
  while (is_hex(*str)) {
    len++;
    str += 1;
  }
  return len;
}

int get_octnum_length(char *str) {
  int len = 0;
  while (is_oct(*str)) {
    len++;
    str += 1;
  }
  return len;
}

int get_expo_len(char *str) {
  int len = 0;
  while (is_expo(*str)) {
    if ((*str == 'E' || *str == 'e') && (*(str+1) == '+' || *(str+1) == '-')) {
      str += 2;
      len += 2;
    } else if ((*str == 'E' || *str == 'e') && is_dec(*(str +1))) {
      int num = atoi(str + 1);
      while (num != 0) {
        len++;
        num /= 10;
      }
      len += 1;
      str += 2;
      } else {
        len++;
        str += 1;
      }
  }
  return len;
}

int get_width(char *str) {
  int len = 0;
  char copy[12];
  int i = 0;
  while (*str == '0') {
    str += 1;
  }
  while (*str >= '0' && *str <= '9') {
    copy[i] = *str;
    str += 1;
    i++;
  }
  copy[i] = '\0';
  len = atoi(copy);
  return len;
}

char *copy_dec_str(char *dest, char *src) {
  int i = 0;
  for (; i < Token.width && is_dec(*src); i++) {
    dest[i] = *src;
    src += 1;
  }
  dest[i] = '\0';
  return dest;
}

char *copy_hex_str(char *dest, char *src) {
  int i = 0;
  while (i < Token.width && is_hex(*src)) {
    dest[i] = *src;
    src += 1;
    i++;
  }
  dest[i] = '\0';
  return dest;
}

char *copy_oct_str(char *dest, char *src) {
  int i = 0;
  while (i < Token.width && is_oct(*src)) {
    dest[i] = *src;
    src += 1;
    i++;
  }
  dest[i] = '\0';
  return dest;
}

char *copy_expo_str(char *dest, char *src, int index) {
  int i = index;
  while (i < Token.width && (is_expo(*src))) {
    if (*src == 'E' || *src == 'e') {
      if (i + 1 == Token.width && !E_FLAG)
        break;
      dest[i] = *src;
      dest[i+1] = *(src+1);
      i += 2;
    } else {
        dest[i] = *src;
        src += 1;
        i++;
    }
  }
  dest[i] = '\0';
  return dest;
}

char *copy_string_str(char *dest, char *src) {
  int i = 0;
  while (i < Token.width && (*src != ' ' && *src)) {
    dest[i] = *src;
    src += 1;
    i++;
  }
  dest[i] = '\0';
  return dest;
}

int get_copy_hex_len(char *str) {
    int len = 0;
    while (len < Token.width && is_hex(*str)) {
                len++;
                str += 1;
    }
    return len;
}

int get_copy_oct_len(char *str) {
    int len = 0;
    while (len < Token.width && is_oct(*str)) {
        str += 1;
        len++;
    }
    return len;
}

int get_copy_expo_len(char *str) {
  int len = 0;
  if (*str == '-' || *str == '+') {
    len++;
    str += 1;
  }
  while (len < Token.width && (is_expo(*str))) {
    if ((*str == 'E' || *str == 'e') && (*(str+1) == '+' || *(str+1) == '-')) {
      str += 2;
      len += 2;
    } else if ((*str == 'E' || *str == 'e') && is_dec(*(str +1))) {
      int num = atoi(str + 1);
      while (num != 0) {
        len++;
        num /= 10;
      }
      str += 2;
    } else {
      len++;
      str += 1;
    }
  }
  return len;
}

int esli_razdelitel(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

int is_hex(char c) {
  return (c >= 'a' && c <= 'f') || (c>= 'A' && c <= 'F') || (c>= '0' && c <= '9');
}

int is_oct(char c) {
  return c >= '0' && c <= '7';
}

int is_expo(char c) {
  return (c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E';
}

int is_float(char c) {
  return (c >= '0' && c <= '9') || c == '.';
}

int is_spec_float(char c) {
  return c == 'E' || c == 'e' || c == 'g' || c == 'G' || c == 'f';
}

int is_sign(char c) {
  return c == '+' || c == '-';
}

int is_invalid_spec(char c) {
  return c == '\0' || (!check_specs(c) && c != '*' && (c < '0' || c > '9') && !is_length(c)) || c == ' ';
}

int is_length(char c) {
  return c == 'l' || c == 'L' || c == 'h';
}

long double expo(long double k, long double n) {
  long long int b = k;
  while (n > 1) {
    k *= b;
    n--;
  }
  return k;
}

long double convert_to_expo(double any_var, long double any_exp) {
  double result = any_var;
  long double exp = any_exp;
  if (exp > 0) {
    result = (float)result * expo(10, exp);
  }
  if (exp < 0) {
    exp = exp * -1;
    result = (float)result / expo(10, exp);
  }
  return result;
}

int skip_elements(char *str, int *displace, int *minus) {
  *displace = skip_spaces(str);
  str += *displace;
  *minus = *str == '-' ? 1 : 0;
  skip_sign(str, displace);
  return *displace;
}

int skip_spaces(const char *str) {
  int displace = 0;
  while (*str == ' ') {
    str += 1;
    displace++;
  }
  return displace;
}

int skip_sign(char *str, int *displace) {
  int sign = is_sign(*str);
  *displace += sign;
  return sign;
}

int check_length(char *str) {
  int res = 0;
  while (*str) {
    if (*str == 'l' || *str == 'L' || *str == 'h') {
      res = 1;
    }
    str += 1;
  }
  return res;
}

void check_specifiers(char *format) {
  while (*format && !check_specs(*format)) {
    if (is_invalid_spec(*format)) {
      printf("invalid conversion specifier");
      exit(0);
    }
    format += 1;
  }
}
