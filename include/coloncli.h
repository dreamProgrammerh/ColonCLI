#ifndef CCLI
#   ifdef COLON_CLI_H
#      error "coloncli.h with no CCLI can be only used once per project!"
#   endif
#   define COLON_CLI_H 1
#   define CCLI ccli
#endif

#include <stdarg.h>
#include <limits.h>

#define __C(a,b,c) a ## b ## c
#define _C(a,b,c) __C(a, b, c)
#define __S(x) #x
#define _S(x) __S(x)
#define __R(x) x
#define _R(x) __R(x)
#define _A_(pre, suf) _C(pre, CCLI, suf)

#ifndef CCLI_LEGACY
#   define CCLI_LEGACY 0
#endif
#ifndef CCLI_POSITIONAL
#   define CCLI_POSITIONAL 0
#endif

#ifndef CCLI_NAME
#   define CCLI_NAME _S(CCLI)
#endif
#ifndef CCLI_HEADER
#   define CCLI_HEADER CCLI_NAME " - Colon CLI"
#endif
#ifndef CCLI_USAGE
#   define CCLI_USAGE "Usage: " CCLI_NAME " [OPTIONS] :: [ARGS...]"
#endif
#ifndef CCLI_DESCRIPTION
#   define CCLI_DESCRIPTION ""
#endif
#ifndef CCLI_EXAMPLE
#   define CCLI_EXAMPLE ""
#endif

#ifndef CCLI_HELP_TYPE
#   define CCLI_HELP_TYPE 1
#endif
#ifndef CCLI_HELP_DEFAULT
#   define CCLI_HELP_DEFAULT 1
#endif
#ifndef CCLI_HELP_LABEL
#   define CCLI_HELP_LABEL 1
#endif
#ifndef CCLI_HELP_DESC
#   define CCLI_HELP_DESC 1
#endif
#ifndef CCLI_HELP_ALIGN
#   define CCLI_HELP_ALIGN 1
#endif

#define CCLI_X(...) (__VA_ARGS__)
#define CCLI_Y(...) {__VA_ARGS__}
#define CCLI_NULL ((void*)0u)

#define CCLI_O_INVALID 0u
#define CCLI_O_SKIP 1u
#define CCLI_O_KEY 2u
#define CCLI_O_ON 3u
#define CCLI_O_OFF 4u
#define CCLI_O_ON_OR_KEY 5u

#define CCLI_B_TRUE 1u
#define CCLI_B_FALSE 0u
#define CCLI_B_YES 1u
#define CCLI_B_NO 0u
#define CCLI_B_ CCLI_D_FALSE

#define CCLI_BOOL(def) CCLI_B_##def

#define CCLI_T_UINT unsigned int
#define CCLI_T_INT signed int
#define CCLI_T_STRING _A_(,_string_t)
#define CCLI_T_CHAR char
#define CCLI_T_ CCLI_T_STRING

#define CCLI_TYPE(type) CCLI_T_##type

#define CCLI_S_UINT(...) _A_(_,_sb_ap_u32)(__VA_ARGS__)
#define CCLI_S_INT(...) _A_(_,_sb_ap_i32)(__VA_ARGS__)
#define CCLI_S_STRING(sb, ...) _A_(_,_sb_ap_chr)(sb, '"');_A_(_,_sb_ap)(sb,__VA_ARGS__);_A_(_,_sb_ap_chr)(sb, '"')
#define CCLI_S_CHAR(...) _A_(_,_sb_ap_chr)(__VA_ARGS__)
#define CCLI_S_(...) CCLI_P_STRING(__VA_ARGS__)

#define CCLI_SB(type) CCLI_S_##type

#define CCLI_E_UINT(val) val
#define CCLI_E_INT(val) val
#define CCLI_E_STRING(val) val.data, val.len
#define CCLI_E_CHAR(val) val
#define CCLI_E_(val) CCLI_E_STRING(val)

#define CCLI_EXPEND(type, prop) CCLI_E_##type(prop)

#define CCLI_A_UINT(default) default
#define CCLI_A_INT(default) default
#define CCLI_A_STRING(default) (_A_(,_string_t)){.data=default,.len=sizeof(default)-1}
#define CCLI_A_CHAR(default) (char)default
#define CCLI_A_(default) CCLI_A_STRING(default)

#define CCLI_ASSIGN(type, prop, default) prop = (CCLI_A_##type(default))

#define CCLI_SUPPRESS \
    (void) _A_(_,_sb_ap_chr_r); \
    (void) _A_(_,_read_UINT); \
    (void) _A_(_,_read_INT); \
    (void) _A_(_,_read_STRING); \
    (void) _A_(_,_read_CHAR); \
    (void) _A_(_,_read_); \
    (void) _A_(_,_colon_oneof); \
    (void) _A_(_,_legacy_oneof);

#define CCLI_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define CCLI_RSEQ_N() 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 1
#define CCLI_VA_COUNT(...) _CCLI_VA_COUNT(__VA_ARGS__, CCLI_RSEQ_N())
#define _CCLI_VA_COUNT(...) CCLI_ARG_N(__VA_ARGS__)

typedef struct _A_(_,_sb) _A_(_,_sb);
typedef struct _A_(,_args_t) _A_(,_args_t);

typedef struct {
    unsigned int len;
    const char* data;
} _A_(,_string_t);

struct _A_(_,_sb) {
    char* data;
    unsigned int len;
    unsigned int cap;
};

struct _A_(,_args_t) {

    #ifdef CCLI_POS_ARGS
    #   define _X(...)
    #   define _Y(cap, type, prop, ...) struct {CCLI_TYPE(type) items[cap]; unsigned int count;} prop;
    CCLI_POS_ARGS(_X, _Y)
    #   undef _X
    #   undef _Y
    #   define _X(type, prop, ...) CCLI_TYPE(type) prop;
    #   define _Y(...)
    CCLI_POS_ARGS(_X, _Y)
    #   undef _X
    #   undef _Y
    #endif

    #ifdef CCLI_OPT_ARGS
    #   define _X(type, prop, ...) CCLI_TYPE(type) prop;
    CCLI_OPT_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    #ifdef CCLI_BOL_ARGS
    #   define _X(prop, ...) unsigned char prop;
    CCLI_BOL_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    _A_(_,_sb) sberr;
    unsigned int level; // level of positional arg that have been read
    unsigned char ok; // if the cli went ok without a report
};

static int _A_(_,_sb_ap)(_A_(_,_sb)* sb, const char* data, unsigned int len) {
    if (sb->len + len > sb->cap) return 1;
    for (unsigned int i = 0; i < len; i++) sb->data[sb->len++] = data[i];
    return 0;
}

static int _A_(_,_sb_ap_str)(_A_(_,_sb)* sb, const char* data) {
    unsigned int len = 0;
    while (data[len]) len++;
    if (sb->len + len > sb->cap) return 1;
    for (unsigned int i = 0; i < len; i++) sb->data[sb->len++] = data[i];
    return 0;
}

static int _A_(_,_sb_ap_chr)(_A_(_,_sb)* sb, char ch) {
    if (sb->len + 1 > sb->cap) return 1;
    sb->data[sb->len++] = ch;
    return 0;
}

static int _A_(_,_sb_ap_chr_r)(_A_(_,_sb)* sb, char ch, unsigned int rep) {
    if (sb->len + rep > sb->cap) return 1;
    for (unsigned int r = 0; r < rep; r++)
        sb->data[sb->len++] = ch;
    return 0;
}

static int _A_(_,_sb_ap_i32)(_A_(_,_sb)* sb, int val) {
    char buf[12];
    char tmp[10];
    unsigned int tidx = 0;
    unsigned int len = 0;
    if (val < 0) {
        if (_A_(_,_sb_ap_chr)(sb, '-')) return 1;
        unsigned int uval = (unsigned int)(-(val + 1)) + 1u;
        while (uval > 0) { tmp[tidx++] = (char)('0' + uval % 10); uval /= 10; }
    } else {
        unsigned int uval = (unsigned int)val;
        while (uval > 0) { tmp[tidx++] = (char)('0' + uval % 10); uval /= 10; }
    }
    if (tidx == 0) tmp[tidx++] = '0';
    len = tidx;
    for (unsigned int j = 0; j < tidx; j++) buf[j] = tmp[tidx - 1 - j];
    return _A_(_,_sb_ap)(sb, buf, len);
}

static int _A_(_,_sb_ap_u32)(_A_(_,_sb)* sb, unsigned int val) {
    char buf[10];
    char tmp[10];
    unsigned int tidx = 0;
    unsigned int len = 0;
    while (val > 0) { tmp[tidx++] = (char)('0' + val % 10); val /= 10; }
    if (tidx == 0) tmp[tidx++] = '0';
    len = tidx;
    for (unsigned int j = 0; j < tidx; j++) buf[j] = tmp[tidx - 1 - j];
    return _A_(_,_sb_ap)(sb, buf, len);
}

static int _A_(_,_sb_ap_i64)(_A_(_,_sb)* sb, long long val) {
    char buf[20];
    char tmp[20];
    unsigned int tidx = 0;
    unsigned int len = 0;
    if (val < 0) {
        if (_A_(_,_sb_ap_chr)(sb, '-')) return 1;
        unsigned long long uval = (unsigned long long)(-(val + 1)) + 1ull;
        while (uval > 0) { tmp[tidx++] = (char)('0' + uval % 10); uval /= 10; }
    } else {
        unsigned long long uval = (unsigned long long)val;
        while (uval > 0) { tmp[tidx++] = (char)('0' + uval % 10); uval /= 10; }
    }
    if (tidx == 0) tmp[tidx++] = '0';
    len = tidx;
    for (unsigned int j = 0; j < tidx; j++) buf[j] = tmp[tidx - 1 - j];
    return _A_(_,_sb_ap)(sb, buf, len);
}

static int _A_(_,_sb_ap_u64)(_A_(_,_sb)* sb, unsigned long long val) {
    char buf[20];
    char tmp[20];
    unsigned int tidx = 0;
    unsigned int len = 0;
    while (val > 0) { tmp[tidx++] = (char)('0' + val % 10); val /= 10; }
    if (tidx == 0) tmp[tidx++] = '0';
    len = tidx;
    for (unsigned int j = 0; j < tidx; j++) buf[j] = tmp[tidx - 1 - j];
    return _A_(_,_sb_ap)(sb, buf, len);
}

static int _A_(_,_sb_ap_f)(_A_(_,_sb)* sb, const char* fmt, va_list args) {
    for (unsigned int i = 0; fmt[i]; i++) {
        if (fmt[i] != '$') {
            if (_A_(_,_sb_ap_chr)(sb, fmt[i])) return 1;
            continue;
        }

        i++;
        if (!fmt[i]) {
            if (_A_(_,_sb_ap_chr)(sb, '$')) return 1;
            break;
        }

        char spec = fmt[i];

        switch (spec) {
            case '$':
                if (_A_(_,_sb_ap_chr)(sb, '$')) return 1;
                break;
            case 's': {
                const char* s = va_arg(args, const char*);
                if (_A_(_,_sb_ap_str)(sb, s)) return 1;
                break;
            }
            case 'n': {
                const char* s = va_arg(args, const char*);
                unsigned int n = va_arg(args, unsigned int);
                if (_A_(_,_sb_ap)(sb, s, n)) return 1;
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                if (_A_(_,_sb_ap_chr)(sb, c)) return 1;
                break;
            }
            case 'i': {
                int val = va_arg(args, int);
                if (_A_(_,_sb_ap_i32)(sb, val)) return 1;
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                if (_A_(_,_sb_ap_u32)(sb, val)) return 1;
                break;
            }
            case 'I': {
                long long val = va_arg(args, long long);
                if (_A_(_,_sb_ap_i64)(sb, val)) return 1;
                break;
            }
            case 'U': {
                unsigned long long val = va_arg(args, unsigned long long);
                if (_A_(_,_sb_ap_u64)(sb, val)) return 1;
                break;
            }
            case '*': {
                const char* s = va_arg(args, const char*);
                if (_A_(_,_sb_ap_f)(sb, s, args)) return 1;
                break;
            }
            default: {
                if (_A_(_,_sb_ap_chr)(sb, '$')) return 1;
                if (_A_(_,_sb_ap_chr)(sb, spec)) return 1;
                break;
            }
        }
    }

    return 0;
}

static unsigned int _A_(_,_strlen)(const char* str) {
    unsigned int l = 0;
    while (str[l]) l++;
    return l;
}

static void _A_(_,_report)(_A_(,_args_t)* cli, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _A_(_,_sb_ap_f)(&cli->sberr, fmt, args);
    va_end(args);
    _A_(_,_sb_ap_chr)(&cli->sberr, '\n');
    cli->ok = 0;
}

static int _A_(_,_colon_oneof)(const char* arg, unsigned int len, const char** keys, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) {
        const char* key = keys[i];
        unsigned int j = 0;
        for (; key[j] && j < len; ++j) {
            if (key[j] != arg[j]) goto notmatch;
        }
        if (j == len && !key[j]) return 1; // if same len
        notmatch: continue;
    }
    return 0;
}

static int _A_(_,_legacy_oneof)(const char* arg, unsigned int len, int short_form, const char** keys, unsigned int count) {
    if (short_form) {
        for (unsigned int i = 0; i < count; ++i) {
            const char* key = keys[i];
            if (key[0] && key[1]) continue; // skip if non-single char
            if (key[0] == arg[0]) return 1;
        }

        return 0;
    }

    for (unsigned int i = 0; i < count; ++i) {
        const char* key = keys[i];
        unsigned int j = 0;
        for (; key[j] && j < len; ++j) {
            if (key[j] != arg[j]) goto notmatch;
        }
        if (j == len && !key[j]) return 1; // if same len
        notmatch: continue;
    }

    return 0;
}

static unsigned int _A_(_,_read_UINT)(_A_(,_args_t)* cli, const char* key, unsigned int key_len, int positional, const char* str, unsigned int len) {
    // u32 (sizeof("4294967295") - 1) = 10
    if (len > 10) {
        if (positional)
            _A_(_,_report)(cli, "Error: position ($u): value '$n' is beyond the limit for unsigned int (max 10 digits)", cli->level, str, len);
        else
            _A_(_,_report)(cli, "Error: option '$n': value '$n' is beyond the limit for unsigned int (max 10 digits)", key, key_len, str, len);
        return 0;
    }

    unsigned int n = 0;
    for (unsigned int i = 0; i < len; ++i) {
        char c = str[i];
        if (!(('9' >= c) & (c >= '0'))) {
            if (positional)
                _A_(_,_report)(cli, "Error: position ($u): invalid character '$c' in unsigned int value '$n'", cli->level, c, str, len);
            else
                _A_(_,_report)(cli, "Error: option '$n': invalid character '$c' in unsigned int value '$n'", key, key_len, c, str, len);
            return 0;
        }

        unsigned int digit = (unsigned int)c - (unsigned int)'0';
        if (n > (UINT_MAX - digit) / 10u) {
            if (positional)
                _A_(_,_report)(cli, "Error: position ($u): value '$n' overflows unsigned int (max $u)", cli->level, str, len, UINT_MAX);
            else
                _A_(_,_report)(cli, "Error: option '$n': value '$n' overflows unsigned int (max $u)", key, key_len, str, len, UINT_MAX);
            return 0;
        }

        n = n * 10 + digit;
    }

    return n;
}

static signed int _A_(_,_read_INT)(_A_(,_args_t)* cli, const char* key, unsigned int key_len, int positional, const char* str, unsigned int len) {
    // i32 (sizeof("-2147483647") - 1) = 11
    int sign = str[0] == '-';
    if (len > 11 || (sign && len > 10)) {
        if (positional)
            _A_(_,_report)(cli, "Error: position ($u): value '$n' is beyond the limit for signed int (max 11 digits)", cli->level, str, len);
        else
            _A_(_,_report)(cli, "Error: option '$n': value '$n' is beyond the limit for signed int (max 11 digits)", key, key_len, str, len);
        return 0;
    }

    unsigned int n = 0;

    unsigned int i = 0;
    if (sign || str[0] == '+') i++;

    if (i == len) {
        if (positional)
            _A_(_,_report)(cli, "Error: position ($u): invalid signed int value '$n' (missing digits after sign)", cli->level, str, len);
        else
            _A_(_,_report)(cli, "Error: option '$n': invalid signed int value '$n' (missing digits after sign)", key, key_len, str, len);
        return 0;
    }

    for (; i < len; ++i) {
        char c = str[i];
        if (!(('9' >= c) & (c >= '0'))) {
            if (positional)
                _A_(_,_report)(cli, "Error: position ($u): invalid character '$c' in signed int value '$n'", cli->level, c, str, len);
            else
                _A_(_,_report)(cli, "Error: option '$n': invalid character '$c' in signed int value '$n'", key, key_len, c, str, len);
            return 0;
        }

        unsigned int digit = (unsigned int)c - (unsigned int)'0';
        if (n > (UINT_MAX - digit) / 10u) {
            if (positional)
                _A_(_,_report)(cli, "Error: position ($u): value '$n' overflows signed int (max $i)", cli->level, str, len, INT_MAX);
            else
                _A_(_,_report)(cli, "Error: option '$n': value '$n' overflows signed int (max $i)", key, key_len, str, len, INT_MAX);
            return 0;
        }

        n = n * 10 + digit;
    }

    if (sign) {
        if (n > (unsigned int)INT_MAX + 1u) {
            if (positional)
                _A_(_,_report)(cli, "Error: position ($u): value '$n' underflows signed int (min $i)", cli->level, str, len, INT_MIN);
            else
                _A_(_,_report)(cli, "Error: option '$n': value '$n' underflows signed int (min $i)", key, key_len, str, len, INT_MIN);
            return 0;
        }
        return (signed int)(0u - n);
    }
    if (n > (unsigned int)INT_MAX) {
        if (positional)
            _A_(_,_report)(cli, "Error: position ($u): value '$n' overflows signed int (max $i)", cli->level, str, len, INT_MAX);
        else
            _A_(_,_report)(cli, "Error: option '$n': value '$n' overflows signed int (max $i)", key, key_len, str, len, INT_MAX);
        return 0;
    }
    return (signed int)n;
}

static _A_(,_string_t) _A_(_,_read_STRING)(_A_(,_args_t)* cli, const char* key, unsigned int key_len, int positional, const char* str, unsigned int len) {
    (void) cli;
    (void) key;
    (void) key_len;
    (void) positional;
    return (_A_(,_string_t)) {.len=len,.data=str};
}

static char _A_(_,_read_CHAR)(_A_(,_args_t)* cli, const char* key, unsigned int key_len, int positional, const char* str, unsigned int len) {
    if (len > 1) {
        if (positional)
            _A_(_,_report)(cli, "Error: position ($u): char value '$n' must be only one character (got $u)", cli->level, str, len, len);
        else
            _A_(_,_report)(cli, "Error: option '$n': char value '$n' must be only one character (got $u)", key, key_len, str, len, len);
        return '\0';
    }

    return str[0];
}

static _A_(,_string_t) _A_(_,_read_)(_A_(,_args_t)* cli, const char* key, unsigned int key_len, int positional, const char* str, unsigned int len) {
    return _A_(_,_read_STRING)(cli, key, key_len, positional, str, len);
}

#define CCLI_READ(type) _A_(_,_read_##type)

static unsigned int _A_(_,_colon_key_val_opt)(_A_(,_args_t)* cli,
    unsigned int argc, unsigned int* index, char** argv,
    const char* arg, unsigned int len,
    const char** out_key, unsigned int* out_key_len,
    const char** out_val, unsigned int* out_val_len
) {
    *out_key = arg;
    *out_val = arg;
    *out_key_len = len;
    *out_val_len = 0;

    if (arg[0] == ':') {
        if (arg[len-1] == ':') { // disable bool
            *out_key = arg + 1;
            *out_key_len = len - 2;
            return CCLI_O_OFF;
        } else { // enable bool
            *out_key = arg + 1;
            *out_key_len = len - 1;
            return CCLI_O_ON;
        }
    } else if (len > 1) { // key
        for (unsigned int j = 1; j < len - 1; ++j) {
            if (arg[j] != ':') continue;
            *out_key = arg;
            *out_key_len = j;
            *out_val = &(*out_key)[j + 1];
            *out_val_len = len - j - 1;
            return CCLI_O_KEY;
        }

        if (arg[len-1] == ':') { // key
            *out_key_len = len - 1;
            (*index)++;
            if (*index >= argc) {
                _A_(_,_report)(cli, "Error: missing value for option '$n' (expected a value after the trailing colon)", arg, len);
                return CCLI_O_SKIP;
            }
            *out_val = argv[*index];
            *out_val_len = _A_(_,_strlen)(*out_val);
            return CCLI_O_KEY;
        }
    }

    return CCLI_O_INVALID;
}

static unsigned int _A_(_,_legacy_key_val_opt)(_A_(,_args_t)* cli,
    unsigned int argc, unsigned int* index, char** argv,
    const char* arg, unsigned int len,
    const char** out_key, unsigned int* out_key_len,
    const char** out_val, unsigned int* out_val_len,
    int* short_form
) {
    *out_key = arg;
    *out_val = arg;
    *out_key_len = len;
    *out_val_len = 0;
    *short_form = 0;

    if (arg[0] == '-') {
        if (arg[1] == '-') {
            arg += 2;
            len -= 2;
            *short_form = 0;
        } else {
            arg += 1;
            len -= 1;
            *short_form = 1;
        }
    } else return CCLI_O_INVALID;

    if (len >= 3 && arg[0] == 'n' && arg[1] == 'o' && arg[2] == '-') {
        if (len == 3) {
            _A_(_,_report)(cli, "Error: empty $sno- flag disable (no flag name after the prefix)", short_form ? "-" : "--");
            return CCLI_O_SKIP;
        }

        *out_key = arg + 3;
        *out_key_len = len - 3;
        return CCLI_O_OFF;
    } else if (len > 1) {
        for (unsigned int j = 1; j < len - 1; ++j) {
            if (arg[j] != '=') continue;
            *out_key = arg;
            *out_key_len = j;
            *out_val = &(*out_key)[j + 1];
            *out_val_len = len - j - 1;
            return CCLI_O_KEY;
        }

        if (arg[len-1] == '=') {
            *out_key = arg;
            *out_key_len = len - 1;
            (*index)++;
            if (*index >= argc) {
                _A_(_,_report)(cli, "Error: missing value for option '$n' (expected a value after '=')", arg, len);
                return CCLI_O_SKIP;
            }
            *out_val = argv[*index];
            *out_val_len = _A_(_,_strlen)(*out_val);
            return CCLI_O_KEY;
        }
    }

    *out_key = arg;
    *out_key_len = len;
    *out_val = CCLI_NULL;
    *out_val_len = 0;
    return CCLI_O_ON_OR_KEY;
}

_A_(,_args_t) _A_(,_default)(void) {
    CCLI_SUPPRESS; // get rid of warnings

    _A_(,_args_t) d = {0};

    #ifdef CCLI_OPT_ARGS
    #   define _X(type, prop, default, ...) d.CCLI_ASSIGN(type, prop, default);
    CCLI_OPT_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    #ifdef CCLI_BOL_ARGS
    #   define _X(prop, default, ...) d.prop = CCLI_BOOL(default);
    CCLI_BOL_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    d.sberr.data = CCLI_NULL;
    d.sberr.cap = 0;
    d.sberr.len = 0;
    d.level = 0;
    d.ok = 0;

    return d;
}

unsigned int _A_(,_parse)(_A_(,_args_t)* cli, char* err_buf, unsigned int err_cap, unsigned argc, char** argv) {
    cli->ok = 1;
    cli->sberr = (_A_(_,_sb)){err_buf, 0, err_cap};

    if (!argc || !argv) goto end;

    #ifdef CCLI_POS_ARGS
    cli->level = 0;
    #endif

    int legacy_mode = !!(CCLI_LEGACY);
    #ifdef CCLI_POS_ARGS
    int positional_mode = !!(CCLI_POSITIONAL);
    #endif

    for (unsigned int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (!arg) continue;

        unsigned int len = _A_(_,_strlen)(arg);

        const char* key = CCLI_NULL;
        const char* val = CCLI_NULL;
        unsigned int key_len;
        unsigned int val_len;

        if (len == 1 && arg[0] == ':') {
            legacy_mode = !legacy_mode;
            continue;
        }

        #define _Y(...) (void*)(char*[]){__VA_ARGS__}

        if (legacy_mode) goto legacy;

        if (len == 2 && arg[0] == ':' && arg[1] == ':') {
            #ifdef CCLI_POS_ARGS
            positional_mode = !positional_mode;
            continue;
            #else
            _A_(_,_report)(cli, "Error: '::' is not supported when no positional arguments are defined");
            goto end;
            #endif
        }

        #ifdef CCLI_POS_ARGS
        if (positional_mode) goto positional;
        #endif

        const unsigned int colon_opt = _A_(_,_colon_key_val_opt)(
            cli, argc, &i, argv,
            arg, len,
            &key, &key_len, &val, &val_len
        );

        if (!cli->ok) goto end;
        if (colon_opt == CCLI_O_SKIP) continue;

        #ifdef CCLI_OPT_ARGS
        #   define _X(type, prop, default, keys, ...) \
            if (_A_(_,_colon_oneof)(key, key_len, keys, CCLI_VA_COUNT(keys))) { \
                cli->prop = CCLI_READ(type)(cli, key, key_len, 0, val, val_len); \
                if (cli->ok) continue; \
                goto end; \
            }

        if (colon_opt == CCLI_O_KEY) {
            CCLI_OPT_ARGS(_X, _Y);

            _A_(_,_report)(cli, "Error: unexpected option '$n'", key, key_len);
            goto end;
        }
        #   undef _X
        #endif

        #ifdef CCLI_BOL_ARGS
        #   define _X(prop, default, keys, ...) \
            if (_A_(_,_colon_oneof)(bol, bol_len, keys, CCLI_VA_COUNT(keys))) { \
                cli->prop = res; \
                goto bol_end; \
            }

        if (colon_opt == CCLI_O_ON || colon_opt == CCLI_O_OFF) {
            const unsigned char res = colon_opt != CCLI_O_OFF;
            unsigned int j = 0, k = 0;
            for (;k <= key_len; ++k) {
                if (key[k] != ':' && k != key_len) continue;
                unsigned int bol_len = k - j;
                if (bol_len == 0) continue;

                const char* bol = key + j;
                CCLI_BOL_ARGS(_X, _Y);

                _A_(_,_report)(cli, res
                    ? "Error: unexpected flag '$n' enable"
                    : "Error: unexpected flag '$n' disable",
                    bol, bol_len
                );
                goto end;

                bol_end: j = ++k;
            }
            continue;
        }
        #   undef _X
        #endif

        #ifdef CCLI_POS_ARGS
        goto positional;
        #else
        continue;
        #endif

        legacy:

        if (len == 2 && arg[0] == '-' && arg[1] == '-') {
            #ifdef CCLI_POS_ARGS
            positional_mode = !positional_mode;
            continue;
            #else
            _A_(_,_report)(cli, "Error: '--' is not supported when no positional arguments are defined");
            goto end;
            #endif
        }

        #ifdef CCLI_POS_ARGS
        if (positional_mode) goto positional;
        #endif

        int short_form;

        const unsigned int legacy_opt = _A_(_,_legacy_key_val_opt)(
            cli, argc, &i, argv,
            arg, len,
            &key, &key_len, &val, &val_len,
            &short_form
        );

        if (!cli->ok) goto end;
        if (legacy_opt == CCLI_O_SKIP) continue;

        #ifdef CCLI_OPT_ARGS
        #   define _X(type, prop, default, keys, ...) \
            if (_A_(_,_legacy_oneof)(sub, sub_len, short_form, keys, CCLI_VA_COUNT(keys))) { \
                found = 1; \
                if (!val) { \
                    i++; \
                    if (i >= argc) { \
                        _A_(_,_report)(cli, "Error: missing value for option '$n' (expected a value after the option)", key, key_len); \
                        goto end; \
                    } \
                    val = (void*)argv[i]; \
                    val_len = _A_(_,_strlen)(val); \
                } \
                cli->prop = CCLI_READ(type)(cli, sub, sub_len, 0, val, val_len); \
                if (cli->ok) continue; \
                goto end; \
            }

        if (legacy_opt == CCLI_O_ON_OR_KEY || legacy_opt == CCLI_O_KEY) {
            const char* sub = key;
            const unsigned int inc = short_form;
            const unsigned int sub_len = inc ? 1 : key_len;
            const unsigned int n = inc ? key_len : 1;

            int found = 0;

            for (unsigned int j = 0; j < n; ++j, sub += inc) {
                CCLI_OPT_ARGS(_X, _Y);

        #   ifdef CCLI_BOL_ARGS
                if (found || legacy_opt == CCLI_O_KEY) {
        #   endif
                _A_(_,_report)(cli, "Error: unexpected option '$n'", sub, sub_len);
                goto end;
        #   ifdef CCLI_BOL_ARGS
                }
                break;
        #   endif
            }
            if (found) continue;
        }
        #   undef _X
        #endif

        #ifdef CCLI_BOL_ARGS
        #   define _X(prop, default, keys, ...) \
            if (_A_(_,_legacy_oneof)(sub, sub_len, short_form, keys, CCLI_VA_COUNT(keys))) { \
                cli->prop = res; \
                continue; \
            }

        if (legacy_opt == CCLI_O_ON_OR_KEY || legacy_opt == CCLI_O_OFF) {
            const unsigned char res = legacy_opt != CCLI_O_OFF;

            const unsigned int inc = short_form;
            const char* sub = key;
            const unsigned int sub_len = inc ? 1 : key_len;
            const unsigned int n = inc ? key_len : 1;

            for (unsigned int j = 0; j < n; ++j, sub += inc) {
                CCLI_BOL_ARGS(_X, _Y);

                _A_(_,_report)(cli, res
                    ? "Error: unexpected flag '$n' enable"
                    : "Error: unexpected flag '$n' disable",
                    sub, sub_len
                );

                goto end;
            }

            continue;
        }
        #   undef _X
        #endif

        #undef _Y

        #ifdef CCLI_POS_ARGS
        #   define _X(type, prop, ...) \
            if (cli->level == COUNTER) { \
                cli->prop = CCLI_READ(type)(cli, CCLI_NULL, 0, 1, arg, len); \
                goto pos_end; \
            } \
            COUNTER++;
        #   define _Y(cap, type, prop, ...) \
            if (cli->prop.count != cap) { \
                cli->prop.items[cli->prop.count] = CCLI_READ(type)(cli, CCLI_NULL, 0, 1, arg, len); \
                ++cli->prop.count; \
                continue; \
            }

        positional:;
        unsigned int COUNTER = 0;
        CCLI_POS_ARGS(_X, _Y);

        _A_(_,_report)(cli, "Error: unexpected positional ($u) argument '$n'", cli->level, arg, len);
        goto end;

        pos_end: cli->level++;
        #   undef _X
        #   undef _Y
        #endif
    }


    end:
    return cli->sberr.len;
}

unsigned int _A_(,_help_str)(char* buf, unsigned int cap) {
    _A_(_,_sb) sb = { buf, 0, cap };

    #define CCLI_SB_AP_LIT(s) _A_(_,_sb_ap)(&sb, s, sizeof(s)-1u)
    #define CCLI_SB_AP_LIT_PAD(s, p) do { \
        _A_(_,_sb_ap)(&sb, s, sizeof(s) - 1u); \
        _A_(_,_sb_ap_chr_r)(&sb, ' ', (unsigned int)(p) - (unsigned int)(sizeof(s) - 1u)); \
        } while (0);
    #define CCLI_SB_AP_F(fmt, ...) _A_(_,_sb_ap_f)(&sb, fmt, __VA_ARGS__)

    if ((CCLI_HEADER)[0]) CCLI_SB_AP_LIT(CCLI_HEADER "\n\n");
    if ((CCLI_USAGE)[0]) CCLI_SB_AP_LIT(CCLI_USAGE "\n\n");
    if ((CCLI_DESCRIPTION)[0]) CCLI_SB_AP_LIT(CCLI_DESCRIPTION "\n\n");

    #define _Y(...) __VA_ARGS__
    #define CCLI_Y_OPT1(a) a":"
    #define CCLI_Y_OPT2(a,b) a":"b":"
    #define CCLI_Y_OPT3(a,b,c) a":"b":"c":"
    #define CCLI_Y_OPT4(a,b,c,d) a":"b":"c":"d":"
    #define CCLI_Y_OPT5(a,b,c,d,e) a":"b":"c":"d":"e":"
    #define CCLI_Y_BOL_1u "*"
    #define CCLI_Y_BOL_0u " "
    #define CCLI_Y_BOL10u(a) ":"a
    #define CCLI_Y_BOL20u(a,b) ":"a":"b
    #define CCLI_Y_BOL30u(a,b,c) ":"a":"b":"c
    #define CCLI_Y_BOL40u(a,b,c,d) ":"a":"b":"c":"d
    #define CCLI_Y_BOL50u(a,b,c,d,e) ":"a":"b":"c":"d":"e
    #define CCLI_Y_BOL11u(a) ":"a":"
    #define CCLI_Y_BOL21u(a,b) ":"a":"b":"
    #define CCLI_Y_BOL31u(a,b,c) ":"a":"b":"c":"
    #define CCLI_Y_BOL41u(a,b,c,d) ":"a":"b":"c":"d":"
    #define CCLI_Y_BOL51u(a,b,c,d,e) ":"a":"b":"c":"d":"e":"
    #define CCLI_Y_OPT_SET(...) _C(,CCLI_Y_OPT,CCLI_VA_COUNT(__VA_ARGS__))(__VA_ARGS__)
    #define CCLI_Y_BOL_SET(default, ...) _C(,_C(,CCLI_Y_BOL,CCLI_VA_COUNT(__VA_ARGS__)),_R(CCLI_BOOL(default))) (__VA_ARGS__)
    #define CCLI_Y_BOL_CHK(default) _C(,CCLI_Y_BOL_,_R(CCLI_BOOL(default)))

    #if CCLI_HELP_TYPE
    #   define CCLI_SWALLOW_TYPE(...) __VA_ARGS__
    #else
    #   define CCLI_SWALLOW_TYPE(...)
    #endif

    #if CCLI_HELP_DEFAULT
    #   define CCLI_SWALLOW_DEFAULT(...) __VA_ARGS__
    #else
    #   define CCLI_SWALLOW_DEFAULT(...)
    #endif

    #if CCLI_HELP_LABEL
    #   define CCLI_SWALLOW_LABEL(...) __VA_ARGS__
    #else
    #   define CCLI_SWALLOW_LABEL(...)
    #endif

    #if CCLI_HELP_DESC
    #   define CCLI_SWALLOW_DESC(...) __VA_ARGS__
    #else
    #   define CCLI_SWALLOW_DESC(...)
    #endif

    #if !CCLI_HELP_ALIGN
    _A_(_,_sb_ap_str)(&sb, ""

    #ifdef CCLI_OPT_ARGS
    #   define _X(type, prop, default, keys, label, desc) \
        "  "CCLI_Y_OPT_SET(keys) \
        CCLI_SWALLOW_TYPE("  "#type) \
        CCLI_SWALLOW_LABEL("  "label) \
        CCLI_SWALLOW_DEFAULT("  ["#default"]") \
        CCLI_SWALLOW_DESC("  "desc) "\n"

    "[Options]\n" CCLI_OPT_ARGS(_X, _Y) "\n"
    #   undef _X
    #endif

    #ifdef CCLI_BOL_ARGS
    #   define _X(prop, default, keys, label, desc) \
        "  "CCLI_Y_BOL_SET(default, keys) \
        CCLI_SWALLOW_LABEL("  "label) \
        CCLI_SWALLOW_DEFAULT("  ["CCLI_Y_BOL_CHK(default)"]") \
        CCLI_SWALLOW_DESC("  "desc) "\n"

    "[Flags]\n" CCLI_BOL_ARGS(_X, _Y) "\n"
    #   undef _X
    #endif

    #ifdef CCLI_POS_ARGS
    #   define _X(type, prop, name, label, desc) \
        "  "name \
        CCLI_SWALLOW_TYPE("  "#type) \
        CCLI_SWALLOW_DESC("  "label) \
        CCLI_SWALLOW_DESC("  "desc) "\n"
    #   define _POS_Y(cap, type, prop, name, label, desc) \
        "  "name \
        CCLI_SWALLOW_TYPE("  "#type"["#cap"]") \
        CCLI_SWALLOW_DESC("  "label) \
        CCLI_SWALLOW_DESC("  "desc) "\n"

    "[Positions]\n" CCLI_POS_ARGS(_X, _POS_Y) "\n"
    #   undef _X
    #   undef _POS_Y
    #endif
    );

    #else // CCLI_HELP_ALIGN

    #ifdef CCLI_OPT_ARGS
    unsigned int opt_options_max = 0;
    CCLI_SWALLOW_TYPE(unsigned int opt_type_max = 0;)
    CCLI_SWALLOW_LABEL(unsigned int opt_label_max = 0;)
    CCLI_SWALLOW_DEFAULT(unsigned int opt_default_max = 0;)
    {
        unsigned int ol CCLI_SWALLOW_TYPE(, tl) CCLI_SWALLOW_LABEL(, ll) CCLI_SWALLOW_DEFAULT(, dl);

    #   define _X(type, prop, default, keys, label, ...) \
            ol = sizeof(_C(,CCLI_Y_OPT, CCLI_VA_COUNT(keys))(keys)) - 1; if (ol > opt_options_max) opt_options_max = ol; \
            CCLI_SWALLOW_TYPE(tl = sizeof(#type) - 1; if (tl > opt_type_max) opt_type_max = tl;) \
            CCLI_SWALLOW_LABEL(ll = sizeof(label) - 1; if (ll > opt_label_max) opt_label_max = ll;) \
            CCLI_SWALLOW_DEFAULT(dl = sizeof(#default) - 1; if (dl > opt_default_max) opt_default_max = dl;)

        CCLI_OPT_ARGS(_X, _Y);
    }
    #   undef _X
    #   define _X(type, prop, default, keys, label, desc) \
        CCLI_SB_AP_LIT_PAD("  "_C(,CCLI_Y_OPT, CCLI_VA_COUNT(keys))(keys), opt_options_max + 2) \
        CCLI_SWALLOW_TYPE(CCLI_SB_AP_LIT_PAD("  "#type, opt_type_max + 2);) \
        CCLI_SWALLOW_LABEL(CCLI_SB_AP_LIT_PAD("  "label, opt_label_max + 2);) \
        CCLI_SWALLOW_DEFAULT(CCLI_SB_AP_LIT_PAD("  ["#default"]", opt_default_max + 4);) \
        CCLI_SWALLOW_DESC(CCLI_SB_AP_LIT("  "desc);) \
        CCLI_SB_AP_LIT("\n");

    CCLI_SB_AP_LIT("[Options]\n");
    CCLI_OPT_ARGS(_X, _Y);
    CCLI_SB_AP_LIT("\n");
    #   undef _X

    #endif

    #ifdef CCLI_BOL_ARGS
    unsigned int bol_options_max = 0;
    CCLI_SWALLOW_LABEL(unsigned int bol_label_max = 0;)
    {
        unsigned int ol CCLI_SWALLOW_LABEL(, ll);

    #   define _X(prop, default, keys, label, ...) \
            ol = sizeof(CCLI_Y_BOL_SET(default, keys)) - 1; if (ol > bol_options_max) bol_options_max = ol; \
            CCLI_SWALLOW_LABEL(ll = sizeof(label) - 1; if (ll > bol_label_max) bol_label_max = ll;)
        CCLI_BOL_ARGS(_X, _Y);
    }
    #   undef _X
    #   define _X(prop, default, keys, label, desc) \
        CCLI_SB_AP_LIT_PAD("  "CCLI_Y_BOL_SET(default, keys), bol_options_max + 2) \
        CCLI_SWALLOW_LABEL(CCLI_SB_AP_LIT_PAD("  "label, bol_label_max + 2);) \
        CCLI_SWALLOW_DEFAULT(CCLI_SB_AP_LIT("  ["CCLI_Y_BOL_CHK(default)"]");) \
        CCLI_SWALLOW_DESC(CCLI_SB_AP_LIT("  "desc);) \
        CCLI_SB_AP_LIT("\n");

    CCLI_SB_AP_LIT("[Flags]\n");
    CCLI_BOL_ARGS(_X, _Y);
    CCLI_SB_AP_LIT("\n");
    #   undef _X

    #endif

    #ifdef CCLI_POS_ARGS
    unsigned int pos_name_max = 0;
    CCLI_SWALLOW_TYPE(unsigned int pos_type_max = 0;)
    CCLI_SWALLOW_LABEL(unsigned int pos_label_max = 0;)
    {
        unsigned int nl CCLI_SWALLOW_TYPE(, tl) CCLI_SWALLOW_LABEL(, ll);

    #   define _X(type, prop, name, label, ...) \
            nl = sizeof(name) - 1; if (nl > pos_name_max) pos_name_max = nl; \
            CCLI_SWALLOW_TYPE(tl = sizeof(#type) - 1; if (tl > pos_type_max) pos_type_max = tl;) \
            CCLI_SWALLOW_LABEL(ll = sizeof(label) - 1; if (ll > pos_label_max) pos_label_max = ll;)
    #   define _POS_Y(cap, type, prop, name, label, ...) \
            nl = sizeof(name) - 1; if (nl > pos_name_max) pos_name_max = nl; \
            CCLI_SWALLOW_TYPE(tl = sizeof(#type #cap) - 1; if (tl > pos_type_max) pos_type_max = tl;) \
            CCLI_SWALLOW_LABEL(ll = sizeof(label) - 1; if (ll > pos_label_max) pos_label_max = ll;)

        CCLI_POS_ARGS(_X, _POS_Y);
    }
    #   undef _X
    #   undef _POS_Y
    #   define _X(type, prop, name, label, desc) \
        CCLI_SB_AP_LIT_PAD("  "name, pos_name_max + 2) \
        CCLI_SWALLOW_TYPE(CCLI_SB_AP_LIT_PAD("  "#type, pos_type_max + 4);) \
        CCLI_SWALLOW_LABEL(CCLI_SB_AP_LIT_PAD("  "label, pos_label_max + 2);) \
        CCLI_SWALLOW_DESC(CCLI_SB_AP_LIT("  "desc);) \
        CCLI_SB_AP_LIT("\n");
    #   define _POS_Y(cap, type, prop, name, label, desc) \
        CCLI_SB_AP_LIT_PAD("  "name, pos_name_max + 2) \
        CCLI_SWALLOW_TYPE(CCLI_SB_AP_LIT_PAD("  "#type"["#cap"]", pos_type_max + 4);) \
        CCLI_SWALLOW_LABEL(CCLI_SB_AP_LIT_PAD("  "label, pos_label_max + 2);) \
        CCLI_SWALLOW_DESC(CCLI_SB_AP_LIT("  "desc);) \
        CCLI_SB_AP_LIT("\n");

    CCLI_SB_AP_LIT("[Positions]\n");
    CCLI_POS_ARGS(_X, _POS_Y);
    CCLI_SB_AP_LIT("\n");
    #   undef _X
    #   undef _POS_Y

    #endif
    #endif // CCLI_HELP_ALIGN

    if ((CCLI_EXAMPLE)[0]) CCLI_SB_AP_LIT("Examples: \n" CCLI_EXAMPLE);

    while (sb.len && sb.data[sb.len-1] == '\n') --sb.len;

    return sb.len;

    #undef CCLI_SB_AP_LIT
    #undef CCLI_SB_AP_LIT_PAD
    #undef _Y
    #undef CCLI_Y_BOL_1u
    #undef CCLI_Y_BOL_0u
    #undef CCLI_Y_OPT1
    #undef CCLI_Y_OPT2
    #undef CCLI_Y_OPT3
    #undef CCLI_Y_OPT4
    #undef CCLI_Y_OPT5
    #undef CCLI_Y_BOL10u
    #undef CCLI_Y_BOL20u
    #undef CCLI_Y_BOL30u
    #undef CCLI_Y_BOL40u
    #undef CCLI_Y_BOL50u
    #undef CCLI_Y_BOL11u
    #undef CCLI_Y_BOL21u
    #undef CCLI_Y_BOL31u
    #undef CCLI_Y_BOL41u
    #undef CCLI_Y_BOL51u
    #undef CCLI_Y_OPT_SET
    #undef CCLI_Y_BOL_SET
    #undef CCLI_Y_BOL_CHK
    #undef CCLI_SWALLOW_TYPE
    #undef CCLI_SWALLOW_DEFAULT
    #undef CCLI_SWALLOW_LABEL
    #undef CCLI_SWALLOW_DESC
}

unsigned int _A_(,_struct_str)(_A_(,_args_t)* cli, char* buf, unsigned int cap) {
    (void) cli;

    _A_(_,_sb) sb = { buf, 0, cap };
    #define CCLI_SB_AP_LIT(s) _A_(_,_sb_ap)(&sb,s,sizeof(s)-1)

    CCLI_SB_AP_LIT(CCLI_NAME " {\n");

    #ifdef CCLI_POS_ARGS
    #   define _X(...)
    #   define _Y(cap, type, prop, ...) \
        if (cli->prop.count == 0) { \
            CCLI_SB_AP_LIT("  "#prop": []\n"); \
        } else { \
            CCLI_SB_AP_LIT("  "#prop": [\n"); \
            for (unsigned int i = 0; i < cli->prop.count; ++i) { \
                CCLI_SB_AP_LIT("    "); \
                CCLI_SB(type)(&sb, CCLI_EXPEND(type, cli->prop.items[i])); \
                CCLI_SB_AP_LIT("\n"); \
            } \
            CCLI_SB_AP_LIT("  ]\n"); \
        }

    CCLI_POS_ARGS(_X, _Y)
    #   undef _X
    #   undef _Y
    #   define _X(type, prop, ...) \
        CCLI_SB_AP_LIT("  "#prop": "); \
        CCLI_SB(type)(&sb, CCLI_EXPEND(type, cli->prop)); \
        CCLI_SB_AP_LIT("\n");
    #   define _Y(...)

    CCLI_POS_ARGS(_X, _Y)
    #   undef _X
    #   undef _Y
    #endif

    #ifdef CCLI_OPT_ARGS
    #   define _X(type, prop, ...) \
        CCLI_SB_AP_LIT("  "#prop": "); \
        CCLI_SB(type)(&sb, CCLI_EXPEND(type, cli->prop)); \
        CCLI_SB_AP_LIT("\n");

    CCLI_OPT_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    #ifdef CCLI_BOL_ARGS
    #   define _X(prop, ...) \
        CCLI_SB_AP_LIT("  "#prop": "); \
        if (cli->prop) CCLI_SB_AP_LIT("true"); else CCLI_SB_AP_LIT("false"); \
        CCLI_SB_AP_LIT("\n");

    CCLI_BOL_ARGS(_X, CCLI_Y)
    #   undef _X
    #endif

    CCLI_SB_AP_LIT("}");

    return sb.len;
}

#undef CCLI
#undef CCLI_POS_ARGS
#undef CCLI_OPT_ARGS
#undef CCLI_BOL_ARGS

#undef CCLI_LEGACY
#undef CCLI_POSITIONAL

#undef CCLI_NAME
#undef CCLI_HEADER
#undef CCLI_USAGE
#undef CCLI_DESCRIPTION
#undef CCLI_EXAMPLE

#undef CCLI_HELP_TYPE
#undef CCLI_HELP_DEFAULT
#undef CCLI_HELP_LABEL
#undef CCLI_HELP_DESC
#undef CCLI_HELP_ALIGN

#undef CCLI_X
#undef CCLI_Y
#undef __C
#undef _C
#undef __S
#undef _S
#undef __R
#undef _R
#undef _A_

#undef CCLI_O_INVALID
#undef CCLI_O_KEY
#undef CCLI_O_ON
#undef CCLI_O_OFF
#undef CCLI_O_ON_OR_KEY
#undef CCLI_O_SKIP

#undef CCLI_D_TRUE
#undef CCLI_D_FALSE
#undef CCLI_D_YES
#undef CCLI_D_NO
#undef CCLI_D_

#undef CCLI_BOOL

#undef CCLI_T_UINT
#undef CCLI_T_INT
#undef CCLI_T_STRING
#undef CCLI_T_CHAR
#undef CCLI_T_

#undef CCLI_TYPE

#undef CCLI_S_UINT
#undef CCLI_S_INT
#undef CCLI_S_STRING
#undef CCLI_S_CHAR
#undef CCLI_S_

#undef CCLI_SB

#undef CCLI_E_UINT
#undef CCLI_E_INT
#undef CCLI_E_STRING
#undef CCLI_E_CHAR
#undef CCLI_E_

#undef CCLI_EXPEND

#undef CCLI_A_UINT
#undef CCLI_A_INT
#undef CCLI_A_STRING
#undef CCLI_A_CHAR
#undef CCLI_A_

#undef CCLI_ASSIGN

#undef CCLI_SUPPRESS

#undef CCLI_ARG_N
#undef CCLI_RSEQ_N
#undef CCLI_VA_COUNT
#undef _CCLI_VA_COUNT

#undef CCLI_READ
