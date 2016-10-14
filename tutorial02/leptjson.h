#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum {
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT,
    LEPT_COUNT
}lept_type;

typedef struct {
	double n;
    lept_type type;
}lept_value;

typedef struct {
    const char* json;
}lept_context;

typedef enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    LEPT_PARSE_NUMBER_TOO_BIG,
    LEPT_PARSE_COUNT
}lept_parse_e;

typedef enum {
    LEPT_ENUM_TYPE,
    LEPT_ENUM_PARSE,
    LEPT_ENUM_COUNT
}enum_type;

int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

double lept_get_number(const lept_value* v);

const char* lept_get_error_name(int error, enum_type e_type);

#define ISDIGIT(ch) ((ch>='0') && (ch<='9'))
#define ISDIGIT1TO9(ch) ((ch>='1') && (ch<='9'))

#endif /* LEPTJSON_H__ */
