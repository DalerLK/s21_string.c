#include "s21_string.h"

struct p_flag {
    int pluses;
    int minuses;
    int spaces;
    int length;
    int star;
    int null;
    int grid;
    int dot;
} p_flag;

int s21_sprintf(char *str, const char *format, ...) {
    int result = 0, counter = 0;
    s21_clear(str);
    va_list argptr;
    va_start(argptr, format);
    while (*format != '\0') {
        if (*format != '%') {
            char tmp = *format;
            s21_strcat(str, &tmp);
            format += 1;
        } else {
            if (!check_string(format)) {
                printf("You haven't entered a qualifier!\n");
                break;
            }
            char token_str[256];
            format++;
            s21_parser(format, token_str, &counter);
            start_processing_p(str, token_str, argptr);
            format+=counter;
            counter = 0;
        }
    }
    va_end(argptr);
    result = s21_strlen(str);
    return result;
}

char *s21_parser(const char *format, char *token_str, int *counter) {
    int i = 0;
    while (*format) {
        if (check_specs(*format)) {
            *counter += 1;
            break;
        }
        *counter += 1;
        token_str[i] = *format;
        format++;
        i++;
    }
    token_str[i] = *format;
    token_str[i + 1] = '\0';
    return token_str;
}


int start_processing_p(char *str, char *format, va_list argptr) {
    check_flags(format);
    main_process(str, format, argptr);
    return 0;
}

void main_process(char *str, char *token_str, va_list argptr) {
    P_Token t = init_token();
    get_width_pres(token_str, argptr, &t);
    t.width = t.width < 0 ? t.width * (-1) : t.width;
    int len = s21_strlen(token_str);
    switch (token_str[len - 1]) {
        case 'c':
            processing_c(str, token_str, argptr, t);
            break;
        case 'd':
            processing_d(str, token_str, argptr, t);
            break;
        case 'i':
            processing_d(str, token_str, argptr, t);
            break;
        case 's':
            processing_s(str, token_str, argptr, t);
            break;
        case 'u':
            processing_u(str, token_str, argptr, t);
            break;
        case '%':
            processing_percent(str, t);
            break;
        case 'f':
            processing_f(str, argptr, t);
            break;
        case 'x':
            processing_x(str, token_str, argptr, t, 0);
            break;
        case 'X':
            processing_x(str, token_str, argptr, t, 1);
            break;
        case 'o':
            processing_o(str, token_str, argptr, t);
            break;
        case 'p':
            processing_p(str, argptr, t, 0);
            break;
        case 'n':
            processing_n(str, argptr);
            break;
        case 'e':
            processing_e(str, argptr, t);
            break;
        case 'E':
            t.exp = 1;
            processing_e(str, argptr, t);
            break;
        case 'g':
            processing_g(str, argptr, t);
            break;
        case 'G':
            t.exp = 1;
            processing_g(str, argptr, t);
            break;
        default:
            printf("Specifier is not correct!");
            exit(0);
    }
}

void processing_d(char *str, char *token_str, va_list argptr, P_Token t) {
    if (p_flag.dot) p_flag.null = 0;
    int token_len = s21_strlen(token_str);
    long tmp = 0;
    if (p_flag.length) {
        if (token_str[token_len - 2] == 'l') {
            tmp = va_arg(argptr, long);
        }
        if (token_str[token_len - 2] == 'h') {
            tmp = (short) va_arg(argptr, int);
        }
    } else {
        tmp = va_arg(argptr, int);
    }
    char *num = s21_itoa(tmp);
    int len = s21_strlen(num);
    write_d_to_string(str, num, len, t);
}

void processing_g(char *str, va_list argptr, P_Token t) {
    double tmp = va_arg(argptr, double);
    char tmp_str[10000] = {0}, num[10000] = {0};
    check_precision(&t);
    t.g = 1;
    gcvt(tmp , t.precision, tmp_str);
    int sign = tmp_str[0] == '-' ? 1 : 0;
    copy_e_str(num, tmp_str);
    int decs = get_dec_length(num);
    int is_e = is_expo_str(num);
    if (t.exp) change_letter_e(num);
    if (is_e) {
        if (p_flag.grid && decs < t.precision) {
            write_e_grid_and_nulls(str, num, decs, sign, t);
        } else if (p_flag.grid) {
            if (no_dot(tmp_str)) {
              int index = get_dot_index(num);
              char *s = s21_insert(num, ".", index);
              write_f_to_string(str, s, sign, &t);
              free(s);
            } else {
            write_f_to_string(str, num, sign, &t);
            }
        } else {
            char copy[10000] = {0};
            remove_dot(copy, num);
            write_f_to_string(str, copy, sign, &t);
        }
    } else {
        if (p_flag.grid && decs < t.precision) {
            int num_len = s21_strlen(num);
            while (decs < t.precision) {
                num[num_len++] = '0';
                decs++;
            }
        }
        write_f_to_string(str, num, sign, &t);
    }
}

int get_dot_index(char *str) {
  int index = 0;
  while (*str) {
    if (*str == 'e' || *str == 'E')
      break;
    str += 1;
    index++;
  }
  return index;
}

int no_dot(char *str) {
  int res = 1;
  while (*str) {
    if (*str == '.')
      res = 0;
    str += 1;
  }
  return res;
}

void write_e_grid_and_nulls(char *str, char *num, int decs, int sign, P_Token t) {
    int start_index = get_e_index(num);
    char nulls[10000] = {0};
    int k = 0;
    for (int i = decs; i < t.precision; i++) {
        nulls[k++] = '0';
    }
    char *s = s21_insert(num, nulls, start_index);
    write_f_to_string(str, s, sign, &t);
    free(s);
}

void copy_e_str(char *dest, char *src) {
    int j = 0;
    int len = s21_strlen(src);
    for (int i = 0; i < len; i++) {
        if (src[i] == '-' && src[i - 1] != 'e') {
            continue;
        }
        dest[j] = src[i];
        j++;
    }
    dest[j] = '\0';
}

void remove_dot(char *dest, char *str) {
    while (*str) {
        if (*str == '.' && (*(str - 1) == 'e' || *(str - 1) == 'E')) {
            str += 1;
            continue;
        }
        *dest++ = *str++;
    }
}

int get_e_index(char *str) {
    int res = 0;
    while (*str) {
        if (*str == 'e') {
            break;
        }
        str += 1;
        res++;
    }
    return res;
}

int is_expo_str(char *str) {
    int res = 0;
    while (*str) {
        if (*str == 'e') {
            res = 1;
        }
        str += 1;
    }
    return res;
}

int get_dec_length(char *str) {
    int len = 0;
    if (*str == '0') {
        str += 1;
    }
    while (*str) {
        if (is_dec(*str)) len++;
        if (*str == 'e') break;
        str += 1;
    }
    return len;
}

void change_letter_e(char *str) {
    while (*str) {
        if (*str == 'e') {
            *str = 'E';
        }
        str += 1;
    }
}

float_el init_f_el() {
    float_el f_el;
    f_el.decnpt = 0;
    f_el.end = 0;
    f_el.float_len = 0;
    f_el.i = 0;
    f_el.sign = 0;
    return f_el;
}

void processing_e(char *str, va_list argptr, P_Token t) {
    double tmp = va_arg(argptr, double);
    check_precision(&t);
    int decnpt = 0, sign = 0;
    char num[10000] = {0};
    make_expo_str(tmp, num, decnpt, &sign, t);
    write_f_to_string(str, num, sign, &t);
}

void make_expo_str(double arg, char *str, int decnpt, int *sign, P_Token t) {
    int i = 0;
    int pres = !t.null_flag ? t.precision + 1 : 1;
    char *tmp_str = ecvt(arg, pres, &decnpt, sign);
    int e = 0;
    e = decnpt <= 0 ? generate_minus_expo(decnpt) : generate_plus_expo(decnpt);
    str[i++] = tmp_str[0];
    if (!t.null_flag) {
        int j = 1;
        if (t.precision > 1 || !t.g) str[i++] = '.';
        if (t.g && !p_flag.grid) {
            while (tmp_str[j] > '0') {
                str[i++] = tmp_str[j];
                j++;
            }
        } else {
            if (t.g && p_flag.grid) t.precision -= 1;
            while (j <= t.precision) {
                str[i++] = tmp_str[j++];
            }
        }
    } else {
        if (p_flag.grid) {
           str[i++] = '.';
        }
    }
    str[i++] = t.exp ? 'E' : 'e';
    str[i++] = decnpt > 0 ? '+' : '-';
    if (e < 10)
        add_lt_ten(str, i, e);
    else
        add_gt_ten(str, e);
}

void add_lt_ten(char *str, int index, int e) {
    str[index++] = '0';
    str[index] = e % 10 + '0';
}

void add_gt_ten(char *str, int e) {
    int k = 0;
    char e_str[100] = {0};
    while (e != 0) {
        e_str[k++] = e % 10 + '0';
        e /= 10;
    }
    int e_len = s21_strlen(e_str);
    s21_reverse_array(e_str, e_len);
    s21_strcat(str, e_str);
}

int generate_plus_expo(int decnpt) {
    int e = 0;
    while (decnpt-- != 1) {
        e++;
    }
    return e;
}

int generate_minus_expo(int decnpt) {
    return (decnpt * -1) + 1;
}

void processing_f(char *str, va_list argptr, P_Token t) {
    char num[10000] = {0};
    check_precision(&t);
    float_el f_el = init_f_el();
    double tmp = va_arg(argptr, double);
    make_double_str(tmp, num, &f_el, t);
    write_f_to_string(str, num, f_el.sign, &t);
}


void make_double_str(double arg, char *num_str, float_el *f_el, P_Token t) {
    if (!t.null_flag) {
        char *tmp_str;
        tmp_str = fcvt(arg, t.precision, &f_el->decnpt, &f_el->sign);
        f_el->sign = 1 / arg < 0 ? 1 : 0;
        if (check_nan(tmp_str)) {
            make_nan_str(num_str, *f_el, t);
        } else {
            make_notnan_str(tmp_str, num_str, *f_el, t);
        }
     } else {
        t.precision = t.null_flag == 1 ? 0 : 1;
        fcvt(arg, t.precision, &f_el->decnpt, &f_el->sign);
        if (f_el->sign) num_str[f_el->i++] = '-';
        if (f_el->decnpt >= 0) {
            round_double_str(arg, num_str, f_el);
        } else {
            num_str[f_el->i++] = '0';
        }
        if (p_flag.grid) num_str[f_el->i] = '.';
    }
}

void make_nan_str(char *num_str, float_el f_el, P_Token t) {
    if (p_flag.spaces && !p_flag.pluses) num_str[f_el.i++] = ' ';
    num_str[f_el.i++] = '0';
    num_str[f_el.i++] = '.';
    int j = 0;
    while (j++ < t.precision) {
        num_str[f_el.i++] = '0';
    }
}

void make_notnan_str(char *tmp_str, char *num_str, float_el f_el, P_Token t) {
    if (f_el.decnpt > 0) {
        char floats[10000] = {0};
        f_el.float_len = get_float_len_sprintf(tmp_str, f_el.decnpt);
        for (int j = 0; j < f_el.decnpt; j++) {
            num_str[f_el.i++] = tmp_str[j];
        }
        if (t.precision && num_str[0] != '0') num_str[f_el.i++] = '.';
        f_el.end = t.precision < f_el.float_len ? t.precision : (int)s21_strlen(tmp_str);
        get_float_copy(floats, tmp_str, f_el.decnpt, f_el.end);
        if (t.g) {
            int k = 0;
            if (!p_flag.grid) {
              while (floats[k] > '0')
                num_str[f_el.i++] = floats[k++];
            } else {
              while (k < t.precision)
                num_str[f_el.i++] = floats[k++];
            }
        } else {
            s21_strcat(num_str, floats);
            add_nulls_to_str(num_str, f_el.float_len, t);
        }
    } else {
        copy_zero_float(num_str, tmp_str, f_el.decnpt, f_el.sign, t);
    }
}

int check_nan(char *str) {
    char *nan = "nan";
    return s21_strcmp(str, nan) == 0 ? 1 : 0;
}

void round_double_str(double arg, char *num_arr, float_el *f_el) {
    double n;
    int t, z;
    double okr = modf(arg, &n);
    long long f = -2;
    f = (long long)n;
    if (f < 0) {
        char *ee = fcvt(n, 100, &t, &z);
        char num[100];
        int ee_len = s21_strlen(ee);
        for (int i = ee_len - 1; ee[i] == '0'; i--) {
            ee_len--;
        }
        int k = 0;
        while (k < ee_len) {
            num[k++] = *ee++;
        }
        num[k] = '\0';
        f_el->i += k;
        s21_strcat(num_arr, num);
    } else {
        char *s = fcvt(okr, 14, &t, &z);
        if ((*s - '0' >= 5 && f % 2 != 0) || (*s - '0' > 5 && f % 2 == 0)) {
          if (f > 0)
            f += 1;
          else
            f -= 1;
        } else if (f % 2 == 0 && *s - '0' == 5) {
            s += 1;
            int q = 0;
            while (*s) {
                if (*s - '0' > 0) q++;
                s += 1;
            }
            if (q != 0) f += 1;
        }
        char pp[100] = {0};
        int k = 0;
        if (f < 0) f *= -1;
        while (f != 0) {
            pp[k++] = f % 10 + '0';
            f /= 10.0;
        }
        f_el->i += k;
        s21_reverse_array(pp, s21_strlen(pp));
        s21_strcat(num_arr, pp);
    }
}

void copy_zero_float(char *num_str, const char *tmp_str, int decnpt, int sign, P_Token t) {
    decnpt *= -1;
    int curr = 0, i = sign ? 1 : 0;
    num_str[i++] = '0';
    num_str[i++] = '.';
    while (curr++ < decnpt) {
        num_str[i++] = '0';
    }
    int j = 0;
    while (decnpt++ < t.precision) {
        num_str[i++] = tmp_str[j++];
    }
}

void processing_x(char *str, char *token_str, va_list argptr, P_Token t, int x) {
    if (p_flag.dot) p_flag.null = 0;
    unsigned long num = get_unsigned_arg(token_str, argptr);
    char result[100] = {0};
    get_hexarr(result, num, x);
    int len = s21_strlen(result);
    s21_reverse_array(result, len);
    t.hex = !x ? 1 : 2;
    write_u_to_string(str, result, len, t);
}

void processing_p(char *str, va_list argptr, P_Token t, int x) {
    if (p_flag.dot) p_flag.null = 0;
    void *p = va_arg(argptr, void*);
    unsigned long num = (unsigned long)p;
    char result[100];
    get_hexarr(result, num, x);
    int len = s21_strlen(result);
    s21_reverse_array(result, len);
    write_p_to_string(str, result, len, t);
}

void processing_n(char *str, va_list argptr) {
    int *arg = va_arg(argptr, int*);
    *arg = s21_strlen(str);
}

void write_p_to_string(char *str, char *result, int len, P_Token t) {
    t.width -= 2;
    init_parameters(&t, len);
    if (!t.precision) {
        if (p_flag.null && !p_flag.minuses) {
            s21_strcat(str, "0x");
            write_width_to_str(str, len, &t);
            s21_strcat(str, result);
        } else if (!p_flag.null && !p_flag.minuses) {
            write_width_to_str(str, len, &t);
            s21_strcat(str, "0x");
            s21_strcat(str, result);
        } else {
            s21_strcat(str, "0x");
            s21_strcat(str, result);
            write_width_to_str(str, len, &t);
        }
    }  else {
        write_width_to_str(str, len, &t);
        s21_strcat(str, "0x");
        write_pres_to_str(str, &t);
        s21_strcat(str, result);
    }
}

void *get_hexarr(char *arr, unsigned long num, int x) {
    int index = 0;
    int bletters = 55, sletters = 87, hexbase = 16;
    unsigned long let_min = 10;
    while (num != 0) {
        unsigned long curr = num % hexbase;
        if (curr >= let_min) {
            arr[index] = x ? curr + bletters : curr + sletters;
        } else {
            arr[index] = curr + '0';
        }
        index++;
        num /= hexbase;
    }
    arr[index] = '\0';
    return arr;
}

void processing_o(char *str, char *token_str, va_list argptr, P_Token t) {
    if (p_flag.dot) p_flag.null = 0;
    unsigned long tmp = get_unsigned_arg(token_str, argptr);
    char result[100];
    int index = 0;
    while (tmp != 0) {
        result[index] = tmp % 8 + '0';
        index++;
        tmp /= 8;
    }
    result[index] = '\0';
    t.oct = 1;
    int len = s21_strlen(result);
    s21_reverse_array(result, len);
    write_u_to_string(str, result, len, t);
}


void s21_reverse_array(char *a, int n) {
  int length = n % 2 == 0 ? n / 2 : n / 2 + 1;
  for (int i = 0; i < length; i++) {
      char tmp = a[i];
      int next = i + 1;
      a[i] = a[n - next];
      a[n - next] = tmp;
  }
}


unsigned long get_unsigned_arg(char *token_str, va_list argptr) {
    unsigned long arg;
    int token_len = s21_strlen(token_str);
    if (p_flag.length) {
        if (token_str[token_len - 2] == 'l') {
            arg = va_arg(argptr, unsigned long);
        } else if (token_str[token_len - 2] == 'h') {
            arg = (short) va_arg(argptr, unsigned);
        } else {
            arg = va_arg(argptr, unsigned);
        }
    } else {
        arg = va_arg(argptr, unsigned);
    }
    return arg;
}

void write_minus_to_str(char *str, int sign, P_Token *t) {
  if (sign) {
    s21_strcat(str, "-");
    t->width -= 1;
  }
}

void write_f_nothing(char *str, char *num, int sign, int len, P_Token *t) {
    if (p_flag.null) {
        write_minus_to_str(str, sign, t);
        write_width_to_str(str, len, t);
        s21_strcat(str, num);
      } else {
        if (sign) t->width -=1;
        write_width_to_str(str, len, t);
        write_minus_to_str(str, sign, t);
        s21_strcat(str, num);
      }
}

void write_f_pluses(char *str, char *num, int sign, int len, P_Token *t) {
    t->width -= 1;
    write_width_to_str(str, len, t);
    if (!sign)
        write_plus_to_str(str, t);
    else
        s21_strcat(str, "-");
    s21_strcat(str, num);
}

void write_f_pluses_spaces(char *str, char *num, int sign, int len, P_Token *t) {
    if (!sign)
        write_space_to_str(str, t);
    else
        write_minus_to_str(str, sign, t);
    write_width_to_str(str, len, t);
    s21_strcat(str, num);
}

void write_f_minuses_pluses(char *str, char *num, int sign, int len, P_Token *t) {
    if (!sign)
      write_plus_to_str(str, t);
    else
      write_minus_to_str(str, sign, t);
    s21_strcat(str, num);
    write_width_to_str(str, len, t);
}

void write_f_minuses_spaces(char *str, char *num, int sign, int len, P_Token *t) {
    if (!sign)
      write_space_to_str(str, t);
    else
      write_minus_to_str(str, sign, t);
    s21_strcat(str, num);
    write_width_to_str(str, len, t);
}

void write_f_to_string(char *str, char *num, int sign, P_Token *t) {
    int len = s21_strlen(num);
    if (!p_flag.minuses && !p_flag.pluses && !p_flag.spaces) {
      write_f_nothing(str, num, sign, len, t);
    } else if (!p_flag.minuses && p_flag.pluses && !p_flag.spaces) {
      write_f_pluses(str, num, sign, len, t);
    } else if (!p_flag.minuses && (p_flag.pluses || p_flag.spaces)) {
      write_f_pluses_spaces(str, num, sign, len, t);
    } else if (p_flag.minuses && p_flag.pluses) {
      write_f_minuses_pluses(str, num, sign, len, t);
    } else if (p_flag.minuses && p_flag.spaces) {
      write_f_minuses_spaces(str, num, sign, len, t);
    } else {
        write_minus_to_str(str, sign, t);
        s21_strcat(str, num);
        write_width_to_str(str, len, t);
    }
}

void add_nulls_to_str(char *str, int float_len, P_Token t) {
    if (t.pres_flag == -1) {
        while (float_len < 6) {
            s21_strcat(str, "0");
            float_len++;
        }
    } else {
        while (float_len < t.precision && t.precision <= 15 && !t.null_flag && !p_flag.length) {
            s21_strcat(str, "0");
            float_len++;
        }
        while ((float_len < t.precision) && !t.null_flag && p_flag.length) {
            s21_strcat(str, "0");
            float_len++;
        }
    }
}

void check_precision(P_Token *t) {
    if (!t->precision && !t->null_flag) {
        t->precision = 6;
        t->pres_flag = -1;
    }
    if (!t->precision && t->null_flag == 1) {
        t->precision = 1;
    }
}

int get_float_len_sprintf(char *str, int start_index) {
    int res = 0;
    int len = s21_strlen(str);
    for (int i = start_index; i < len; i++) {
        res++;
    }
    return res;
}

char* get_float_copy(char *dest, const char *source, int start_index, int end) {
    int j = 0;
    for (int i = start_index; i < end; i++) {
        dest[j] = source[i];
        j++;
    }
    dest[j] = '\0';
    return dest;
}

void init_parameters(P_Token *t, int len) {
    t->precision = t->precision - len;
    t->precision = t->precision < 0 ? 0 : t->precision;
    t->width = t->width - t->precision;
}

void write_d_to_string(char *str, char *arg, int len, P_Token t) {
    char copy[10000] = {0};
    init_parameters(&t, len);
    write_pres_to_str(copy, &t);
    s21_strcat(copy, arg);
    if (!p_flag.minuses && !p_flag.spaces && !t.width && !t.precision) {
        write_plus_to_str(str, &t);
        s21_strcat(str, copy);
    } else if (!p_flag.minuses && !p_flag.pluses && !p_flag.spaces) {
        write_width_to_str(str, len, &t);
        s21_strcat(str, copy);
    } else if (p_flag.minuses && !p_flag.spaces && p_flag.pluses) {
        write_plus_to_str(str, &t);
        s21_strcat(str, copy);
        write_width_to_str(str, len, &t);
    } else if (p_flag.minuses && p_flag.spaces && !p_flag.pluses) {
        write_space_to_str(str, &t);
        s21_strcat(str, copy);
        write_width_to_str(str, len, &t);
    } else if (p_flag.minuses && !p_flag.spaces && !p_flag.pluses) {
        s21_strcat(str, copy);
        write_width_to_str(str, len, &t);
    } else if (!p_flag.minuses && p_flag.spaces && !p_flag.pluses) {
        t.width -= 1;
        s21_strcat(str, " ");
        write_width_to_str(str, len, &t);
        s21_strcat(str, copy);
    } else if (!p_flag.minuses && (p_flag.spaces || p_flag.pluses)) {
        t.width = p_flag.spaces || p_flag.pluses ? t.width - 1 : t.width;
        write_width_to_str(str, len, &t);
        write_plus_to_str(str, &t);
        s21_strcat(str, copy);
    } else {
        s21_strcat(str, arg);
    }
}

void check_grid_u(char *str, int len, P_Token *t) {
    if (p_flag.grid) {
        if (t->hex == 2) {
            s21_strcat(str, "0X");
            t->width -= 2;
        }
        if (t->hex == 1) {
            s21_strcat(str, "0x");
            t->width -= 2;
        }
        if (t->oct) {
            if (t->precision <= len) {
                 t->precision -= 1;
                s21_strcat(str, "0");
            }
            if (t->precision < 0) t->width -= 1;
        }
    }
}

void write_u_to_string(char *str, char *arg, int len, P_Token t) {
    char copy[10000];
    s21_clear(copy);
    init_parameters(&t, len);
    if (len != 0) check_grid_u(copy, len, &t);
    write_pres_to_str(copy, &t);
    s21_strcat(copy, arg);
    if (!p_flag.minuses) {
        write_width_to_str(str, len, &t);
        s21_strcat(str, copy);
    } else {
        s21_strcat(str, copy);
        write_width_to_str(str, len, &t);
    }
}

void write_width_to_str(char *str, int len, P_Token *t) {
    int width_len = t->width - len;
    while (width_len > 0) {
        p_flag.null && !p_flag.minuses ? s21_strcat(str, "0") : s21_strcat(str, " ");
        width_len--;
    }
}

void write_pres_to_str(char *str, P_Token *t) {
    while (t->precision > 0) {
        s21_strcat(str, "0");
        t->precision--;
    }
}

void write_plus_to_str(char *str, P_Token *t) {
    if (p_flag.pluses) {
        s21_strcat(str, "+");
        t->width = t->width > 0 ? t->width - 1 : t->width + 1;
    }
}

void write_space_to_str(char *str, P_Token *t) {
    if (p_flag.pluses)
        s21_strcat(str, "+");
    else
        s21_strcat(str, " ");
    t->width = t->width > 0 ? t->width - 1 : t->width + 1;
}

void init_flags() {
  p_flag.pluses = 0;
  p_flag.minuses = 0;
  p_flag.spaces = 0;
  p_flag.length = 0;
  p_flag.star = 0;
  p_flag.null = 0;
  p_flag.grid = 0;
  p_flag.dot = 0;
}

void check_flags(char *token_str) {
    init_flags();
    int i = 0;
    while (!check_specs(token_str[i])) {
        if (token_str[i] == '+') {
            p_flag.pluses = 1;
        }
        if (token_str[i] == '-') {
            p_flag.minuses = 1;
        }
        if (token_str[i]== ' ') {
            p_flag.spaces = 1;
        }
        if (token_str[i] == 'l' || token_str[i] == 'L' || token_str[i] == 'h') {
            p_flag.length = 1;
        }
        if (token_str[i] == '*') {
            p_flag.star = 1;
        }
        if (token_str[i] == '0' && (!is_dec(token_str[i - 1]) && token_str[i -1] != '.')) {
            p_flag.null = 1;
        }
        if (token_str[i] == '#') {
            p_flag.grid = 1;
        }
        if (token_str[i] == '.') {
            p_flag.dot = 1;
        }
        i++;
    }
}

void processing_c(char *str, char *token_str, va_list argptr, P_Token t) {
    char chr_str[10000] = {0};
    int token_len = s21_strlen(token_str);
    if (p_flag.length) {
        if (token_str[token_len - 2] == 'l') {
            wchar_t tmp = va_arg(argptr, wchar_t);
            wchar_t s[10] = {0};
            s[0] = tmp;
            s21_wcharcat(chr_str, s);
        }
    } else {
        char tmp = va_arg(argptr, int);
        char s[10] = {0};
        s[0] = tmp;
        s21_strcat(chr_str, s);
    }
    int len = s21_strlen(chr_str);
    if (p_flag.minuses) {
        s21_strcat(str, chr_str);
        write_width_to_str(str, len, &t);
    } else {
        write_width_to_str(str, len, &t);
        s21_strcat(str, chr_str);
    }
}

void processing_s(char *str, char *token_str, va_list argptr, P_Token t) {
    char string[100000] = {0};
    if (p_flag.length) {
        int token_len = s21_strlen(token_str);
        if (token_str[token_len - 2] == 'l') {
            wchar_t *s = va_arg(argptr, wchar_t*);
            s21_wcharcat(string, s);
        }
    } else {
        char *s = va_arg(argptr, char*);
        s21_strcat(string, s);
    }
    int len = s21_strlen(string);
    if (t.precision > len || (!t.precision && !t.null_flag)) {
        if (p_flag.minuses) {
            s21_strcat(str, string);
            write_width_to_str(str, len, &t);
        } else {
            write_width_to_str(str, len, &t);
            s21_strcat(str, string);
        }
    } else {
        if (p_flag.minuses && !t.null_flag) {
            s21_strncat(str, string, t.precision);
            write_width_to_str(str, t.precision, &t);
        } else {
            write_width_to_str(str, t.precision, &t);
            s21_strncat(str, string, t.precision);
        }
    }
}

void processing_u(char *str, char *token_str, va_list argptr, P_Token t) {
    if (p_flag.dot) p_flag.null = 0;
    unsigned long tmp;
    int token_len = s21_strlen(token_str);
    if (p_flag.length) {
        if (token_str[token_len - 2] == 'l') {
            tmp = va_arg(argptr, unsigned long);
        } else if (token_str[token_len - 2] == 'h') {
            tmp = (short) va_arg(argptr, unsigned);
        } else {
            tmp = va_arg(argptr, unsigned);
        }
    } else {
        tmp = va_arg(argptr, unsigned);
    }
    char *num = s21_itoa(tmp);
    int len = s21_strlen(num);
    write_u_to_string(str, num, len, t);
}

void processing_percent(char *str, P_Token t) {
    char *percent = "%";
    if (!p_flag.minuses) {
        write_width_to_str(str, 1, &t);
        s21_strcat(str, percent);
    } else {
        s21_strcat(str, percent);
        write_width_to_str(str, 1, &t);
    }
}

char *s21_wcharcat(char *destination, const wchar_t *source) {
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

P_Token init_token() {
    P_Token t;
    t.width = 0;
    t.precision = 0;
    t.null_flag = 0;
    t.oct = 0;
    t.hex = 0;
    t.pres_flag = 0;
    t.str_len = 0;
    t.exp = 0;
    t.g = 0;
    return t;
}

void get_width_pres(char *token_str, va_list argptr, P_Token *t) {
    if (p_flag.star) {
        read_star_width_pres(token_str, argptr, t);
    } else {
        read_nostar_width_pres(token_str, t);
    }
}

void read_nostar_width_pres(char *token_str, P_Token *t) {
    while (*token_str) {
      if (is_dec(*token_str) && *(token_str - 1) != '.') {
          t->width = !p_flag.minuses ? atoi(token_str) : atoi(token_str) * (-1);
          int w_len = get_num_len(t->width);
          while (*token_str == '0') {
              token_str += 1;
          }
          token_str += w_len;
      } else if (*token_str == '.' && *(token_str + 1) == '0') {
          while (*token_str == '0') {
              token_str += 1;
          }
          if (!is_dec(*token_str)) {
            t->precision = 0;
            t->null_flag = 1;
            token_str += 2;
          } else {
              t->precision = atoi(token_str);
              int p_len = get_num_len(t->precision);
              token_str += p_len;
          }
      } else if (*token_str == '.' && !is_dec(*(token_str + 1))) {
          t->precision = 0;
          t->null_flag = 2;
          token_str += 1;
      } else if (is_dec(*token_str) && *(token_str - 1) == '.') {
          t->precision = atoi(token_str);
          int p_len = get_num_len(t->precision);
          token_str += p_len;
      } else {
          token_str += 1;
      }
    }
}

void read_star_width_pres(char *token_str, va_list argptr, P_Token *t) {
    while (*token_str) {
        if (*token_str == '*' && *(token_str + 1) == '.' && *(token_str+2) == '*') {
            t->width = va_arg(argptr, int);
            t->precision = va_arg(argptr, int);
            token_str += 3;
        } else if (*token_str == '*' && *(token_str + 1) == '.' &&  *(token_str + 2) == '0') {
            t->width = va_arg(argptr, int);
            token_str += 2;
            token_str += skip_nulls(token_str);
            if (!is_dec(*token_str)) {
                t->precision = 0;
                t->null_flag = 2;
                token_str += 1;
            } else {
                t->precision = atoi(token_str);
                int p_len = get_num_len(t->precision);
                token_str += p_len;
            }
        } else if (*token_str == '*' && *(token_str + 1) == '.' && is_dec(*(token_str+2))) {
            t->width = va_arg(argptr, int);
            t->precision = atoi(token_str + 2);
            int p_len = get_num_len(t->precision);
            token_str += p_len + 2;
        } else if (*token_str == '*' && *(token_str + 1) == '.' && !is_dec(*(token_str + 2))) {
            t->width = va_arg(argptr, int);
            t->precision = 0;
            t->null_flag = 1;
            token_str += 2;
        } else if (*token_str == '*' && *(token_str + 1) != '.') {
            t->width = va_arg(argptr, int);
            t->precision = 0;
            token_str += 1;
        } else if (*token_str == '.' && *(token_str + 1) == '*') {
            t->width = 0;
            t->precision = va_arg(argptr, int);
            token_str += 2;
        } else if (is_dec(*token_str)) {
            token_str += skip_nulls(token_str);
            t->width = atoi(token_str);
            int w_len = get_num_len(t->width);
            token_str += w_len;
            if (*token_str == '.' && *(token_str + 1) == '*') {
                t->precision = va_arg(argptr, int);
                token_str += 2;
            }
        } else {
            token_str += 1;
        }
    }
}

int skip_nulls(char *str) {
    int displace = 0;
    while (*str == '0') {
        displace++;
        str += 1;
    }
    return displace;
}

char *s21_itoa(long  num) {
    static char res[30];
    s21_clear(res);
    int i = 0;
    long n = num;
    while (n != 0) {
        if (n < 0) {
            res[i] = (n *(-1) % 10) + '0';
        } else {
            res[i] = (n % 10) + '0';
        }
        n /= 10;
        i++;
    }
    if (num < 0) {
        res[i] = '-';
    }
    s21_reverse_array(res, (int)s21_strlen(res));
    return res;
}

int get_num_len(long long number) {
    int len = 0;
    while (number != 0) {
        len++;
        number /= 10;
    }
    return len;
}


int check_string(const char *str) {
    int res = 0, i = 1;
    while (str[i] != '\0') {
        if (check_specs(str[i])) {
            res = 1;
            break;
        }
        i++;
    }
    return res;
}
