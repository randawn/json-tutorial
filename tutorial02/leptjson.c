#include "leptjson.h"
#include <stdio.h>
#include <errno.h>   /* errno, ERANGE */
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <string.h>  /* memcmp(), strlen(), strchr() */
#include <math.h>  /* HUGE_VAL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

const char* lept_get_error_name(int error, enum_type e_type) {
    static const char* ret_e_name[] = {
        "LEPT_PARSE_OK",
        "LEPT_PARSE_EXPECT_VALUE",
        "LEPT_PARSE_INVALID_VALUE",
        "LEPT_PARSE_ROOT_NOT_SINGULAR",
        "LEPT_PARSE_NUMBER_TOO_BIG"
    };
    static const char* type_e_name[] = {
        "LEPT_NULL",
        "LEPT_FALSE",
        "LEPT_TRUE",
        "LEPT_NUMBER",
        "LEPT_STRING",
        "LEPT_ARRAY",
        "LEPT_OBJECT"
    };
    const char** e_name = e_type==LEPT_ENUM_TYPE? ret_e_name : type_e_name;
    int count = e_type==LEPT_ENUM_TYPE? LEPT_COUNT : LEPT_PARSE_COUNT;
    assert(error>=0 && error< count);
    return e_name[error];
}

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

#if 0
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
#endif

static int lept_parse_literal(lept_context* c, lept_value* v, const char* ref_liter, lept_type ref_type) {
    /*EXPECT*/;
    if (memcmp(c->json, ref_liter, strlen(ref_liter)))
        return LEPT_PARSE_INVALID_VALUE;
    c->json += strlen(ref_liter);
    v->type = ref_type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    /* JSON */
    /* number = [ "-" ] int [ frac ] [ exp ]
     * int = "0" / digit1-9 *digit
     * frac = "." 1*digit
     * exp = ("e" / "E") ["-" / "+"] 1*digit
     */
    /* C90 */
    /* a valid floating point number for strtod using the "C" locale is formed by
     * an optional sign character (+ or -),
     * followed by a sequence of digits, optionally containing a decimail-point charactor(.),
     * optionally followed by an exponent part
     * (an e or E character followed by an optional sign and a sequence of digits).
     */
    if ((*c->json == '+') ||
        (*c->json == '.') ||
        !(ISDIGIT(strchr(c->json, '.')==NULL?
                '0' :
                *(strchr(c->json, '.')+1))) ||
        (*c->json == 'I') ||
        (*c->json == 'i') ||
        (*c->json == 'N') ||
        (*c->json == 'n')) {
        return LEPT_PARSE_INVALID_VALUE;
    }
    else if (*c->json == '0' && !(*(c->json+1)=='.' || *(c->json+1)=='\0')) {
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    errno = 0;
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    else if(errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))  /* FIXME abs(v->n) not work */
        return LEPT_PARSE_NUMBER_TOO_BIG;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
#if 0
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
#endif
        case 't':  return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
