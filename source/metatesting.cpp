#include "types.h"
#include <glm/glm.hpp>

#include <vector>
#include <string.h>

#define DEF_ACCESS_TYPES \
    DEF_ACCESS_TYPE(readonly, STRING(readonly)) \
    DEF_ACCESS_TYPE(readwrite, STRING(readwrite)) \
    DEF_ACCESS_TYPE(debugonly, STRING(debugonly)) \
    DEF_ACCESS_TYPE(nometa, STRING(nometa))

enum struct ACCESS_TYPE {
    #define DEF_ACCESS_TYPE(a, b) a,
    DEF_ACCESS_TYPES
    #undef DEF_ACCESS_TYPE
    ENUM_COUNT
};

const char* ACCESS_TYPE_strings[(usize)ACCESS_TYPE::ENUM_COUNT] = {
    #define DEF_ACCESS_TYPE(a, b) b,
    DEF_ACCESS_TYPES
    #undef DEF_ACCESS_TYPE
};

#undef DEF_ACCESS_TYPES


struct FieldMetaData {
    const char* name;
    i32         offset;
    FIELD_TYPE  type;
};

struct ArrayMetaData {
    const char*     name;
    void*           array_ptr;
    usize           type_size;
    FieldMetaData*  meta_data;
};

struct Wee {
    i32 x;
    Vec3 v;

    static const usize   COUNT;
    static usize         used;
    static Wee           set[1024];
    static FieldMetaData meta_data[3];
};

const usize Wee::COUNT = 1024;
usize Wee::used = 0;
Wee Wee::set[1024] = {};

FieldMetaData Wee::meta_data[3] = {
    {"x", offsetof(Wee, x), FIELD_TYPE::f_i32},
    {"v", offsetof(Wee, v), FIELD_TYPE::f_vec3},
    {0, 0, (FIELD_TYPE)0}
};

ArrayMetaData struct_meta_data[] = {
    {"Wee", &Wee::set, sizeof(Wee), &Wee::meta_data[0]},
    {0, 0, 0, 0}
};

namespace runtime_access {
// bool is_symbol(char c)
// {
//     return (
//         c == '.' || 
//         c == '=' ||
//         c == '+' ||
//         c == '-' ||
//         c == '*' ||
//         c == '/' ||
//         c == '&' ||
//         c == '(' ||
//         c == ')' ||
//         c == '[' ||
//         c == ']' ||
//         c == '{' ||
//         c == '}' ||
//         c == '<' ||
//         c == '>' ||
//         c == '%' ||
//         c == ':' ||
//         c == ';' ||
//         c == '?' ||
//         c == '^' ||
//         c == ',' ||
//         c == '!'
//     );
// }

bool is_symbol(char c)
{
    return (
        c == '.' ||
        c == '=' ||
        c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/' ||
        c == '\"'||
        c == '<' ||
        c == '>' ||
        c == '^' ||
        c == ',' ||
        c == ':' ||
        c == ';' ||
        c == '!'
    );
}

bool is_delimiter(char c)
{
    return (
        c == '(' ||
        c == ')' ||
        c == '[' ||
        c == ']' ||
        c == '{' ||
        c == '}' ||
        c == ',' ||
        c == ';'
    );
}

/*
    DEF_ASSOC(ASSIGN,   STRING(=)) \
    DEF_ASSOC(DOT,      ".") \
    DEF_ASSOC(ADD,      STRING(+)) \
    DEF_ASSOC(SUBTRACT, STRING(-)) \
    DEF_ASSOC(MULTIPLY, STRING(*)) \
    DEF_ASSOC(DIVIDE,   STRING(/)) \
    DEF_ASSOC(POWER,    STRING(pow)) \
    DEF_ASSOC(NOT,      STRING(not)) \
    DEF_ASSOC(COMP_L,   STRING(<)) \
    DEF_ASSOC(COMP_LE,  STRING(<=)) \
    DEF_ASSOC(COMP_G,   STRING(>)) \
    DEF_ASSOC(COMP_GE,  STRING(>=)) \
    DEF_ASSOC(COMP_EQ,  STRING(sameas)) \
    DEF_ASSOC(MOD,      STRING(mod))
*/

bool is_operator(char c)
{
    return (
        c == '=' ||
        c == '.' ||
        c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/' ||
        c == '<' ||
        c == '>'
    );
}

enum struct TOKEN_KIND {
    KEYWORD,
    OPERATOR,
    IDENTIFIER,
    CONSTANT,
    DELIMITER,
    ENUM_COUNT
};

// #define DEF_TOKEN_KEYWORDS \
//     DEF_TOKEN_KEYWORD(TRUE,       STRING(true)) \
//     DEF_TOKEN_KEYWORD(FALSE,      STRING(false)) \
//     DEF_TOKEN_KEYWORD(IF,         STRING(if)) \
//     DEF_TOKEN_KEYWORD(ELSE,       STRING(else)) \
//     DEF_TOKEN_KEYWORD(WHILE,      STRING(while)) \
//     DEF_TOKEN_KEYWORD(FOR,        STRING(for)) \
//     DEF_TOKEN_KEYWORD(DO,         STRING(do)) \
//     DEF_TOKEN_KEYWORD(META,       STRING(meta)) \
//     DEF_TOKEN_KEYWORD(NOMETA,     STRING(nometa)) \
//     DEF_TOKEN_KEYWORD(READONLY,   STRING(readonly)) \
//     DEF_TOKEN_KEYWORD(READWRITE,  STRING(readwrite)) \
//     DEF_TOKEN_KEYWORD(DEBUGONLY,  STRING(debugonly))
#define DEF_ASSOC_LIST \
    DEF_ASSOC(IF,                 STRING(if)) \
    DEF_ASSOC(ELSE,               STRING(else)) \
    DEF_ASSOC(WHILE,              STRING(while)) \
    DEF_ASSOC(FOR,                STRING(for)) \
    DEF_ASSOC(DO,                 STRING(do)) \
    DEF_ASSOC(UNTIL,              STRING(until)) \
    DEF_ASSOC(REPEAT,             STRING(repeat)) \
    DEF_ASSOC(JUMP,               STRING(jump)) \
    DEF_ASSOC(SELECT,             STRING(select)) \
    DEF_ASSOC(SET,                STRING(set)) \
    DEF_ASSOC(CONTEXTUAL_COMMAND, STRING(contextual_command)) \

    enum struct TOKEN_KEYWORD {
        #define DEF_ASSOC(a, b) a,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
        ENUM_COUNT
    };

    const char* TOKEN_KEYWORD_strings[(usize)TOKEN_KEYWORD::ENUM_COUNT] = {
        #define DEF_ASSOC(a, b) b,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
    };

#undef DEF_ASSOC_LIST

#define DEF_ASSOC_LIST \
    DEF_ASSOC(ASSIGN,   STRING(=)) \
    DEF_ASSOC(DOT,      ".") \
    DEF_ASSOC(ADD,      STRING(+)) \
    DEF_ASSOC(SUBTRACT, STRING(-)) \
    DEF_ASSOC(MULTIPLY, STRING(*)) \
    DEF_ASSOC(DIVIDE,   STRING(/)) \
    DEF_ASSOC(COMP_L,   STRING(<)) \
    DEF_ASSOC(COMP_LE,  STRING(<=)) \
    DEF_ASSOC(COMP_G,   STRING(>)) \
    DEF_ASSOC(COMP_GE,  STRING(>=)) \
    DEF_ASSOC(POWER,    STRING(pow)) \
    DEF_ASSOC(NOT,      STRING(not)) \
    DEF_ASSOC(NOT_SYMBOL, STRING(!)) \
    DEF_ASSOC(COMP_EQ,  STRING(sameas)) \
    DEF_ASSOC(MOD,      STRING(mod)) \
    DEF_ASSOC(COMP_EQ_SYMBOL, STRING(==)) \
    DEF_ASSOC(CREATE, STRING(CREATE)) \
    DEF_ASSOC(COMP_EQ_ALT, STRING(is))

    enum struct TOKEN_OPERATOR {
        #define DEF_ASSOC(a, b) a,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
        ENUM_COUNT
    };

    const char* TOKEN_OPERATOR_strings[(usize)TOKEN_OPERATOR::ENUM_COUNT] = {
        #define DEF_ASSOC(a, b) b,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
    };

#undef DEF_ASSOC_LIST

#define DEF_ASSOC_LIST \
    DEF_ASSOC(OPEN_ARRAY,       "[") \
    DEF_ASSOC(CLOSE_ARRAY,      "]") \
    DEF_ASSOC(OPEN_EXPRESSION,  "(") \
    DEF_ASSOC(CLOSE_EXPRESSION, ")") \
    DEF_ASSOC(OPEN_SCOPE,       "{") \
    DEF_ASSOC(CLOSE_SCOPE,      "}") \
    DEF_ASSOC(COMMA,            ",") \
    DEF_ASSOC(STATEMENT_END,    STRING(end)) \
    DEF_ASSOC(STATEMENT_END_SYMBOL, STRING(;))

    enum struct TOKEN_DELIMITER {
        #define DEF_ASSOC(a, b) a,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
        ENUM_COUNT
    };

    const char* TOKEN_DELIMITER_strings[(usize)TOKEN_DELIMITER::ENUM_COUNT] = {
        #define DEF_ASSOC(a, b) b,
        DEF_ASSOC_LIST
        #undef DEF_ASSOC
    };

#undef DEF_ASSOC_LIST


enum struct TOKEN_CONSTANT {
    NUMBER,
    STRING,
    ENUM_COUNT
};

struct Token {
    TOKEN_KIND type;

    union {
        struct {
            TOKEN_KEYWORD type;
        } keyword;
        struct {
            TOKEN_OPERATOR type;
        } operation;
        struct {
            TOKEN_DELIMITER type;
        } delimiter;
        struct {
            char* string;
        } identifier;
        struct {
            TOKEN_CONSTANT type;
            char* string;
        } constant;
    };
};

char* allocate_substring(char* string, usize count)
{
    char* substring = (char*)xcalloc(1, count + 1);
    strncpy(substring, string, count);
    return substring;
}

void Token_print(Token* token) 
{
    switch (token->type) {
    case TOKEN_KIND::KEYWORD:
        printf("KEYWORD: %s\n", TOKEN_KEYWORD_strings[(usize)token->keyword.type]);
        break;
    case TOKEN_KIND::OPERATOR:
        printf("OPERATOR: %s\n", TOKEN_OPERATOR_strings[(usize)token->operation.type]);
        break;
    case TOKEN_KIND::IDENTIFIER:
        printf("IDENTIFIER: %s\n", token->identifier.string);
        break;
    case TOKEN_KIND::CONSTANT:
        printf("CONSTANT: %s\n", token->constant.string);
        break;
    case TOKEN_KIND::DELIMITER:
        printf("DELIMITER: %s\n", TOKEN_DELIMITER_strings[(usize)token->delimiter.type]);
        break;
    }
}

void print_tokens(Token* tokens, const usize N)
{
    foreach(i, N) {
        Token_print(&tokens[i]);
    }
}

std::vector<Token> tokenize_script(char* code)
{
    if (code == nullptr) {
        return {};
    }

    enum struct READ_MODE {
        SEEK,
        ALPHANUMERIC_WORD,
        OPERATOR_WORD,
        DELIMITER_WORD,
        NUMBER_CONSTANT,
        STRING_CONSTANT,
        ENUM_COUNT
    };

    READ_MODE read_mode = READ_MODE::SEEK;
    TOKEN_KIND token_kind = TOKEN_KIND::KEYWORD;
    usize read_marker = 0;

    std::vector<Token> tokens;

    puts("LEXING");
    for (usize i = 0; code[i];) {
        std::cout << "i=" << i << std::endl;
        switch (read_mode) {
        case READ_MODE::SEEK: {
            std::cout << "SEEK" << std::endl;
            read_marker = i;

            if (isalpha(code[i]) || code[i] == '_') {
                read_mode = READ_MODE::ALPHANUMERIC_WORD;
            } else if (is_operator(code[i])) {
                read_mode = READ_MODE::OPERATOR_WORD;
            } else if (is_delimiter(code[i])) {
                read_mode = READ_MODE::DELIMITER_WORD;
            } else if (code[i] == '"') {
                read_mode = READ_MODE::STRING_CONSTANT;
                read_marker += 1;
            } else if (isdigit(code[i])) {
                read_mode = READ_MODE::NUMBER_CONSTANT;
            }

            i += 1;
            break;
        }

        case READ_MODE::ALPHANUMERIC_WORD: {
            std::cout << "ALPHANUMERIC_WORD" << std::endl;
            if (!isalpha(code[i]) && !isdigit(code[i]) && code[i] != '_') {
                read_mode = READ_MODE::SEEK;

                bool is_matched = false;
                if (!is_matched) {
                    foreach(j, (usize)TOKEN_KEYWORD::ENUM_COUNT) {
                        char saved = code[i];
                        code[i] = '\0';                 
                        i32 cmp = strcmp(code + read_marker, TOKEN_KEYWORD_strings[j]);
                        code[i] = saved;
                        if (cmp == 0) {
                            is_matched = true;

                            Token found_token = {};
                            found_token.type = TOKEN_KIND::KEYWORD;
                            found_token.keyword.type = (TOKEN_KEYWORD)j;

                            tokens.push_back(found_token);

                            break;
                        }
                    }
                }

                if (!is_matched) {
                    foreach(j, (usize)TOKEN_OPERATOR::ENUM_COUNT) {
                        if (0 == strncmp(code + read_marker, 
                                         TOKEN_OPERATOR_strings[j], 
                                         strlen(TOKEN_OPERATOR_strings[j]))) {
                            is_matched = true;

                            Token found_token = {};
                            found_token.type = TOKEN_KIND::OPERATOR;
                            found_token.operation.type = (TOKEN_OPERATOR)j;

                            tokens.push_back(found_token);
                            break;
                        }
                    }
                }

                if (!is_matched) {
                    char saved = code[i];
                    code[i] = '\0';  
                    if (0 == strcmp(code + read_marker,
                                     TOKEN_DELIMITER_strings[(usize)TOKEN_DELIMITER::STATEMENT_END])) {
                        is_matched = true;

                        Token found_token = {};
                        found_token.type = TOKEN_KIND::DELIMITER;
                        found_token.delimiter.type = TOKEN_DELIMITER::STATEMENT_END;

                        tokens.push_back(found_token);
                    }
                    code[i] = saved;
                }

                if (!is_matched) { // identifier
                    Token found_token = {};
                    found_token.type = TOKEN_KIND::IDENTIFIER;
                    found_token.identifier.string = allocate_substring(code + read_marker, i - read_marker);

                    tokens.push_back(found_token);

                    is_matched = true;
                }

                read_mode = READ_MODE::SEEK;
            } else {
                i += 1;
                std::cout << "MOVING MARKER" << std::endl;
            }
            std::cout << "EXITING CASE" << std::endl;
            break;
        } 
        case READ_MODE::OPERATOR_WORD: {
            std::cout << "OPERATOR_WORD" << std::endl;
            if (!is_operator(code[i])) {
                read_mode = READ_MODE::SEEK;

                bool is_matched = false;

                if (!is_matched) {
                    foreach(j, (usize)TOKEN_OPERATOR::ENUM_COUNT) {
                        if (0 == strncmp(code + read_marker, TOKEN_OPERATOR_strings[j], strlen(TOKEN_OPERATOR_strings[j]))) {
                            is_matched = true;

                            Token found_token = {};
                            found_token.type = TOKEN_KIND::OPERATOR;
                            found_token.operation.type = (TOKEN_OPERATOR)j;

                            tokens.push_back(found_token);
                            break;
                        }
                    }
                }

            } else {
                i += 1;
            } 
            break;
        }
        case READ_MODE::DELIMITER_WORD: {
            std::cout << "DELIMITER_WORD" << std::endl;
            if (!is_delimiter(code[i]) || i > read_marker) {
                read_mode = READ_MODE::SEEK;

                bool is_matched = false;

                if (!is_matched) {
                    foreach(j, (usize)TOKEN_DELIMITER::ENUM_COUNT) {
                        if (0 == strncmp(code + read_marker, TOKEN_DELIMITER_strings[j], strlen(TOKEN_DELIMITER_strings[j]))) {
                            is_matched = true;

                            Token found_token = {};
                            found_token.type = TOKEN_KIND::DELIMITER;
                            found_token.delimiter.type = (TOKEN_DELIMITER)j;

                            tokens.push_back(found_token);
                            break;
                        }
                    }
                }
            } else {
                i += 1;
            }
            break;
        }
        case READ_MODE::NUMBER_CONSTANT:
            std::cout << "NUMBER_CONSTANT" << std::endl;
            if (!isdigit(code[i]) && code[i] != '.') {
                read_mode = READ_MODE::SEEK;

                char* constant_string = allocate_substring(code + read_marker, i - read_marker);
                Token found_token = {};
                found_token.type = TOKEN_KIND::CONSTANT;
                found_token.constant.type = TOKEN_CONSTANT::NUMBER;
                found_token.constant.string = constant_string;
                tokens.push_back(found_token);
            } else {
                i += 1;
            }
            break;
        case READ_MODE::STRING_CONSTANT: {
            std::cout << "STRING_CONSTANT" << std::endl;
            if (code[i] == '"' && code[i - 1] != '\\') {
                read_mode = READ_MODE::SEEK;

                char* constant_string = allocate_substring(code + read_marker, i - read_marker);
                Token found_token = {};
                found_token.type = TOKEN_KIND::CONSTANT;
                found_token.constant.type = TOKEN_CONSTANT::STRING;
                found_token.constant.string = constant_string;
                tokens.push_back(found_token);
            }

            i += 1;

            break;
        }
        default: {
            std::cout << "DEFAULT" << std::endl;
            break;
        }
        }

        std::cout << "END OF ITERATION" << std::endl;
    }

    return tokens;
}

void parse_tokens(std::vector<Token>* tokens)
{
    
}


const Wee* init()
{
    if (Wee::used == Wee::COUNT) {
        return nullptr;
    }

    Wee* self = &Wee::set[Wee::used];

    self->x   = 1;
    self->v.x = 0.0f;
    self->v.y = 0.0f;
    self->v.z = 0.0f;

    Wee::used += 1;

    return self;
}

}

void metatesting(void)
{
    using namespace runtime_access;
    puts("KEYWORDS");

    foreach(i , (usize)TOKEN_KEYWORD::ENUM_COUNT) {
        puts(TOKEN_KEYWORD_strings[i]);
    }

    puts("OPERATORS");
    foreach(i , (usize)TOKEN_OPERATOR::ENUM_COUNT) {
        puts(TOKEN_OPERATOR_strings[i]);
    }

    puts("DELIMITERS");
    foreach(i , (usize)TOKEN_DELIMITER::ENUM_COUNT) {
        puts(TOKEN_DELIMITER_strings[i]);
    }

    //char* code = "X[i].field[5 + (y / PI)]=5.0 ";
    char* code = "if selected.field sameas X[i].field do swap(self.field, X[i].field pow 2) else do self.field = 5.0 end ";
    char* c = (char*)xmalloc(strlen(code) + 1);
    memcpy(c, code, strlen(code) + 1);
    std::cout << "[" << c << "]" << std::endl;
    std::vector<Token> tokens = tokenize_script(c);
    std::cout << std::endl << "TOKENS:" << std::endl << std::endl;
    print_tokens(tokens.data(), tokens.size());
}
