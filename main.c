#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ... existing code ...

typedef enum
{
    NODE_NUMBER,
    NODE_OPERATION,
    NODE_PRINT,
    NODE_INPUT,
    NODE_VARIABLE,
    NODE_STRING,
    NODE_ASSIGN,
    NODE_BLOCK,
    NODE_IF,
    NODE_WHILE,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_ARRAY_LITERAL,
    NODE_ARRAY_ACCESS,
    NODE_ARRAY_ASSIGN,
    NODE_MEMBER_ACCESS,
    NODE_METHOD_CALL,
    NODE_BOOL,
    NODE_DICT_LITERAL,
    NODE_VAR_DECL,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_CLASS_DEF,
    NODE_NEW,
    NODE_THIS,
    NODE_MEMBER_ASSIGN,
    NODE_NULL,
    NODE_TRY,
    NODE_THROW,
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT,
    NODE_FOR,
    NODE_FOREACH
} NodeType;

// Forward declaration
struct Array;
struct Dict;

typedef struct Node
{
    NodeType type;
    // Pola wspólne dla wszystkich typów
    double value;      // dla liczb i obliczonych wartości
    char *var_name;    // dla zmiennych
    char *string_value; // dla wartości tekstowych
    struct Array *array_value; // dla tablic
    struct Dict *dict_value;   // dla słowników
    struct Node *expr; // dla wyrażeń (PRINT, RETURN itp.)

    // Pola specyficzne dla operatorów
    struct
    {
        int op_type;    // TokenType
        struct Node *a; // lewe poddrzewo
        struct Node *b; // prawe poddrzewo
    } op;

    // Dla IF / WHILE
    struct {
        struct Node *cond;
        struct Node *body;
        struct Node *else_body; // dla IF
    } flow;

    // Dla BLOCK / ARRAY LITERAL
    struct {
        struct Node **stmts; // używane też jako elementy tablicy w literale
        int count;
    } block;
    
    // Dla FUNC DEF
    struct {
        char *name;
        char **args;
        int arg_count;
        struct Node *body;
    } func;
    
    // Dla FUNC CALL
    struct {
        char *name;
        struct Node **args;
        int arg_count;
    } call;
    
    // Dla ARRAY ACCESS / ASSIGN
    struct {
        char *name;
        struct Node *obj; // Dodane dla zagnieżdżonych tablic
        struct Node *index;
        struct Node *value; // dla ASSIGN
    } array_op;

    // Dla MEMBER ACCESS
    struct {
        struct Node *obj;
        char *name;
    } member;

    // Dla METHOD CALL
    struct {
        struct Node *obj;
        char *name;
        struct Node **args;
        int arg_count;
    } method;

    // Dla CLASS DEF
    struct {
        char *name;
        char *parent;
        struct Node **methods;
        int method_count;
    } class_def;

    // Dla NEW
    struct {
        char *class_name;
        struct Node **args;
        int arg_count;
    } new_inst;

    // Dla TRY-CATCH
    struct {
        struct Node *try_body;
        struct Node *catch_body;
        char *catch_var;
    } try_catch;

    // Dla SWITCH
    struct {
        struct Node *expr;
        struct Node **cases;
        int case_count;
        struct Node *default_case;
    } switch_stmt;

    // Dla CASE
    struct {
        struct Node *value;
        struct Node *body;
    } case_stmt;

    // Dla FOR
    struct {
        struct Node *init;
        struct Node *cond;
        struct Node *inc;
        struct Node *body;
    } for_loop;

    // Dla FOREACH
    struct {
        char *var_name;
        struct Node *collection;
        struct Node *body;
    } foreach_loop;
} Node;
typedef enum
{
    TOKEN_IDENT,   // nazwa zmiennej lub polecenia
    TOKEN_NUMBER,  // liczba
    TOKEN_STRING,  // tekst w cudzysłowie
    TOKEN_KEYWORD, // słowo kluczowe (np. print)
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQ,       // ==
    TOKEN_NEQ,      // !=
    TOKEN_LT,       // <
    TOKEN_GT,       // >
    TOKEN_LTE,      // <=
    TOKEN_GTE,      // >=
    TOKEN_AND,      // &&
    TOKEN_OR,       // ||
    TOKEN_LBRACKET, // [
    TOKEN_RBRACKET, // ]
    TOKEN_DOT,      // .
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_COLON,    // :
    TOKEN_INC,      // ++
    TOKEN_DEC,      // --
    TOKEN_PLUS_ASSIGN, // +=
    TOKEN_MINUS_ASSIGN, // -=
    TOKEN_MUL_ASSIGN, // *=
    TOKEN_DIV_ASSIGN, // /=
    TOKEN_MOD,        // %
    TOKEN_MOD_ASSIGN, // %=
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_CLASS,
    TOKEN_CONSTRUCTOR,
    TOKEN_EXTENDS,
    TOKEN_STATIC,
    TOKEN_PRIVATE,
    TOKEN_PUBLIC,
    TOKEN_NEW,
    TOKEN_THIS,
    TOKEN_NULL,
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_THROW,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_EOF       // koniec pliku
} TokenType;

typedef struct
{
    TokenType type;
    char text[64];
} Token;

Token *tokens = NULL;
int token_count = 0;
int token_capacity = 0;
typedef enum {
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_BOOL,
    TYPE_DICT,
    TYPE_INT,
    TYPE_CLASS,
    TYPE_INSTANCE,
    TYPE_NULL
} VarType;

// Forward declarations
struct Class;
struct Instance;

typedef struct {
    int type; // 0=double, 1=string, 2=array, 3=bool, 4=dict, 5=class, 6=instance
    union {
        double doubleValue;
        char *stringValue;
        struct Array *arrayValue;
        struct Dict *dictValue;
        int intValue; // for bool
        struct Class *classValue;
        struct Instance *instanceValue;
    } value;
} ArrayElement; // Reusing for Dict values too

typedef struct Array {
    ArrayElement *elements;
    int count;
    int capacity;
    int ref_count; // Reference counting
} Array;

typedef struct {
    char *key;
    ArrayElement value;
} DictEntry;

typedef struct Dict {
    DictEntry *entries;
    int count;
    int capacity;
    int ref_count; // Reference counting
} Dict;

typedef struct
{
    char name[64]; // <-- tu trzymamy nazwę zmiennej
    VarType type;
    union
    {
        int intValue;
        double doubleValue;
        char stringValue[64];
        Array *arrayValue;
        Dict *dictValue;
        struct Class *classValue;
        struct Instance *instanceValue;
    } value;
} Variable;

typedef struct Env {
    Variable *variables;
    int var_count;
    int var_capacity;
    struct Env *parent;
} Env;

Env *global_env = NULL;
Env *current_env = NULL;
struct Instance *current_instance = NULL;

Env *create_env(Env *parent) {
    Env *env = calloc(1, sizeof(Env));
    env->parent = parent;
    env->var_count = 0;
    env->var_capacity = 8;
    env->variables = malloc(sizeof(Variable) * env->var_capacity);
    return env;
}

Variable *env_get(Env *env, const char *name) {
    if (!env || !name) return NULL;
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i];
        }
    }
    if (env->parent) return env_get(env->parent, name);
    return NULL;
}

Variable *env_define(Env *env, const char *name) {
    if (!env || !name) return NULL;
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i]; // Return existing to update
        }
    }
    if (env->var_count >= env->var_capacity) {
        env->var_capacity *= 2;
        env->variables = realloc(env->variables, sizeof(Variable) * env->var_capacity);
    }
    Variable *var = &env->variables[env->var_count++];
    strncpy(var->name, name, 63);
    // Initialize to safe defaults
    var->type = TYPE_DOUBLE;
    var->value.doubleValue = 0;
    return var;
}

typedef struct {
    char name[64];
    Node *body;
    char **args;
    int arg_count;
} Function;

Function *functions = NULL;
int func_count = 0;
int func_capacity = 0;

Function *get_function(const char *name) {
    for (int i=0; i<func_count; i++) {
        if (strcmp(functions[i].name, name) == 0) return &functions[i];
    }
    return NULL;
}

typedef struct Class {
    char *name;
    char *parent;
    Function *methods;
    int method_count;
    int method_capacity;
} Class;

typedef struct Instance {
    Class *cls;
    Dict *fields;
    int ref_count;
} Instance;

// Memory Management Helpers
void free_array(Array *arr);
void free_dict(Dict *d);

void incref_array(Array *arr) {
    if (arr) arr->ref_count++;
}

void decref_array(Array *arr) {
    if (arr) {
        arr->ref_count--;
        if (arr->ref_count <= 0) free_array(arr);
    }
}

void incref_dict(Dict *d) {
    if (d) d->ref_count++;
}

void decref_dict(Dict *d) {
    if (d) {
        d->ref_count--;
        if (d->ref_count <= 0) free_dict(d);
    }
}

// Array helpers
Array *create_array() {
    Array *arr = malloc(sizeof(Array));
    arr->count = 0;
    arr->capacity = 8;
    arr->ref_count = 1; // Start with 1 ref
    arr->elements = malloc(sizeof(ArrayElement) * arr->capacity);
    return arr;
}

void free_array(Array *arr) {
    if (!arr) return;
    for (int i=0; i<arr->count; i++) {
        if (arr->elements[i].type == 1 && arr->elements[i].value.stringValue) {
            free(arr->elements[i].value.stringValue);
        } else if (arr->elements[i].type == 2) {
            decref_array(arr->elements[i].value.arrayValue);
        } else if (arr->elements[i].type == 4) {
            decref_dict(arr->elements[i].value.dictValue);
        }
    }
    free(arr->elements);
    free(arr);
}

void array_push(Array *arr, double val, char *str, Array *subArr, Dict *subDict, struct Instance *inst) {
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->elements = realloc(arr->elements, sizeof(ArrayElement) * arr->capacity);
    }
    if (str) {
        arr->elements[arr->count].type = 1;
        arr->elements[arr->count].value.stringValue = malloc(strlen(str) + 1);
        strcpy(arr->elements[arr->count].value.stringValue, str);
    } else if (subArr) {
        arr->elements[arr->count].type = 2;
        arr->elements[arr->count].value.arrayValue = subArr;
        incref_array(subArr);
    } else if (subDict) {
        arr->elements[arr->count].type = 4;
        arr->elements[arr->count].value.dictValue = subDict;
        incref_dict(subDict);
    } else if (inst) {
        arr->elements[arr->count].type = 6;
        arr->elements[arr->count].value.instanceValue = inst;
        // incref_instance(inst); // TODO
    } else {
        arr->elements[arr->count].type = 0;
        arr->elements[arr->count].value.doubleValue = val;
    }
    arr->count++;
}

void array_set(Array *arr, int index, double val, char *str) {
    if (index < 0 || index >= arr->count) {
        printf("Błąd: indeks poza zakresem\n");
        return;
    }
    // Free old string if exists
    if (arr->elements[index].type == 1 && arr->elements[index].value.stringValue) {
        free(arr->elements[index].value.stringValue);
    } else if (arr->elements[index].type == 2) {
        decref_array(arr->elements[index].value.arrayValue);
    } else if (arr->elements[index].type == 4) {
        decref_dict(arr->elements[index].value.dictValue);
    }
    
    if (str) {
        arr->elements[index].type = 1;
        arr->elements[index].value.stringValue = malloc(strlen(str) + 1);
        strcpy(arr->elements[index].value.stringValue, str);
    } else {
        arr->elements[index].type = 0;
        arr->elements[index].value.doubleValue = val;
    }
}

// Dict helpers
Dict *create_dict() {
    Dict *d = malloc(sizeof(Dict));
    d->count = 0;
    d->capacity = 8;
    d->ref_count = 1;
    d->entries = malloc(sizeof(DictEntry) * d->capacity);
    return d;
}

void free_dict(Dict *d) {
    if (!d) return;
    for (int i=0; i<d->count; i++) {
        if (d->entries[i].key) free(d->entries[i].key);
        if (d->entries[i].value.type == 1 && d->entries[i].value.value.stringValue) {
            free(d->entries[i].value.value.stringValue);
        } else if (d->entries[i].value.type == 2) {
            decref_array(d->entries[i].value.value.arrayValue);
        } else if (d->entries[i].value.type == 4) {
            decref_dict(d->entries[i].value.value.dictValue);
        }
    }
    free(d->entries);
    free(d);
}

void dict_set(Dict *d, const char *key, double val, char *str, Array *arr, Dict *subDict, struct Instance *inst) {
    // Check if key exists
    for (int i=0; i<d->count; i++) {
        if (strcmp(d->entries[i].key, key) == 0) {
            // Update
            if (d->entries[i].value.type == 1 && d->entries[i].value.value.stringValue) {
                free(d->entries[i].value.value.stringValue);
            } else if (d->entries[i].value.type == 2) {
                decref_array(d->entries[i].value.value.arrayValue);
            } else if (d->entries[i].value.type == 4) {
                decref_dict(d->entries[i].value.value.dictValue);
            }
            
            if (str) {
                d->entries[i].value.type = 1;
                d->entries[i].value.value.stringValue = strdup(str);
            } else if (arr) {
                d->entries[i].value.type = 2;
                d->entries[i].value.value.arrayValue = arr;
                incref_array(arr);
            } else if (subDict) {
                d->entries[i].value.type = 4;
                d->entries[i].value.value.dictValue = subDict;
                incref_dict(subDict);
            } else if (inst) {
                d->entries[i].value.type = 6;
                d->entries[i].value.value.instanceValue = inst;
            } else {
                d->entries[i].value.type = 0;
                d->entries[i].value.value.doubleValue = val;
            }
            return;
        }
    }
    
    // Add new
    if (d->count >= d->capacity) {
        d->capacity *= 2;
        d->entries = realloc(d->entries, sizeof(DictEntry) * d->capacity);
    }
    d->entries[d->count].key = strdup(key);
    if (str) {
        d->entries[d->count].value.type = 1;
        d->entries[d->count].value.value.stringValue = strdup(str);
    } else if (arr) {
        d->entries[d->count].value.type = 2;
        d->entries[d->count].value.value.arrayValue = arr;
        incref_array(arr);
    } else if (subDict) {
        d->entries[d->count].value.type = 4;
        d->entries[d->count].value.value.dictValue = subDict;
        incref_dict(subDict);
    } else if (inst) {
        d->entries[d->count].value.type = 6;
        d->entries[d->count].value.value.instanceValue = inst;
    } else {
        d->entries[d->count].value.type = 0;
        d->entries[d->count].value.value.doubleValue = val;
    }
    d->count++;
}

ArrayElement *dict_get(Dict *d, const char *key) {
    for (int i=0; i<d->count; i++) {
        if (strcmp(d->entries[i].key, key) == 0) {
            return &d->entries[i].value;
        }
    }
    return NULL;
}

Variable *get_variable(const char *name)
{
    return env_get(current_env, name);
}

// Deklaracje funkcji
double eval(Node *n);
void free_node(Node *n);

// Global return value register
double return_value = 0;
char *return_string = NULL;
volatile int is_returning = 0;
volatile int is_breaking = 0;
volatile int is_continuing = 0;
Array *return_array = NULL;
Dict *return_dict = NULL;
struct Class *return_class = NULL;
struct Instance *return_instance = NULL;
volatile int is_exception = 0;
double exception_value = 0;
char *exception_string = NULL;

void free_env(Env *env) {
    if (!env) return;
    for (int i=0; i<env->var_count; i++) {
        if (env->variables[i].type == TYPE_ARRAY && env->variables[i].value.arrayValue) {
            decref_array(env->variables[i].value.arrayValue);
        } else if (env->variables[i].type == TYPE_DICT && env->variables[i].value.dictValue) {
            decref_dict(env->variables[i].value.dictValue);
        }
    }
    if (env->variables) free(env->variables);
    free(env);
}

Node *make_number(double value)
{
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_NUMBER;
    n->value = value;
    n->string_value = NULL;
    return n;
}

Node *make_string(const char *text)
{
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_STRING;
    n->value = 0;
    n->var_name = NULL;
    n->string_value = malloc(strlen(text) + 1);
    strcpy(n->string_value, text);
    return n;
}

Node *make_op(int op_type, Node *a, Node *b)
{
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_OPERATION;
    n->op.op_type = op_type;
    n->op.a = a;
    n->op.b = b;
    return n;
}

Node *add_strings(const char *a, const char *b)
{
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    Node *result = malloc(sizeof(Node));
    result->type = NODE_STRING;
    result->string_value = malloc(len_a + len_b + 1);
    strcpy(result->string_value, a);
    strcat(result->string_value, b);
    return result;
}

int pos = 0;

// Binding powers dla operatorów
int get_binding_power(Token t)
{
    if (t.type == TOKEN_DOT) return 100;
    if (t.type == TOKEN_STAR || t.type == TOKEN_SLASH || t.type == TOKEN_MOD) return 20;
    if (t.type == TOKEN_PLUS || t.type == TOKEN_MINUS) return 10;
    if (t.type == TOKEN_LT || t.type == TOKEN_GT || t.type == TOKEN_LTE || t.type == TOKEN_GTE) return 4;
    if (t.type == TOKEN_EQ || t.type == TOKEN_NEQ) return 3;
    if (t.type == TOKEN_AND) return 2;
    if (t.type == TOKEN_OR) return 1;
    return 0;
}

Node *parse_expr_bp(int min_bp)
{
    Node *left = NULL;

    // operand
    if (tokens[pos].type == TOKEN_NUMBER)
    {
        left = make_number(atof(tokens[pos].text));
        pos++;
    }
    else if (tokens[pos].type == TOKEN_IDENT)
    {
        // Sprawdź czy to wywołanie funkcji
        if (tokens[pos+1].type == TOKEN_LPAREN) {
             char *func_name = tokens[pos].text;
             pos += 2; // pomiń nazwę i '('
             
             Node *node = calloc(1, sizeof(Node));
             node->type = NODE_FUNC_CALL;
             node->call.name = malloc(strlen(func_name)+1);
             strcpy(node->call.name, func_name);
             node->call.args = malloc(sizeof(Node*) * 16);
             node->call.arg_count = 0;
             
             if (tokens[pos].type != TOKEN_RPAREN) {
                 while (1) {
                     node->call.args[node->call.arg_count++] = parse_expr_bp(0);
                     if (tokens[pos].type == TOKEN_COMMA) pos++;
                     else break;
                 }
             }
             if (tokens[pos].type == TOKEN_RPAREN) pos++;
             else printf("Błąd: oczekiwano ')' po argumentach funkcji\n");
             
             left = node;
        }
        else 
        {
            // Nie sprawdzamy istnienia zmiennej tutaj, aby umożliwić argumenty funkcji
            // i zmienne lokalne (lub forward references)
            char *var_name = tokens[pos].text;
            pos++;
            left = calloc(1, sizeof(Node));
            left->type = NODE_VARIABLE;
            // Bezpieczne kopiowanie nazwy zmiennej
            left->var_name = malloc(strlen(var_name) + 1);
            strcpy(left->var_name, var_name);
            left->value = 0; // Wartość będzie ustawiona przez eval jeśli potrzebne
            left->string_value = NULL;
        }
    }
    else if (tokens[pos].type == TOKEN_STRING)
    {
        char *str = tokens[pos].text;
        pos++;
        
        // Check for interpolation ${...}
        char *start = str;
        char *p = str;
        Node *current_node = NULL;
        
        while (*p) {
            if (*p == '$' && *(p+1) == '{') {
                // Found start of interpolation
                // Create string node for text before ${
                int len = p - start;
                if (len > 0) {
                    char *part = malloc(len + 1);
                    strncpy(part, start, len);
                    part[len] = 0;
                    Node *str_node = make_string(part);
                    free(part);
                    
                    if (current_node) {
                        current_node = make_op(TOKEN_PLUS, current_node, str_node);
                    } else {
                        current_node = str_node;
                    }
                }
                
                // Parse expression inside ${...}
                p += 2; // skip ${
                char *expr_start = p;
                int brace_depth = 1;
                while (*p && brace_depth > 0) {
                    if (*p == '{') brace_depth++;
                    else if (*p == '}') brace_depth--;
                    if (brace_depth > 0) p++;
                }
                
                if (brace_depth == 0) {
                    // Found matching }
                    int expr_len = p - expr_start;
                    char *expr_text = malloc(expr_len + 1);
                    strncpy(expr_text, expr_start, expr_len);
                    expr_text[expr_len] = 0;
                    
                    // Save global state
                    Token *saved_tokens = tokens;
                    int saved_count = token_count;
                    int saved_pos = pos;
                    int saved_capacity = token_capacity;
                    
                    // Reset global state for inner parsing
                    tokens = NULL;
                    token_count = 0;
                    token_capacity = 0;
                    
                    // Lex and parse inner expression
                    // We need to declare lex() prototype or move it up
                    void lex(const char *src);
                    lex(expr_text);
                    
                    int inner_pos = 0;
                    // Temporarily swap pos/tokens to use parse_expr_bp
                    Token *inner_tokens = tokens;
                    int inner_count = token_count;
                    
                    // tokens is already set by lex() because lex() uses the global 'tokens' variable
                    // But wait, lex() reallocs 'tokens'.
                    // So when we called lex(expr_text), it modified the global 'tokens'.
                    // We need to be careful.
                    // The lex() function uses global 'tokens', 'token_count', 'token_capacity'.
                    // We saved them before calling lex().
                    // So now 'tokens' points to the NEW tokens from inner lexing.
                    
                    // We need to set pos = 0 for the inner parse.
                    pos = 0; 
                    
                    Node *expr_node = parse_expr_bp(0);
                    
                    // Restore global state
                    free(tokens); // Free inner tokens
                    tokens = saved_tokens;
                    token_count = saved_count;
                    token_capacity = saved_capacity;
                    pos = saved_pos;
                    
                    free(expr_text);
                    
                    if (current_node) {
                        current_node = make_op(TOKEN_PLUS, current_node, expr_node);
                    } else {
                        current_node = expr_node;
                    }
                    
                    start = p + 1; // Next part starts after }
                } else {
                    printf("Błąd: brak zamykającego '}' w interpolacji\n");
                }
            }
            p++;
        }
        
        // Add remaining string part
        if (start < p) {
            Node *str_node = make_string(start);
            if (current_node) {
                current_node = make_op(TOKEN_PLUS, current_node, str_node);
            } else {
                current_node = str_node;
            }
        }
        
        left = current_node;
    }
    else if (tokens[pos].type == TOKEN_TRUE)
    {
        pos++;
        left = calloc(1, sizeof(Node));
        left->type = NODE_BOOL;
        left->value = 1;
        left->string_value = NULL;
    }
    else if (tokens[pos].type == TOKEN_FALSE)
    {
        pos++;
        left = calloc(1, sizeof(Node));
        left->type = NODE_BOOL;
        left->value = 0;
        left->string_value = NULL;
    }
    else if (tokens[pos].type == TOKEN_NEW)
    {
        pos++;
        if (tokens[pos].type != TOKEN_IDENT) {
            printf("Błąd: oczekiwano nazwy klasy\n");
            return NULL;
        }
        char *class_name = tokens[pos].text;
        pos++;
        
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_NEW;
        node->new_inst.class_name = strdup(class_name);
        node->new_inst.args = malloc(sizeof(Node*) * 16);
        node->new_inst.arg_count = 0;
        
        if (tokens[pos].type == TOKEN_LPAREN) {
            pos++;
            if (tokens[pos].type != TOKEN_RPAREN) {
                while (1) {
                    node->new_inst.args[node->new_inst.arg_count++] = parse_expr_bp(0);
                    if (tokens[pos].type == TOKEN_COMMA) pos++;
                    else break;
                }
            }
            if (tokens[pos].type == TOKEN_RPAREN) pos++;
        }
        
        left = node;
    }
    else if (tokens[pos].type == TOKEN_THIS)
    {
        pos++;
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_THIS;
        left = node;
    }
    else if (tokens[pos].type == TOKEN_NULL)
    {
        pos++;
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_NULL;
        left = node;
    }
    else if (tokens[pos].type == TOKEN_LBRACKET)
    {
        // Array literal [a, b, c]
        pos++;
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_ARRAY_LITERAL;
        node->block.stmts = malloc(sizeof(Node*) * 64);
        node->block.count = 0;
        
        if (tokens[pos].type != TOKEN_RBRACKET) {
            while (1) {
                node->block.stmts[node->block.count++] = parse_expr_bp(0);
                if (tokens[pos].type == TOKEN_COMMA) pos++;
                else break;
            }
        }
        if (tokens[pos].type == TOKEN_RBRACKET) pos++;
        else printf("Błąd: oczekiwano ']'\n");
        
        left = node;
    }
    else if (tokens[pos].type == TOKEN_LBRACE) {
        // Dict literal { key: val, ... } OR Block { stmt; ... }
        // parse_expr is called for expressions. Blocks are statements.
        // However, parse_stmt calls parse_block if it sees {.
        // If we are here in parse_expr, it MIGHT be a dict literal.
        // But wait, parse_stmt calls parse_expr only after keywords or assignments.
        // If a statement starts with {, it's a block.
        // If an expression starts with {, it's a dict.
        // Ambiguity: { ... } at top level.
        // In JS: { a: 1 } is a block with label 'a'. ({ a: 1 }) is object.
        // Here, let's assume if we are in parse_expr, we want a value, so it's a dict.
        
        pos++; // skip {
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_DICT_LITERAL;
        // We can reuse block.stmts to store keys and values alternately?
        // Or make a new structure. Let's reuse block.stmts but treat them as pairs.
        // Even better: key must be string literal or ident?
        // Let's support string keys only for simplicity.
        
        node->block.stmts = malloc(sizeof(Node*) * 64);
        node->block.count = 0;
        
        if (tokens[pos].type != TOKEN_RBRACE) {
            while (1) {
                // Key
                Node *key = NULL;
                if (tokens[pos].type == TOKEN_STRING || tokens[pos].type == TOKEN_IDENT) {
                    key = make_string(tokens[pos].text);
                    pos++;
                } else {
                    printf("Błąd: klucz słownika musi być napisem\n");
                    return NULL;
                }
                
                if (tokens[pos].type == TOKEN_COLON) pos++;
                else printf("Błąd: oczekiwano ':' po kluczu\n");
                
                Node *val = parse_expr_bp(0);
                
                node->block.stmts[node->block.count++] = key;
                node->block.stmts[node->block.count++] = val;
                
                if (tokens[pos].type == TOKEN_COMMA) pos++;
                else break;
            }
        }
        if (tokens[pos].type == TOKEN_RBRACE) pos++;
        else printf("Błąd: oczekiwano '}'\n");
        
        left = node;
    }
    else if (tokens[pos].type == TOKEN_LPAREN)
    {
        pos++;                   // pomiń '('
        left = parse_expr_bp(0); // rekurencyjnie wczytaj wyrażenie w nawiasie
        if (tokens[pos].type == TOKEN_RPAREN)
        {
            pos++; // pomiń ')'
        }
        else
        {
            printf("Błąd: brakujący ')'\n");
            return NULL;
        }
    }
    else
    {
        printf("Błąd składni: nieoczekiwany token '%s'\n", tokens[pos].text);
        return NULL;
    }

    // operator loop
    while (1)
    {
        // Member access (dot)
        if (tokens[pos].type == TOKEN_DOT) {
            pos++;
            if (tokens[pos].type == TOKEN_IDENT || tokens[pos].type == TOKEN_KEYWORD) {
                char *member_name = tokens[pos].text;
                pos++;
                
                if (tokens[pos].type == TOKEN_LPAREN) {
                    // Method call: obj.method(...)
                    pos++; // skip (
                    Node *node = calloc(1, sizeof(Node));
                    node->type = NODE_METHOD_CALL;
                    node->method.obj = left;
                    node->method.name = malloc(strlen(member_name) + 1);
                    strcpy(node->method.name, member_name);
                    node->method.args = malloc(sizeof(Node*) * 16);
                    node->method.arg_count = 0;
                    
                    if (tokens[pos].type != TOKEN_RPAREN) {
                        while (1) {
                            node->method.args[node->method.arg_count++] = parse_expr_bp(0);
                            if (tokens[pos].type == TOKEN_COMMA) pos++;
                            else break;
                        }
                    }
                    if (tokens[pos].type == TOKEN_RPAREN) pos++;
                    else printf("Błąd: oczekiwano ')' po argumentach metody\n");
                    
                    left = node;
                } else {
                    // Member access: obj.field
                    Node *node = calloc(1, sizeof(Node));
                    node->type = NODE_MEMBER_ACCESS;
                    node->member.obj = left;
                    node->member.name = malloc(strlen(member_name) + 1);
                    strcpy(node->member.name, member_name);
                    left = node;
                }
                continue;
            } else {
                printf("Błąd: oczekiwano nazwy pola po kropce\n");
                return NULL;
            }
        }

        // Postfix operators (array access)
        if (tokens[pos].type == TOKEN_LBRACKET) {
            pos++;
            Node *index = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_RBRACKET) pos++;
            else printf("Błąd: oczekiwano ']'\n");
            
            Node *access = calloc(1, sizeof(Node));
            access->type = NODE_ARRAY_ACCESS;
            access->array_op.index = index;
            access->array_op.obj = left;
            access->array_op.name = NULL;
            
            left = access;
            continue;
        }

        int bp = get_binding_power(tokens[pos]);
        if (bp == 0 || bp < min_bp)
            break;

        int op_type = tokens[pos].type;
        pos++;

        Node *right = parse_expr_bp(bp + 1);
        left = make_op(op_type, left, right);
    }

    return left;
}

Node *parse_stmt();

Node *parse_block() {
    if (tokens[pos].type != TOKEN_LBRACE) {
        return NULL;
    }
    pos++; // pomiń {
    
    Node *block = calloc(1, sizeof(Node));
    block->type = NODE_BLOCK;
    block->block.stmts = malloc(sizeof(Node*) * 128);
    block->block.count = 0;
    
    while (tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
        Node *stmt = parse_stmt();
        if (stmt) {
            block->block.stmts[block->block.count++] = stmt;
        } else {
            pos++; // Pomiń błędny token, aby uniknąć pętli nieskończonej
        }
    }
    
    if (tokens[pos].type == TOKEN_RBRACE) pos++;
    return block;
}

Node *parse_stmt()
{
    // printf("DEBUG: parse_stmt token=%s type=%d\n", tokens[pos].text, tokens[pos].type);
    // fflush(stdout);
    if (tokens[pos].type == TOKEN_LBRACE) {
        return parse_block();
    }

    if (tokens[pos].type == TOKEN_BREAK) {
        pos++;
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_BREAK;
        return node;
    }

    if (tokens[pos].type == TOKEN_CONTINUE) {
        pos++;
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_CONTINUE;
        return node;
    }
    
    if (tokens[pos].type == TOKEN_CLASS)
    {
        pos++;
        if (tokens[pos].type != TOKEN_IDENT) {
            printf("Błąd: oczekiwano nazwy klasy\n");
            return NULL;
        }
        char *name = tokens[pos].text;
        pos++;
        
        char *parent = NULL;
        if (tokens[pos].type == TOKEN_EXTENDS) {
            pos++;
            if (tokens[pos].type != TOKEN_IDENT) {
                printf("Błąd: oczekiwano nazwy klasy bazowej\n");
                return NULL;
            }
            parent = tokens[pos].text;
            pos++;
        }
        
        if (tokens[pos].type != TOKEN_LBRACE) {
            printf("Błąd: oczekiwano '{'\n");
            return NULL;
        }
        pos++;
        
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_CLASS_DEF;
        node->class_def.name = strdup(name);
        if (parent) node->class_def.parent = strdup(parent);
        node->class_def.methods = malloc(sizeof(Node*) * 32);
        node->class_def.method_count = 0;
        
        while (tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
            int is_static = 0;
            if (tokens[pos].type == TOKEN_STATIC) {
                is_static = 1;
                pos++;
            }
            if (tokens[pos].type == TOKEN_PRIVATE || tokens[pos].type == TOKEN_PUBLIC) {
                pos++;
            }
            
            char *method_name = NULL;
            if (tokens[pos].type == TOKEN_CONSTRUCTOR) {
                method_name = "constructor";
                pos++;
            } else if (tokens[pos].type == TOKEN_IDENT) {
                method_name = tokens[pos].text;
                pos++;
            } else {
                if (tokens[pos].type == TOKEN_RBRACE) break;
                printf("Błąd: oczekiwano nazwy metody\n");
                pos++; continue;
            }
            
            Node *method = calloc(1, sizeof(Node));
            method->type = NODE_FUNC_DEF;
            method->func.name = strdup(method_name);
            method->func.args = malloc(sizeof(char*) * 16);
            method->func.arg_count = 0;
            
            if (tokens[pos].type == TOKEN_LPAREN) {
                pos++;
                while (tokens[pos].type == TOKEN_IDENT) {
                    method->func.args[method->func.arg_count] = strdup(tokens[pos].text);
                    method->func.arg_count++;
                    pos++;
                    if (tokens[pos].type == TOKEN_COMMA) pos++;
                }
                if (tokens[pos].type == TOKEN_RPAREN) pos++;
            } else {
                while (tokens[pos].type == TOKEN_IDENT) {
                    method->func.args[method->func.arg_count] = strdup(tokens[pos].text);
                    method->func.arg_count++;
                    pos++;
                    if (tokens[pos].type == TOKEN_COMMA) {
                        pos++;
                    } else {
                        break;
                    }
                }
            }
            
            method->func.body = parse_stmt();
            node->class_def.methods[node->class_def.method_count++] = method;
        }
        if (tokens[pos].type == TOKEN_RBRACE) pos++;
        return node;
    }

    if (tokens[pos].type == TOKEN_KEYWORD)
    {
        if (strcmp(tokens[pos].text, "zmienna") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_IDENT)
            {
                char *var_name = tokens[pos].text;
                pos++;
                if (tokens[pos].type == TOKEN_ASSIGN)
                {
                    pos++;
                    Node *expr = parse_expr_bp(0);
                    if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
                    
                    Node *node = calloc(1, sizeof(Node));
                    node->type = NODE_VAR_DECL;
                    node->var_name = malloc(strlen(var_name) + 1);
                    strcpy(node->var_name, var_name);
                    node->expr = expr;
                    return node;
                }
            }
        }
        else if (strcmp(tokens[pos].text, "pisz") == 0)
        {
            pos++;
            Node *expr = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_PRINT;
            node->expr = expr;
            return node;
        }
        else if (strcmp(tokens[pos].text, "jezeli") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_LPAREN) pos++; // opcjonalny nawias
            Node *cond = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_RPAREN) pos++;
            
            Node *body = parse_stmt(); // zazwyczaj blok
            Node *else_body = NULL;
            
            if (tokens[pos].type == TOKEN_KEYWORD && strcmp(tokens[pos].text, "inaczej") == 0) {
                pos++;
                else_body = parse_stmt();
            }
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_IF;
            node->flow.cond = cond;
            node->flow.body = body;
            node->flow.else_body = else_body;
            return node;
        }
        else if (strcmp(tokens[pos].text, "wkolko") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_LPAREN) pos++;
            Node *cond = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_RPAREN) pos++;
            
            Node *body = parse_stmt();
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_WHILE;
            node->flow.cond = cond;
            node->flow.body = body;
            return node;
        }
        else if (strcmp(tokens[pos].text, "dla") == 0)
        {
            // printf("DEBUG: Parsing dla loop\n");
            // dla (init; cond; step) body
            // Desugar to: { init; wkolko(cond) { body; step; } }
            pos++;
            if (tokens[pos].type == TOKEN_LPAREN) pos++;
            
            Node *init = parse_stmt(); // consumes ;
            
            Node *cond = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *step = parse_stmt(); // might consume ; or not depending on implementation
            // Actually parse_stmt for assignment consumes ; if present.
            // If step is just expression, parse_stmt handles it.
            
            // If step was parsed as assignment/inc/dec, it might have consumed semicolon.
            // But in `for(;; i++)`, there is no semicolon after i++.
            // parse_stmt checks for semicolon at the end.
            // If we are in `dla`, the semicolon is separator or end of loop header.
            // Let's check if parse_stmt consumed it.
            // If parse_stmt sees `i++`, it consumes semicolon if present.
            // In `dla (..; ..; i++)`, `i++` is followed by `)`.
            // So parse_stmt for `i++` will NOT see semicolon and return.
            // That is correct.
            
            if (tokens[pos].type == TOKEN_RPAREN) pos++;
            
            Node *body = parse_stmt();
            
            // Construct AST
            Node *loop_body = calloc(1, sizeof(Node));
            loop_body->type = NODE_BLOCK;
            loop_body->block.stmts = malloc(sizeof(Node*) * 2);
            loop_body->block.count = 2;
            loop_body->block.stmts[0] = body;
            loop_body->block.stmts[1] = step;
            
            Node *loop = calloc(1, sizeof(Node));
            loop->type = NODE_WHILE;
            loop->flow.cond = cond;
            loop->flow.body = loop_body;
            
            Node *block = calloc(1, sizeof(Node));
            block->type = NODE_BLOCK;
            block->block.stmts = malloc(sizeof(Node*) * 2);
            block->block.count = 2;
            block->block.stmts[0] = init;
            block->block.stmts[1] = loop;
            
            return block;
        }
        else if (strcmp(tokens[pos].text, "zwroc") == 0)
        {
            pos++;
            Node *expr = NULL;
            if (tokens[pos].type != TOKEN_SEMICOLON) {
                expr = parse_expr_bp(0);
            }
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_RETURN;
            node->expr = expr;
            return node;
        }
        else if (strcmp(tokens[pos].text, "funkcja") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_IDENT) {
                char *func_name = tokens[pos].text;
                pos++;
                
                Node *node = calloc(1, sizeof(Node));
                node->type = NODE_FUNC_DEF;
                node->func.name = malloc(strlen(func_name) + 1);
                strcpy(node->func.name, func_name);
                node->func.args = malloc(sizeof(char*) * 16);
                node->func.arg_count = 0;
                
                if (tokens[pos].type == TOKEN_LPAREN) {
                    pos++;
                    while (tokens[pos].type == TOKEN_IDENT) {
                        node->func.args[node->func.arg_count] = malloc(strlen(tokens[pos].text) + 1);
                        strcpy(node->func.args[node->func.arg_count], tokens[pos].text);
                        node->func.arg_count++;
                        pos++;
                        if (tokens[pos].type == TOKEN_COMMA) pos++;
                    }
                    if (tokens[pos].type == TOKEN_RPAREN) pos++;
                } else {
                    while (tokens[pos].type == TOKEN_IDENT) {
                        node->func.args[node->func.arg_count] = malloc(strlen(tokens[pos].text) + 1);
                        strcpy(node->func.args[node->func.arg_count], tokens[pos].text);
                        node->func.arg_count++;
                        pos++;
                        if (tokens[pos].type == TOKEN_COMMA) {
                            pos++;
                        } else {
                            break;
                        }
                    }
                }
                
                node->func.body = parse_stmt(); // blok
                return node;
            }
        }

        else if (strcmp(tokens[pos].text, "zlam") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_BREAK;
            return node;
        }
        else if (strcmp(tokens[pos].text, "pomin") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_CONTINUE;
            return node;
        }
        else if (strcmp(tokens[pos].text, "podaj") == 0)
        {
            pos++;
            if (tokens[pos].type != TOKEN_IDENT) {
                printf("Błąd: oczekiwano nazwy zmiennej po 'podaj'\n");
                return NULL;
            }
            char *var_name = tokens[pos].text;
            pos++;
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_INPUT;
            node->var_name = strdup(var_name);
            return node;
        }
    }
    
    if (tokens[pos].type == TOKEN_TRY) {
        pos++;
        Node *try_body = parse_stmt();
        Node *catch_body = NULL;
        char *catch_var = NULL;
        
        if (tokens[pos].type == TOKEN_CATCH) {
            pos++;
            if (tokens[pos].type == TOKEN_LPAREN) {
                pos++;
                if (tokens[pos].type == TOKEN_IDENT) {
                    catch_var = strdup(tokens[pos].text);
                    pos++;
                }
                if (tokens[pos].type == TOKEN_RPAREN) pos++;
            } else if (tokens[pos].type == TOKEN_IDENT) {
                catch_var = strdup(tokens[pos].text);
                pos++;
            }
            catch_body = parse_stmt();
        }
        
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_TRY;
        node->try_catch.try_body = try_body;
        node->try_catch.catch_body = catch_body;
        node->try_catch.catch_var = catch_var;
        return node;
    }
    
    if (tokens[pos].type == TOKEN_THROW) {
        pos++;
        Node *expr = parse_expr_bp(0);
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        
        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_THROW;
        node->expr = expr;
        return node;
    }

    if (tokens[pos].type == TOKEN_SWITCH) {
        pos++;
        if (tokens[pos].type != TOKEN_LPAREN) {
            printf("Błąd: oczekiwano '('\n");
            return NULL;
        }
        pos++;
        Node *expr = parse_expr_bp(0);
        if (tokens[pos].type != TOKEN_RPAREN) {
            printf("Błąd: oczekiwano ')'\n");
            return NULL;
        }
        pos++;
        if (tokens[pos].type != TOKEN_LBRACE) {
            printf("Błąd: oczekiwano '{'\n");
            return NULL;
        }
        pos++;

        Node *node = calloc(1, sizeof(Node));
        node->type = NODE_SWITCH;
        node->switch_stmt.expr = expr;
        node->switch_stmt.cases = malloc(sizeof(Node*) * 32);
        node->switch_stmt.case_count = 0;
        node->switch_stmt.default_case = NULL;

        while (tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
            if (tokens[pos].type == TOKEN_CASE) {
                pos++;
                Node *val = parse_expr_bp(0);
                if (tokens[pos].type != TOKEN_COLON) {
                    printf("Błąd: oczekiwano ':' po przypadek\n");
                    return NULL;
                }
                pos++;
                
                // Parse statements until next case/default/end
                Node *block = calloc(1, sizeof(Node));
                block->type = NODE_BLOCK;
                block->block.stmts = malloc(sizeof(Node*) * 32);
                block->block.count = 0;
                
                while (tokens[pos].type != TOKEN_CASE && tokens[pos].type != TOKEN_DEFAULT && 
                       tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
                    Node *stmt = parse_stmt();
                    if (stmt) block->block.stmts[block->block.count++] = stmt;
                }
                
                Node *case_node = calloc(1, sizeof(Node));
                case_node->type = NODE_CASE;
                case_node->case_stmt.value = val;
                case_node->case_stmt.body = block;
                
                node->switch_stmt.cases[node->switch_stmt.case_count++] = case_node;
            } else if (tokens[pos].type == TOKEN_DEFAULT) {
                pos++;
                if (tokens[pos].type != TOKEN_COLON) {
                    printf("Błąd: oczekiwano ':' po domyslnie\n");
                    return NULL;
                }
                pos++;
                
                Node *block = calloc(1, sizeof(Node));
                block->type = NODE_BLOCK;
                block->block.stmts = malloc(sizeof(Node*) * 32);
                block->block.count = 0;
                
                while (tokens[pos].type != TOKEN_CASE && tokens[pos].type != TOKEN_DEFAULT && 
                       tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
                    Node *stmt = parse_stmt();
                    if (stmt) block->block.stmts[block->block.count++] = stmt;
                }
                
                node->switch_stmt.default_case = block;
            } else {
                // Skip unexpected tokens inside switch block
                pos++;
            }
        }
        if (tokens[pos].type == TOKEN_RBRACE) pos++;
        return node;
    }

    if (tokens[pos].type == TOKEN_FOR) {
        pos++;
        if (tokens[pos].type != TOKEN_LPAREN) {
            printf("Błąd: oczekiwano '('\n");
            return NULL;
        }
        pos++;
        
        // Check for foreach: dla (ident w ...)
        if (tokens[pos].type == TOKEN_IDENT && tokens[pos+1].type == TOKEN_IN) {
            char *var_name = strdup(tokens[pos].text);
            pos += 2; // skip ident and 'w'
            
            Node *collection = parse_expr_bp(0);
            
            if (tokens[pos].type != TOKEN_RPAREN) {
                printf("Błąd: oczekiwano ')'\n");
                return NULL;
            }
            pos++;
            
            Node *body = parse_stmt();
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_FOREACH;
            node->foreach_loop.var_name = var_name;
            node->foreach_loop.collection = collection;
            node->foreach_loop.body = body;
            return node;
        } else {
            // Classic for: dla (init; cond; inc)
            Node *init = NULL;
            if (tokens[pos].type != TOKEN_SEMICOLON) {
                init = parse_stmt(); 
                // parse_stmt consumes semicolon for VAR_DECL and ASSIGN
                // But if parse_stmt returns NULL (e.g. empty statement?), we might be stuck.
                // Also, parse_stmt might NOT consume semicolon if it's just an expression?
                // parse_stmt handles expression statements by calling parse_expr_bp then checking for semicolon.
                // So init should be fully parsed including semicolon.
            } else {
                pos++; // empty init
            }
            
            Node *cond = NULL;
            if (tokens[pos].type != TOKEN_SEMICOLON) {
                cond = parse_expr_bp(0);
            }
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *inc = NULL;
            if (tokens[pos].type != TOKEN_RPAREN) {
                // inc is usually an expression, not a statement (no semicolon)
                // But we can reuse parse_stmt logic if we want assignment?
                // Actually inc is usually `i++` or `i = i + 1`.
                // `i++` is parsed as assignment statement in parse_stmt?
                // No, `i++` is an expression in some languages, statement in others.
                // In mylang, `i++` is a token TOKEN_INC.
                // parse_expr_bp handles `i++`? No, parse_stmt handles `i++` desugaring.
                // So inc should be parsed as a statement but WITHOUT consuming semicolon?
                // Or parsed as expression?
                // If `i++` is only in parse_stmt, we can't use parse_expr_bp for it.
                // Let's try to parse it as statement but handle the missing semicolon.
                
                // Hack: parse_stmt expects semicolon.
                // We can peek and see if it's an assignment or inc/dec.
                // If so, parse it manually here?
                // Or modify parse_stmt to be more flexible?
                // Let's parse it as expression if possible.
                // But `i = i + 1` is assignment, which is a statement in mylang (NODE_ASSIGN).
                // NODE_ASSIGN is not an expression in mylang (it returns 0 in eval).
                // So we need to parse a statement-like thing for inc.
                
                // Let's try to parse it as a statement.
                // But parse_stmt expects semicolon.
                // We can temporarily insert a semicolon token? No.
                // We can parse_stmt and check if it failed?
                
                // Let's duplicate the assignment logic from parse_stmt here for `inc`.
                if (tokens[pos].type == TOKEN_IDENT && 
                    (tokens[pos+1].type == TOKEN_ASSIGN || 
                     tokens[pos+1].type == TOKEN_PLUS_ASSIGN || 
                     tokens[pos+1].type == TOKEN_MINUS_ASSIGN ||
                     tokens[pos+1].type == TOKEN_MUL_ASSIGN ||
                     tokens[pos+1].type == TOKEN_DIV_ASSIGN ||
                     tokens[pos+1].type == TOKEN_MOD_ASSIGN ||
                     tokens[pos+1].type == TOKEN_INC ||
                     tokens[pos+1].type == TOKEN_DEC)) {
                         
                    // It's an assignment or inc/dec
                    // We can call parse_stmt, but it will complain about missing semicolon.
                    // Let's just copy the logic but without semicolon check.
                    
                    if (tokens[pos+1].type == TOKEN_ASSIGN) {
                        char *var_name = tokens[pos].text;
                        pos += 2;
                        Node *expr = parse_expr_bp(0);
                        Node *node = calloc(1, sizeof(Node));
                        node->type = NODE_ASSIGN;
                        node->var_name = strdup(var_name);
                        node->expr = expr;
                        inc = node;
                    } else if (tokens[pos+1].type == TOKEN_INC || tokens[pos+1].type == TOKEN_DEC) {
                        char *var_name = tokens[pos].text;
                        int op_type = tokens[pos+1].type;
                        pos += 2;
                        Node *var_node = calloc(1, sizeof(Node));
                        var_node->type = NODE_VARIABLE;
                        var_node->var_name = strdup(var_name);
                        Node *one = make_number(1);
                        int bin_op = (op_type == TOKEN_INC) ? TOKEN_PLUS : TOKEN_MINUS;
                        Node *op_node = make_op(bin_op, var_node, one);
                        Node *node = calloc(1, sizeof(Node));
                        node->type = NODE_ASSIGN;
                        node->var_name = strdup(var_name);
                        node->expr = op_node;
                        inc = node;
                    } else {
                        // Compound assignment
                        char *var_name = tokens[pos].text;
                        int op_type = tokens[pos+1].type;
                        pos += 2;
                        Node *expr = parse_expr_bp(0);
                        Node *var_node = calloc(1, sizeof(Node));
                        var_node->type = NODE_VARIABLE;
                        var_node->var_name = strdup(var_name);
                        int bin_op = 0;
                        if (op_type == TOKEN_PLUS_ASSIGN) bin_op = TOKEN_PLUS;
                        else if (op_type == TOKEN_MINUS_ASSIGN) bin_op = TOKEN_MINUS;
                        else if (op_type == TOKEN_MUL_ASSIGN) bin_op = TOKEN_STAR;
                        else if (op_type == TOKEN_DIV_ASSIGN) bin_op = TOKEN_SLASH;
                        else if (op_type == TOKEN_MOD_ASSIGN) bin_op = TOKEN_MOD;
                        Node *op_node = make_op(bin_op, var_node, expr);
                        Node *node = calloc(1, sizeof(Node));
                        node->type = NODE_ASSIGN;
                        node->var_name = strdup(var_name);
                        node->expr = op_node;
                        inc = node;
                    }
                } else {
                    inc = parse_expr_bp(0);
                }
            }
            if (tokens[pos].type == TOKEN_RPAREN) pos++;
            
            Node *body = parse_stmt();
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_FOR;
            node->for_loop.init = init;
            node->for_loop.cond = cond;
            node->for_loop.inc = inc;
            node->for_loop.body = body;
            return node;
        }
    }
    
    // Wyrażenie jako instrukcja
    // Sprawdź czy to przypisanie: ident = expr LUB ident[idx] = expr
    if (tokens[pos].type == TOKEN_IDENT) {
        // Lookahead
        if (tokens[pos+1].type == TOKEN_ASSIGN) {
            char *var_name = tokens[pos].text;
            pos += 2;
            Node *expr = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_ASSIGN;
            node->var_name = malloc(strlen(var_name)+1);
            strcpy(node->var_name, var_name);
            node->expr = expr;
            return node;
        }
        else if (tokens[pos+1].type == TOKEN_PLUS_ASSIGN || tokens[pos+1].type == TOKEN_MINUS_ASSIGN ||
                 tokens[pos+1].type == TOKEN_MUL_ASSIGN || tokens[pos+1].type == TOKEN_DIV_ASSIGN ||
                 tokens[pos+1].type == TOKEN_MOD_ASSIGN) {
            char *var_name = tokens[pos].text;
            int op_type = tokens[pos+1].type;
            pos += 2;
            Node *expr = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            // Desugar to: var = var op expr
            Node *var_node = calloc(1, sizeof(Node));
            var_node->type = NODE_VARIABLE;
            var_node->var_name = strdup(var_name);
            
            int bin_op = 0;
            if (op_type == TOKEN_PLUS_ASSIGN) bin_op = TOKEN_PLUS;
            else if (op_type == TOKEN_MINUS_ASSIGN) bin_op = TOKEN_MINUS;
            else if (op_type == TOKEN_MUL_ASSIGN) bin_op = TOKEN_STAR;
            else if (op_type == TOKEN_DIV_ASSIGN) bin_op = TOKEN_SLASH;
            else if (op_type == TOKEN_MOD_ASSIGN) bin_op = TOKEN_MOD;
            
            Node *op_node = make_op(bin_op, var_node, expr);
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_ASSIGN;
            node->var_name = strdup(var_name);
            node->expr = op_node;
            return node;
        }
        else if (tokens[pos+1].type == TOKEN_INC || tokens[pos+1].type == TOKEN_DEC) {
            char *var_name = tokens[pos].text;
            int op_type = tokens[pos+1].type;
            pos += 2;
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            // Desugar to: var = var + 1 (or - 1)
            Node *var_node = calloc(1, sizeof(Node));
            var_node->type = NODE_VARIABLE;
            var_node->var_name = strdup(var_name);
            
            Node *one = make_number(1);
            int bin_op = (op_type == TOKEN_INC) ? TOKEN_PLUS : TOKEN_MINUS;
            
            Node *op_node = make_op(bin_op, var_node, one);
            
            Node *node = calloc(1, sizeof(Node));
            node->type = NODE_ASSIGN;
            node->var_name = strdup(var_name);
            node->expr = op_node;
            return node;
        }
        else if (tokens[pos+1].type == TOKEN_LBRACKET) {
            // Array assignment: ident[idx] = expr
            char *var_name = tokens[pos].text;
            pos += 2; // skip ident and [
            Node *index = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_RBRACKET) pos++;
            else printf("Błąd: oczekiwano ']'\n");
            
            if (tokens[pos].type == TOKEN_ASSIGN) {
                pos++;
                Node *val = parse_expr_bp(0);
                if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
                
                Node *node = calloc(1, sizeof(Node));
                node->type = NODE_ARRAY_ASSIGN;
                node->array_op.name = strdup(var_name);
                node->array_op.index = index;
                node->array_op.value = val;
                return node;
            }
        }
    }

    Node *expr = parse_expr_bp(0);
    if (expr) {
        if (tokens[pos].type == TOKEN_ASSIGN) {
            pos++;
            Node *val = parse_expr_bp(0);
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            
            if (expr->type == NODE_MEMBER_ACCESS) {
                Node *node = calloc(1, sizeof(Node));
                node->type = NODE_MEMBER_ASSIGN;
                node->member.obj = expr->member.obj;
                node->member.name = expr->member.name;
                node->expr = val;
                free(expr);
                return node;
            }
            else if (expr->type == NODE_ARRAY_ACCESS) {
                Node *node = calloc(1, sizeof(Node));
                node->type = NODE_ARRAY_ASSIGN;
                node->array_op.obj = expr->array_op.obj;
                node->array_op.index = expr->array_op.index;
                node->array_op.value = val;
                node->array_op.name = NULL;
                free(expr);
                return node;
            }
             else {
                 printf("Błąd: nieprawidłowe przypisanie\n");
                 return NULL;
            }
        }
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        return expr;
    }
    
    return NULL;
}

void add_token(TokenType type, const char *text)
{
    if (token_count >= token_capacity)
    {
        token_capacity = (token_capacity == 0) ? 256 : token_capacity * 2;
        tokens = realloc(tokens, sizeof(Token) * token_capacity);
    }
    tokens[token_count].type = type;
    strncpy(tokens[token_count].text, text, 63);
    tokens[token_count].text[63] = '\0';
    token_count++;
}

// Tablica słów kluczowych
const char *keywords[] = {
    "pisz",
    "podaj",
    "usun",
    "jezeli",
    "inaczej",
    "dla",
    "w",
    "funkcja",
    "zwroc",
    "zmienna",
    "wkolko",
    "zlam",
    "pomin",
    "prawda",
    "falsz",
    "oraz",
    "albo",
    "rowne",
    "nierowne",
    "klasa",
    "konstruktor",
    "dziedziczy",
    "statyczna",
    "prywatna",
    "publiczna",
    "nowy",
    "to",
    "nic",
    "sprobuj",
    "zlap",
    "rzuc",
    "wybor",
    "przypadek",
    "domyslnie",
    NULL
};
const int keywords_count = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(const char *text)
{
    for (int i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(text, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

int is_ident_char(char c) {
    return isalnum(c) || (unsigned char)c > 127 || c == '_';
}

void lex(const char *src)
{
    // printf("DEBUG: Lex start\n");
    // fflush(stdout);
    int i = 0;
    while (src[i] != '\0')
    {
        if (src[i] == '#') {
            while (src[i] != '\0' && src[i] != '\n') i++;
            continue;
        }

        if (isspace(src[i]))
        {
            i++;
            continue;
        }

        if (src[i] == '"' || src[i] == '\'')
        {        // string w cudzysłowie (pojedynczym lub podwójnym)
            char quote = src[i];
            i++; // pomiń otwierający cudzysłów
            int start = i;
            while (src[i] != quote && src[i] != '\0')
                i++;
            int len = i - start;
            char buf[1024];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            add_token(TOKEN_STRING, buf);
            if (src[i] == quote)
                i++; // pomiń zamykający cudzysłów
            continue;
        }

        if (is_ident_char(src[i]) && !isdigit(src[i]))
        { // ident lub keyword
            int start = i;
            while (is_ident_char(src[i]))
                i++;
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            if (strcmp(buf, "klasa") == 0) add_token(TOKEN_CLASS, buf);
            else if (strcmp(buf, "konstruktor") == 0) add_token(TOKEN_CONSTRUCTOR, buf);
            else if (strcmp(buf, "dziedziczy") == 0) add_token(TOKEN_EXTENDS, buf);
            else if (strcmp(buf, "statyczna") == 0) add_token(TOKEN_STATIC, buf);
            else if (strcmp(buf, "prywatna") == 0) add_token(TOKEN_PRIVATE, buf);
            else if (strcmp(buf, "publiczna") == 0) add_token(TOKEN_PUBLIC, buf);
            else if (strcmp(buf, "nowy") == 0) add_token(TOKEN_NEW, buf);
            else if (strcmp(buf, "to") == 0) add_token(TOKEN_THIS, buf);
            else if (strcmp(buf, "nic") == 0) add_token(TOKEN_NULL, buf);
            else if (strcmp(buf, "sprobuj") == 0) add_token(TOKEN_TRY, buf);
            else if (strcmp(buf, "zlap") == 0) add_token(TOKEN_CATCH, buf);
            else if (strcmp(buf, "rzuc") == 0) add_token(TOKEN_THROW, buf);
            else if (strcmp(buf, "wybor") == 0) add_token(TOKEN_SWITCH, buf);
            else if (strcmp(buf, "przypadek") == 0) add_token(TOKEN_CASE, buf);
            else if (strcmp(buf, "domyslnie") == 0) add_token(TOKEN_DEFAULT, buf);
            else if (strcmp(buf, "dla") == 0) add_token(TOKEN_FOR, buf);
            else if (strcmp(buf, "w") == 0) add_token(TOKEN_IN, buf);
            else if (is_keyword(buf))
            {
                if (strcmp(buf, "prawda") == 0) add_token(TOKEN_TRUE, buf);
                else if (strcmp(buf, "falsz") == 0) add_token(TOKEN_FALSE, buf);
                else if (strcmp(buf, "oraz") == 0) add_token(TOKEN_AND, buf);
                else if (strcmp(buf, "albo") == 0) add_token(TOKEN_OR, buf);
                else if (strcmp(buf, "zlam") == 0) add_token(TOKEN_BREAK, buf);
                else if (strcmp(buf, "pomin") == 0) add_token(TOKEN_CONTINUE, buf);
                else if (strcmp(buf, "rowne") == 0) add_token(TOKEN_EQ, buf);
                else if (strcmp(buf, "nierowne") == 0) add_token(TOKEN_NEQ, buf);
                else add_token(TOKEN_KEYWORD, buf);
            }
            else
            {
                add_token(TOKEN_IDENT, buf);
            }
            continue;
        }

        if (isdigit(src[i]))
        { // liczba (z obsługą kropki dziesiętnej)
            int start = i;
            while (isdigit(src[i]))
                i++;
            // Sprawdź czy jest kropka dziesiętna
            if (src[i] == '.')
            {
                i++; // pomiń kropkę
                while (isdigit(src[i]))
                    i++;
            }
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            add_token(TOKEN_NUMBER, buf);
            continue;
        }

        // kluczowe znaki
        char c = src[i];
        char buf[2] = {c, '\0'};
        if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '(' || c == ')' || c == ';' || c == '{' || c == '}' || c == ',' ||
            c == '<' || c == '>' || c == '!' || c == '&' || c == '|' ||
            c == '[' || c == ']' || c == '.' || c == ':' || c == '%')
        {
            switch (c)
            {
            case '.':
                add_token(TOKEN_DOT, ".");
                break;
            case ':':
                add_token(TOKEN_COLON, ":");
                break;
            case '=':
                if (src[i+1] == '=') { add_token(TOKEN_EQ, "=="); i++; }
                else { add_token(TOKEN_ASSIGN, "="); }
                break;
            case '!':
                if (src[i+1] == '=') { add_token(TOKEN_NEQ, "!="); i++; }
                else { printf("Błąd: nieoczekiwany znak '!'\n"); }
                break;
            case '<':
                if (src[i+1] == '=') { add_token(TOKEN_LTE, "<="); i++; }
                else { add_token(TOKEN_LT, "<"); }
                break;
            case '>':
                if (src[i+1] == '=') { add_token(TOKEN_GTE, ">="); i++; }
                else { add_token(TOKEN_GT, ">"); }
                break;
            case '&':
                if (src[i+1] == '&') { add_token(TOKEN_AND, "&&"); i++; }
                else { printf("Błąd: oczekiwano '&&'\n"); }
                break;
            case '|':
                if (src[i+1] == '|') { add_token(TOKEN_OR, "||"); i++; }
                else { printf("Błąd: oczekiwano '||'\n"); }
                break;
            case '+':
                if (src[i+1] == '+') { add_token(TOKEN_INC, "++"); i++; }
                else if (src[i+1] == '=') { add_token(TOKEN_PLUS_ASSIGN, "+="); i++; }
                else { add_token(TOKEN_PLUS, "+"); }
                break;
            case '-':
                if (src[i+1] == '-') { add_token(TOKEN_DEC, "--"); i++; }
                else if (src[i+1] == '=') { add_token(TOKEN_MINUS_ASSIGN, "-="); i++; }
                else { add_token(TOKEN_MINUS, "-"); }
                break;
            case '*':
                if (src[i+1] == '=') { add_token(TOKEN_MUL_ASSIGN, "*="); i++; }
                else { add_token(TOKEN_STAR, "*"); }
                break;
            case '%':
                if (src[i+1] == '=') { add_token(TOKEN_MOD_ASSIGN, "%="); i++; }
                else { add_token(TOKEN_MOD, "%"); }
                break;
            case '/':
                if (src[i+1] == '/') {
                    // Komentarz jednoliniowy
                    i += 2;
                    while (src[i] != '\0' && src[i] != '\n') i++;
                    continue;
                }
                else if (src[i+1] == '*') {
                    // Komentarz wieloliniowy
                    i += 2;
                    while (src[i] != '\0') {
                        if (src[i] == '*' && src[i+1] == '/') {
                            i += 2;
                            break;
                        }
                        i++;
                    }
                    continue;
                }
                else if (src[i+1] == '=') { add_token(TOKEN_DIV_ASSIGN, "/="); i++; }
                else { add_token(TOKEN_SLASH, "/"); }
                break;
            case '(':
                add_token(TOKEN_LPAREN, buf);
                break;
            case ')':
                add_token(TOKEN_RPAREN, buf);
                break;
            case '[':
                add_token(TOKEN_LBRACKET, buf);
                break;
            case ']':
                add_token(TOKEN_RBRACKET, buf);
                break;
            case '{':
                add_token(TOKEN_LBRACE, buf);
                break;
            case '}':
                add_token(TOKEN_RBRACE, buf);
                break;
            case ';':
                add_token(TOKEN_SEMICOLON, buf);
                break;
            case ',':
                add_token(TOKEN_COMMA, buf);
                break;
            }
        }
        else
        {
            add_token(TOKEN_IDENT, buf); // traktuj nieznane jako ident
        }
        i++;
    }
    add_token(TOKEN_EOF, "EOF");
    // printf("DEBUG: Lex end\n");
    // fflush(stdout);
}

char *get_node_string(Node *n) {
    if (!n) return NULL;
    if (n->string_value) return n->string_value;
    if (n->type == NODE_VARIABLE) {
        Variable *v = get_variable(n->var_name);
        if (v && v->type == TYPE_STRING) return v->value.stringValue;
    }
    return NULL;
}
 
double eval(Node *n)
{
    if (!n) return 0;
    // printf("DEBUG: eval type %d flags R%d B%d C%d\n", n->type, is_returning, is_breaking, is_continuing);
    if (is_returning || is_breaking || is_continuing || is_exception) return 0;

    else if (n->type == NODE_FOR) {
        Env *prev_env = current_env;
        current_env = create_env(current_env); // New scope for loop vars
        
        if (n->for_loop.init) eval(n->for_loop.init);
        
        while (1) {
            if (n->for_loop.cond) {
                double c = eval(n->for_loop.cond);
                if (c == 0) break;
            }
            
            eval(n->for_loop.body);
            
            if (is_breaking) {
                is_breaking = 0;
                break;
            }
            if (is_continuing) {
                is_continuing = 0;
            }
            
            if (n->for_loop.inc) eval(n->for_loop.inc);
            
            if (is_returning || is_exception) break;
        }
        
        Env *temp = current_env;
        current_env = prev_env;
        free_env(temp);
        return 0;
    }
    else if (n->type == NODE_FOREACH) {
        Node *col_node = n->foreach_loop.collection;
        eval(col_node);
        
        Array *arr = NULL;
        Dict *dict = NULL;
        char *str = NULL;
        
        if (col_node->array_value) arr = col_node->array_value;
        else if (col_node->dict_value) dict = col_node->dict_value;
        else if (col_node->string_value) str = col_node->string_value;
        else if (col_node->type == NODE_VARIABLE) {
            Variable *v = get_variable(col_node->var_name);
            if (v) {
                if (v->type == TYPE_ARRAY) arr = v->value.arrayValue;
                else if (v->type == TYPE_DICT) dict = v->value.dictValue;
                else if (v->type == TYPE_STRING) str = v->value.stringValue;
            }
        }
        
        if (!arr && !dict && !str) {
            printf("Błąd: pętla 'dla' wymaga tablicy, słownika lub napisu\n");
            return 0;
        }
        
        Env *prev_env = current_env;
        current_env = create_env(current_env);
        
        if (arr) {
            for (int i=0; i<arr->count; i++) {
                Variable *var = env_define(current_env, n->foreach_loop.var_name);
                // Copy value to var
                if (arr->elements[i].type == 0) {
                    var->type = TYPE_DOUBLE;
                    var->value.doubleValue = arr->elements[i].value.doubleValue;
                } else if (arr->elements[i].type == 1) {
                    var->type = TYPE_STRING;
                    strncpy(var->value.stringValue, arr->elements[i].value.stringValue, 63);
                } else if (arr->elements[i].type == 2) {
                    var->type = TYPE_ARRAY;
                    var->value.arrayValue = arr->elements[i].value.arrayValue;
                } else if (arr->elements[i].type == 4) {
                    var->type = TYPE_DICT;
                    var->value.dictValue = arr->elements[i].value.dictValue;
                }
                
                eval(n->foreach_loop.body);
                
                if (is_breaking) { is_breaking = 0; break; }
                if (is_continuing) { is_continuing = 0; }
                if (is_returning || is_exception) break;
            }
        } else if (dict) {
            for (int i=0; i<dict->count; i++) {
                Variable *var = env_define(current_env, n->foreach_loop.var_name);
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, dict->entries[i].key, 63);
                
                eval(n->foreach_loop.body);
                
                if (is_breaking) { is_breaking = 0; break; }
                if (is_continuing) { is_continuing = 0; }
                if (is_returning || is_exception) break;
            }
        } else if (str) {
            char buf[2] = {0};
            for (int i=0; str[i]; i++) {
                Variable *var = env_define(current_env, n->foreach_loop.var_name);
                var->type = TYPE_STRING;
                buf[0] = str[i];
                strncpy(var->value.stringValue, buf, 63);
                
                eval(n->foreach_loop.body);
                
                if (is_breaking) { is_breaking = 0; break; }
                if (is_continuing) { is_continuing = 0; }
                if (is_returning || is_exception) break;
            }
        }
        
        Env *temp = current_env;
        current_env = prev_env;
        free_env(temp);
        return 0;
    }
    else if (n->type == NODE_TRY) {
        eval(n->try_catch.try_body);
        if (is_exception) {
            is_exception = 0; // Caught
            if (n->try_catch.catch_body) {
                Env *prev_env = current_env;
                current_env = create_env(current_env);
                
                if (n->try_catch.catch_var) {
                    Variable *var = env_define(current_env, n->try_catch.catch_var);
                    if (exception_string) {
                        var->type = TYPE_STRING;
                        strncpy(var->value.stringValue, exception_string, 63);
                        free(exception_string); exception_string = NULL;
                    } else {
                        var->type = TYPE_DOUBLE;
                        var->value.doubleValue = exception_value;
                    }
                } else {
                    if (exception_string) { free(exception_string); exception_string = NULL; }
                }
                
                eval(n->try_catch.catch_body);
                
                Env *temp = current_env;
                current_env = prev_env;
                free_env(temp);
            } else {
                if (exception_string) { free(exception_string); exception_string = NULL; }
            }
        }
        return 0;
    }
    else if (n->type == NODE_THROW) {
        double val = eval(n->expr);
        is_exception = 1;
        if (n->expr->string_value) {
            exception_string = strdup(n->expr->string_value);
        } else {
            exception_value = val;
        }
        return 0;
    }
    else if (n->type == NODE_NULL) {
        return 0; // Just return 0, but type handling elsewhere might need update
    }
    else if (n->type == NODE_SWITCH) {
        double target_val = eval(n->switch_stmt.expr);
        char *target_str = NULL;
        if (n->switch_stmt.expr->string_value) {
            target_str = strdup(n->switch_stmt.expr->string_value);
        }
        
        int matched = 0;
        for (int i = 0; i < n->switch_stmt.case_count; i++) {
            Node *case_node = n->switch_stmt.cases[i];
            if (!matched) {
                double case_val = eval(case_node->case_stmt.value);
                char *case_str = NULL;
                if (case_node->case_stmt.value->string_value) {
                    case_str = case_node->case_stmt.value->string_value;
                }
                
                if (target_str && case_str) {
                    if (strcmp(target_str, case_str) == 0) matched = 1;
                } else if (!target_str && !case_str) {
                    if (target_val == case_val) matched = 1;
                }
            }
            
            if (matched) {
                eval(case_node->case_stmt.body);
                if (is_breaking) {
                    is_breaking = 0;
                    if (target_str) free(target_str);
                    return 0;
                }
                if (is_returning || is_exception) {
                    if (target_str) free(target_str);
                    return 0;
                }
            }
        }
        
        if (!matched && n->switch_stmt.default_case) {
            eval(n->switch_stmt.default_case);
            if (is_breaking) is_breaking = 0;
        }
        
        if (target_str) free(target_str);
        return 0;
    }
    else if (n->type == NODE_CLASS_DEF) {
        Class *cls = malloc(sizeof(Class));
        cls->name = strdup(n->class_def.name);
        cls->parent = n->class_def.parent ? strdup(n->class_def.parent) : NULL;
        cls->method_count = n->class_def.method_count;
        cls->methods = malloc(sizeof(Function) * cls->method_count);
        
        for (int i=0; i<cls->method_count; i++) {
            Node *method_node = n->class_def.methods[i];
            Function *f = &cls->methods[i];
            strcpy(f->name, method_node->func.name);
            f->body = method_node->func.body;
            f->arg_count = method_node->func.arg_count;
            f->args = malloc(sizeof(char*) * f->arg_count);
            for (int j=0; j<f->arg_count; j++) {
                f->args[j] = strdup(method_node->func.args[j]);
            }
        }
        
        Variable *var = env_define(current_env, cls->name);
        var->type = TYPE_CLASS;
        var->value.classValue = cls;
        return 0;
    }
    else if (n->type == NODE_NEW) {
        Variable *var = env_get(current_env, n->new_inst.class_name);
        if (!var || var->type != TYPE_CLASS) {
            printf("Błąd: nieznana klasa %s\n", n->new_inst.class_name);
            return 0;
        }
        Class *cls = var->value.classValue;
        
        Instance *inst = malloc(sizeof(Instance));
        inst->cls = cls;
        inst->fields = create_dict();
        inst->ref_count = 1;
        
        // Call constructor
        Function *ctor = NULL;
        Class *curr_cls = cls;
        while (curr_cls) {
            for (int i=0; i<curr_cls->method_count; i++) {
                if (strcmp(curr_cls->methods[i].name, "constructor") == 0) {
                    ctor = &curr_cls->methods[i];
                    break;
                }
            }
            if (ctor) break;
            
            if (curr_cls->parent) {
                Variable *v = env_get(global_env, curr_cls->parent);
                if (v && v->type == TYPE_CLASS) {
                    curr_cls = v->value.classValue;
                } else {
                    curr_cls = NULL;
                }
            } else {
                curr_cls = NULL;
            }
        }
        
        if (ctor) {
            // Pre-evaluate arguments in the current scope
            double arg_vals[16];
            char *arg_strs[16] = {0};
            struct Instance *arg_insts[16] = {0};
            
            for (int i=0; i<n->new_inst.arg_count && i < 16; i++) {
                arg_vals[i] = eval(n->new_inst.args[i]);
                
                if (return_instance) {
                    arg_insts[i] = return_instance;
                    return_instance = NULL;
                }
                
                if (n->new_inst.args[i]->string_value) {
                    arg_strs[i] = strdup(n->new_inst.args[i]->string_value);
                }
            }

            Env *prev_env = current_env;
            Instance *prev_inst = current_instance;
            
            current_env = create_env(global_env); 
            current_instance = inst;
            
            for (int i=0; i<ctor->arg_count; i++) {
                double val = 0;
                char *str = NULL;
                struct Instance *ins = NULL;
                
                if (i < n->new_inst.arg_count) {
                    val = arg_vals[i];
                    str = arg_strs[i];
                    ins = arg_insts[i];
                }
                
                Variable *arg = env_define(current_env, ctor->args[i]);
                if (ins) {
                    arg->type = TYPE_INSTANCE;
                    arg->value.instanceValue = ins;
                } else if (str) {
                    arg->type = TYPE_STRING;
                    strcpy(arg->value.stringValue, str);
                    free(str);
                } else {
                    arg->type = TYPE_DOUBLE;
                    arg->value.doubleValue = val;
                }
            }
            
            eval(ctor->body);
            
            current_env = prev_env;
            current_instance = prev_inst;
        }
        
        return_instance = inst;
        return 0;
    }
    else if (n->type == NODE_THIS) {
        if (current_instance) {
            return_instance = current_instance;
            return 0;
        } else {
            printf("Błąd: użycie 'to' poza metodą\n");
            return 0;
        }
    }

    if (n->type == NODE_BLOCK) {
        current_env = create_env(current_env); // Push scope
        for (int i=0; i<n->block.count; i++) {
            eval(n->block.stmts[i]);
            if (is_returning || is_breaking || is_continuing) break;
        }
        Env *parent = current_env->parent;
        free_env(current_env); // cleanup vars
        current_env = parent; // Pop scope
        return 0;
    }
    else if (n->type == NODE_ARRAY_LITERAL) {
        if (n->array_value) return 0;
        Array *arr = create_array();
        for (int i=0; i<n->block.count; i++) {
            double val = eval(n->block.stmts[i]);
            if (n->block.stmts[i]->string_value) {
                array_push(arr, 0, n->block.stmts[i]->string_value, NULL, NULL, NULL);
            } else if (n->block.stmts[i]->array_value) {
                array_push(arr, 0, NULL, n->block.stmts[i]->array_value, NULL, NULL);
            } else if (n->block.stmts[i]->dict_value) {
                array_push(arr, 0, NULL, NULL, n->block.stmts[i]->dict_value, NULL);
            } else {
                array_push(arr, val, NULL, NULL, NULL, NULL);
            }
        }
        n->array_value = arr;
        return 0;
    }
    else if (n->type == NODE_DICT_LITERAL) {
        Dict *d = create_dict();
        for (int i=0; i<n->block.count; i+=2) {
            char *key = n->block.stmts[i]->string_value;
            Node *valNode = n->block.stmts[i+1];
            double val = eval(valNode);
            
            if (valNode->string_value) {
                dict_set(d, key, 0, valNode->string_value, NULL, NULL, NULL);
            } else if (valNode->array_value) {
                dict_set(d, key, 0, NULL, valNode->array_value, NULL, NULL);
            } else if (valNode->dict_value) {
                dict_set(d, key, 0, NULL, NULL, valNode->dict_value, NULL);
            } else {
                dict_set(d, key, val, NULL, NULL, NULL, NULL);
            }
        }
        n->dict_value = d;
        return 0;
    }
    else if (n->type == NODE_ARRAY_ACCESS) {
        Node *obj = n->array_op.obj;
        if (obj) {
            eval(obj);
            
            Array *arr = NULL;
            Dict *dict = NULL;
            
            if (obj->type == NODE_ARRAY_LITERAL || obj->array_value) {
                arr = obj->array_value;
            } else if (obj->type == NODE_DICT_LITERAL || obj->dict_value) {
                dict = obj->dict_value;
            } else if (obj->type == NODE_VARIABLE) {
                 arr = obj->array_value;
                 dict = obj->dict_value;
            }
            
            if (dict) {
                eval(n->array_op.index);
                char *key = get_node_string(n->array_op.index);
                if (key) {
                    // key is valid
                } else {
                    printf("Błąd: klucz słownika musi być napisem\n");
                    return 0;
                }
                
                ArrayElement *el = dict_get(dict, key);
                if (el) {
                    if (el->type == 0) {
                        if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                        return el->value.doubleValue;
                    }
                    if (el->type == 1) {
                        if (n->string_value) free(n->string_value);
                        n->string_value = strdup(el->value.stringValue);
                        return 0;
                    }
                    if (el->type == 2) {
                        if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                        n->array_value = el->value.arrayValue;
                        return 0;
                    }
                    if (el->type == 3) {
                        if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                        return el->value.intValue;
                    }
                    if (el->type == 4) {
                        if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                        n->dict_value = el->value.dictValue;
                        return 0;
                    }
                }
                return 0;
            }
            
            if (arr) {
                double idx_d = eval(n->array_op.index);
                int idx = (int)idx_d;
                if (idx < 0 || idx >= arr->count) {
                    printf("Błąd: indeks %d poza zakresem (rozmiar %d)\n", idx, arr->count);
                    return 0;
                }
                if (arr->elements[idx].type == 0) {
                    if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                    return arr->elements[idx].value.doubleValue;
                }
                if (arr->elements[idx].type == 1) {
                    if (n->string_value) free(n->string_value);
                    n->string_value = strdup(arr->elements[idx].value.stringValue);
                    return 0;
                }
                if (arr->elements[idx].type == 2) {
                    if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                    n->array_value = arr->elements[idx].value.arrayValue;
                    return 0;
                }
                if (arr->elements[idx].type == 4) {
                    if (n->string_value) { free(n->string_value); n->string_value = NULL; }
                    n->dict_value = arr->elements[idx].value.dictValue;
                    return 0;
                }
                return 0;
            }
            
            printf("Błąd: obiekt nie jest tablicą ani słownikiem\n");
            return 0;
        }
        return 0;
    }
    else if (n->type == NODE_ARRAY_ASSIGN) {
        Array *arr = NULL;
        Dict *dict = NULL;
        
        if (n->array_op.name) {
            Variable *var = get_variable(n->array_op.name);
            if (!var) { printf("Błąd: nieznana zmienna %s\n", n->array_op.name); return 0; }
            if (var->type == TYPE_ARRAY) arr = var->value.arrayValue;
            else if (var->type == TYPE_DICT) dict = var->value.dictValue;
            else { printf("Błąd: zmienna %s nie jest tablicą ani słownikiem\n", n->array_op.name); return 0; }
        } else if (n->array_op.obj) {
            eval(n->array_op.obj);
            if (n->array_op.obj->array_value) arr = n->array_op.obj->array_value;
            else if (n->array_op.obj->dict_value) dict = n->array_op.obj->dict_value;
            else if (n->array_op.obj->type == NODE_VARIABLE) {
                 Variable *v = get_variable(n->array_op.obj->var_name);
                 if (v) {
                     if (v->type == TYPE_ARRAY) arr = v->value.arrayValue;
                     else if (v->type == TYPE_DICT) dict = v->value.dictValue;
                 }
            }
            if (!arr && !dict) { printf("Błąd: obiekt nie jest tablicą ani słownikiem\n"); return 0; }
        }
        
        if (dict) {
            eval(n->array_op.index);
            char *key = NULL;
            if (n->array_op.index->string_value) {
                key = n->array_op.index->string_value;
            } else {
                printf("Błąd: klucz słownika musi być napisem\n");
                return 0;
            }
            
            double val = eval(n->array_op.value);
            if (n->array_op.value->type == NODE_ARRAY_LITERAL) {
                dict_set(dict, key, 0, NULL, n->array_op.value->array_value, NULL, NULL);
            } else if (n->array_op.value->type == NODE_DICT_LITERAL) {
                dict_set(dict, key, 0, NULL, NULL, n->array_op.value->dict_value, NULL);
            } else if (n->array_op.value->string_value) {
                dict_set(dict, key, 0, n->array_op.value->string_value, NULL, NULL, NULL);
            } else {
                dict_set(dict, key, val, NULL, NULL, NULL, NULL);
            }
            return 0;
        }
        
        if (arr) {
            double idx_d = eval(n->array_op.index);
            int idx = (int)idx_d;
            double val = eval(n->array_op.value);
            
            if (n->array_op.value->string_value) {
                array_set(arr, idx, 0, n->array_op.value->string_value);
            } else {
                array_set(arr, idx, val, NULL);
            }
            return 0;
        }
        return 0;
    }
    else if (n->type == NODE_MEMBER_ASSIGN) {
        Instance *inst = NULL;
        Dict *dict = NULL;
        
        if (n->member.obj->type == NODE_VARIABLE) {
            Variable *var = get_variable(n->member.obj->var_name);
            if (var) {
                if (var->type == TYPE_INSTANCE) {
                    inst = var->value.instanceValue;
                } else if (var->type == TYPE_DICT) {
                    dict = var->value.dictValue;
                }
            }
        } else if (n->member.obj->type == NODE_THIS) {
            inst = current_instance;
        } else {
             eval(n->member.obj);
             if (return_instance) {
                 inst = return_instance;
                 return_instance = NULL;
             } else if (n->member.obj->dict_value) {
                 dict = n->member.obj->dict_value;
             }
        }
        
        if (inst || dict) {
            Dict *target = inst ? inst->fields : dict;
            
            double val = eval(n->expr);
            char *str_val = NULL;
            Array *arr_val = NULL;
            Dict *dict_val = NULL;
            struct Instance *inst_val = NULL;
            
            if (return_instance) {
                inst_val = return_instance;
                return_instance = NULL;
            }
            
            if (n->expr->type == NODE_ARRAY_LITERAL || n->expr->array_value) {
                arr_val = n->expr->array_value;
            } else if (n->expr->type == NODE_DICT_LITERAL || n->expr->dict_value) {
                dict_val = n->expr->dict_value;
            } else if (n->expr->string_value) {
                str_val = n->expr->string_value;
            }
            
            if (inst_val) {
                dict_set(target, n->member.name, 0, NULL, NULL, NULL, inst_val);
            } else if (arr_val) {
                dict_set(target, n->member.name, 0, NULL, arr_val, NULL, NULL);
            } else if (dict_val) {
                dict_set(target, n->member.name, 0, NULL, NULL, dict_val, NULL);
            } else if (str_val) {
                dict_set(target, n->member.name, 0, str_val, NULL, NULL, NULL);
            } else {
                dict_set(target, n->member.name, val, NULL, NULL, NULL, NULL);
            }
            return 0;
        }
        printf("Błąd: przypisanie do pola obiektu, który nie jest instancją ani słownikiem\n");
        return 0;
    }
    else if (n->type == NODE_ASSIGN) {
        double val = eval(n->expr);
        // printf("DEBUG: Assign %s = %f\n", n->var_name, val);
        char *str_val = NULL;
        Array *arr_val = NULL;
        Dict *dict_val = NULL;
        struct Instance *inst_val = NULL;
        int is_bool = (n->expr->type == NODE_BOOL);
        
        if (return_instance) {
            inst_val = return_instance;
            return_instance = NULL;
        }
        
        // Check if expression resulted in boolean (e.g. comparison)
        if (n->expr->type == NODE_OPERATION) {
             int op = n->expr->op.op_type;
             if (op == TOKEN_EQ || op == TOKEN_NEQ || op == TOKEN_LT || op == TOKEN_GT || 
                 op == TOKEN_LTE || op == TOKEN_GTE || op == TOKEN_AND || op == TOKEN_OR) {
                 is_bool = 1;
             }
        }
        
        if (n->expr->type == NODE_ARRAY_LITERAL || n->expr->array_value) {
            arr_val = n->expr->array_value;
        } else if (n->expr->type == NODE_DICT_LITERAL || n->expr->dict_value) {
            dict_val = n->expr->dict_value;
        } else if (n->expr->string_value) {
            str_val = n->expr->string_value;
        }
        
        Variable *var = get_variable(n->var_name);
        if (var) {
            // printf("DEBUG: Found var %s at %p (val=%f)\n", n->var_name, var, var->value.doubleValue);
            // Cleanup old value
            if (var->type == TYPE_ARRAY) decref_array(var->value.arrayValue);
            if (var->type == TYPE_DICT) decref_dict(var->value.dictValue);
            
            if (inst_val) {
                var->type = TYPE_INSTANCE;
                var->value.instanceValue = inst_val;
            } else if (arr_val) {
                var->type = TYPE_ARRAY;
                var->value.arrayValue = arr_val;
                incref_array(arr_val);
            } else if (dict_val) {
                var->type = TYPE_DICT;
                var->value.dictValue = dict_val;
                incref_dict(dict_val);
            } else if (str_val) {
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, str_val, 63);
            } else if (n->expr->type == NODE_NULL) {
                var->type = TYPE_NULL;
            } else if (is_bool) {
                var->type = TYPE_BOOL;
                var->value.intValue = (val != 0);
            } else {
                var->type = TYPE_DOUBLE;
                var->value.doubleValue = val;
            }
        } else {
            printf("Błąd: zmienna %s nie została zadeklarowana (użyj 'zmienna %s = ...')\n", n->var_name, n->var_name);
        }
        return 0;
    }
    else if (n->type == NODE_VAR_DECL) {
        double val = eval(n->expr);
        char *str_val = NULL;
        Array *arr_val = NULL;
        Dict *dict_val = NULL;
        struct Instance *inst_val = NULL;
        int is_bool = (n->expr->type == NODE_BOOL);
        
        if (return_instance) {
            inst_val = return_instance;
            return_instance = NULL;
        }
        
        if (n->expr->type == NODE_OPERATION) {
             int op = n->expr->op.op_type;
             if (op == TOKEN_EQ || op == TOKEN_NEQ || op == TOKEN_LT || op == TOKEN_GT || 
                 op == TOKEN_LTE || op == TOKEN_GTE || op == TOKEN_AND || op == TOKEN_OR) {
                 is_bool = 1;
             }
        }
        
        if (n->expr->type == NODE_ARRAY_LITERAL || n->expr->array_value) {
            arr_val = n->expr->array_value;
        } else if (n->expr->type == NODE_DICT_LITERAL || n->expr->dict_value) {
            dict_val = n->expr->dict_value;
        } else if (n->expr->string_value) {
            str_val = n->expr->string_value;
        }
        
        Variable *var = env_define(current_env, n->var_name);
        if (var) {
            if (inst_val) {
                var->type = TYPE_INSTANCE;
                var->value.instanceValue = inst_val;
            } else if (arr_val) {
                var->type = TYPE_ARRAY;
                var->value.arrayValue = arr_val;
                incref_array(arr_val);
            } else if (dict_val) {
                var->type = TYPE_DICT;
                var->value.dictValue = dict_val;
                incref_dict(dict_val);
            } else if (str_val) {
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, str_val, 63);
            } else if (n->expr->type == NODE_NULL) {
                var->type = TYPE_NULL;
            } else if (is_bool) {
                var->type = TYPE_BOOL;
                var->value.intValue = (val != 0);
            } else {
                var->type = TYPE_DOUBLE;
                var->value.doubleValue = val;
            }
        }
        return 0;
    }
    else if (n->type == NODE_PRINT) {
        // printf("DEBUG: NODE_PRINT\n");
        double val = eval(n->expr);
        
        // Check if expression is boolean
        int is_bool = (n->expr->type == NODE_BOOL);
        if (n->expr->type == NODE_OPERATION) {
             int op = n->expr->op.op_type;
             if (op == TOKEN_EQ || op == TOKEN_NEQ || op == TOKEN_LT || op == TOKEN_GT || 
                 op == TOKEN_LTE || op == TOKEN_GTE || op == TOKEN_AND || op == TOKEN_OR) {
                 is_bool = 1;
             }
        }
        if (n->expr->type == NODE_VARIABLE) {
            Variable *v = get_variable(n->expr->var_name);
            if (v && v->type == TYPE_BOOL) is_bool = 1;
        }

        if (n->expr->type == NODE_ARRAY_LITERAL || (n->expr->type == NODE_VARIABLE && get_variable(n->expr->var_name) && get_variable(n->expr->var_name)->type == TYPE_ARRAY)) {
            Array *arr = NULL;
            if (n->expr->type == NODE_ARRAY_LITERAL) arr = n->expr->array_value;
            else arr = get_variable(n->expr->var_name)->value.arrayValue;
            
            printf("[");
            for (int i=0; i<arr->count; i++) {
                if (i > 0) printf(", ");
                if (arr->elements[i].type == 1) printf("\"%s\"", arr->elements[i].value.stringValue);
                else printf("%g", arr->elements[i].value.doubleValue);
            }
            printf("]\n");
        }
        else if (n->expr->type == NODE_DICT_LITERAL || (n->expr->type == NODE_VARIABLE && get_variable(n->expr->var_name) && get_variable(n->expr->var_name)->type == TYPE_DICT)) {
            Dict *d = NULL;
            if (n->expr->type == NODE_DICT_LITERAL) d = n->expr->dict_value;
            else d = get_variable(n->expr->var_name)->value.dictValue;
            
            printf("{");
            for (int i=0; i<d->count; i++) {
                if (i > 0) printf(", ");
                printf("\"%s\": ", d->entries[i].key);
                if (d->entries[i].value.type == 1) printf("\"%s\"", d->entries[i].value.value.stringValue);
                else if (d->entries[i].value.type == 2) printf("[...]");
                else if (d->entries[i].value.type == 4) printf("{...}");
                else printf("%g", d->entries[i].value.value.doubleValue);
            }
            printf("}\n");
        }
        else if (n->expr->type == NODE_NULL || (n->expr->type == NODE_VARIABLE && get_variable(n->expr->var_name) && get_variable(n->expr->var_name)->type == TYPE_NULL)) {
            printf("nic\n");
        }
        else if (n->expr->string_value) {
            printf("%s\n", n->expr->string_value);
        } else if (is_bool) {
            printf("%s\n", (val != 0) ? "prawda" : "falsz");
        } else {
            printf("%lf\n", val);
        }
        return 0;
    }
    else if (n->type == NODE_IF) {
        double cond = eval(n->flow.cond);
        if (cond != 0) {
            eval(n->flow.body);
        } else if (n->flow.else_body) {
            eval(n->flow.else_body);
        }
        return 0;
    }
    else if (n->type == NODE_WHILE) {
        while (1) {
            // printf("DEBUG: Before cond, is_breaking=%d\n", is_breaking);
            // fflush(stdout);
            double c = eval(n->flow.cond);
            // printf("DEBUG: While cond: %e\n", c);
            // fflush(stdout);
            if (c == 0 || is_breaking) {
                // printf("DEBUG: Breaking loop c=%e B=%d\n", c, is_breaking);
                // fflush(stdout);
                if (is_breaking) is_breaking = 0;
                break;
            }

            if (is_breaking) {
                is_breaking = 0;
                break;
            }
            // if (is_continuing) {
            //     is_continuing = 0;
            //     continue;
            // }
            eval(n->flow.body);
            if (is_returning) break;
            if (is_continuing) {
                is_continuing = 0;
                continue;
            }
        }
        // printf("DEBUG: NODE_WHILE returning\n");
        // fflush(stdout);
        return 0;
    }
    else if (n->type == NODE_FUNC_DEF) {
        if (func_count >= func_capacity) {
            func_capacity = (func_capacity == 0) ? 32 : func_capacity * 2;
            functions = realloc(functions, sizeof(Function) * func_capacity);
        }
        Function f;
        strncpy(f.name, n->func.name, 63);
        f.body = n->func.body;
        f.args = n->func.args;
        f.arg_count = n->func.arg_count;
        functions[func_count++] = f;
        return 0;
    }
    else if (n->type == NODE_RETURN) {
        if (n->expr) {
            return_value = eval(n->expr);
            
            // Clear previous return objects
            if (return_string) { free(return_string); return_string = NULL; }
            if (return_array) { decref_array(return_array); return_array = NULL; }
            if (return_dict) { decref_dict(return_dict); return_dict = NULL; }

            if (n->expr->string_value) {
                return_string = malloc(strlen(n->expr->string_value)+1);
                strcpy(return_string, n->expr->string_value);
            } else if (n->expr->array_value) {
                return_array = n->expr->array_value;
                incref_array(return_array);
            } else if (n->expr->dict_value) {
                return_dict = n->expr->dict_value;
                incref_dict(return_dict);
            }
        }
        is_returning = 1;
        return return_value;
    }
    else if (n->type == NODE_FUNC_CALL) {
        // printf("DEBUG: Calling function %s\n", n->call.name);
        Function *f = get_function(n->call.name);
        if (!f) { printf("Błąd: nieznana funkcja %s\n", n->call.name); return 0; }
        
        double arg_vals[16];
        char *arg_strs[16] = {0};
        Array *arg_arrs[16] = {0};
        Dict *arg_dicts[16] = {0};
        
        for (int i=0; i<n->call.arg_count; i++) {
            arg_vals[i] = eval(n->call.args[i]);
            if (n->call.args[i]->string_value) {
                arg_strs[i] = malloc(strlen(n->call.args[i]->string_value)+1);
                strcpy(arg_strs[i], n->call.args[i]->string_value);
            } else if (n->call.args[i]->array_value) {
                arg_arrs[i] = n->call.args[i]->array_value;
                incref_array(arg_arrs[i]);
            } else if (n->call.args[i]->dict_value) {
                arg_dicts[i] = n->call.args[i]->dict_value;
                incref_dict(arg_dicts[i]);
            }
        }
        
        // Create new environment for function call
        Env *prev_env = current_env;
        current_env = create_env(global_env);
        
        for (int i=0; i<f->arg_count; i++) {
            if (i >= n->call.arg_count) break;
            
            Variable *var = env_define(current_env, f->args[i]);
            if (var) {
                if (arg_strs[i]) {
                    var->type = TYPE_STRING;
                    strncpy(var->value.stringValue, arg_strs[i], 63);
                } else if (arg_arrs[i]) {
                    var->type = TYPE_ARRAY;
                    var->value.arrayValue = arg_arrs[i];
                } else if (arg_dicts[i]) {
                    var->type = TYPE_DICT;
                    var->value.dictValue = arg_dicts[i];
                } else {
                    var->type = TYPE_DOUBLE;
                    var->value.doubleValue = arg_vals[i];
                }
            }
            if (arg_strs[i]) free(arg_strs[i]);
        }
        
        eval(f->body);
        
        // Reset control flow flags that shouldn't leak out of function
        if (is_breaking) { is_breaking = 0; }
        if (is_continuing) { is_continuing = 0; }
        // Exception should propagate out of function call
        
        // Restore environment
        Env *temp = current_env;
        current_env = prev_env;
        free_env(temp);

        double ret = return_value;
        
        // Clear node values
        if (n->string_value) { free(n->string_value); n->string_value = NULL; }
        n->array_value = NULL;
        n->dict_value = NULL;

        if (return_string) {
            n->string_value = malloc(strlen(return_string)+1);
            strcpy(n->string_value, return_string);
            free(return_string); return_string = NULL;
        } else if (return_array) {
            n->array_value = return_array;
            return_array = NULL; 
        } else if (return_dict) {
            n->dict_value = return_dict;
            return_dict = NULL;
        }
        
        is_returning = 0;
        return_value = 0;
        
        return ret;
    }

    if (n->type == NODE_NUMBER)
    {
        return n->value;
    }
    else if (n->type == NODE_INPUT)
    {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = 0;
            char *endptr;
            double val = strtod(buffer, &endptr);
            
            Variable *var = env_define(current_env, n->var_name);
            if (*endptr == '\0') {
                var->type = TYPE_DOUBLE;
                var->value.doubleValue = val;
            } else {
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, buffer, 63);
            }
        }
        return 0;
    }
    else if (n->type == NODE_OPERATION)
    {
        double a = eval(n->op.a);
        double b = eval(n->op.b);

        if (n->op.op_type == TOKEN_PLUS)
        {
            char *sA = get_node_string(n->op.a);
            char *sB = get_node_string(n->op.b);

            if (sA || sB)
            {
                char bufA[64], bufB[64];
                
                if (!sA) { snprintf(bufA, 64, "%g", a); sA = bufA; }
                if (!sB) { snprintf(bufB, 64, "%g", b); sB = bufB; }
                
                // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                if (n->string_value) free(n->string_value);
                n->string_value = malloc(strlen(sA) + strlen(sB) + 1);
                strcpy(n->string_value, sA);
                strcat(n->string_value, sB);
                return 0;
            }
        }
        
        if (n->op.op_type == TOKEN_EQ) {
            if (n->op.a->string_value && n->op.b->string_value) {
                return strcmp(n->op.a->string_value, n->op.b->string_value) == 0;
            }
        }
        if (n->op.op_type == TOKEN_NEQ) {
            if (n->op.a->string_value && n->op.b->string_value) {
                return strcmp(n->op.a->string_value, n->op.b->string_value) != 0;
            }
        }

        // Handle boolean logic explicitly to return 0/1
        if (n->op.op_type == TOKEN_AND) return (a != 0) && (b != 0);
        if (n->op.op_type == TOKEN_OR) return (a != 0) || (b != 0);
        if (n->op.op_type == TOKEN_EQ) {
            return a == b;
        }
        if (n->op.op_type == TOKEN_NEQ) return a != b;

        switch (n->op.op_type)
        {
        case TOKEN_PLUS: return a + b;
        case TOKEN_MINUS: return a - b;
        case TOKEN_MOD: return b != 0 ? fmod(a, b) : 0;
        case TOKEN_STAR: return a * b;
        case TOKEN_SLASH: return b != 0 ? a / b : 0;
        case TOKEN_LT:
            // printf("DEBUG: LT %f < %f = %d\n", a, b, a < b);
            return a < b;
        case TOKEN_GT: return a > b;
        case TOKEN_LTE: return a <= b;
        case TOKEN_GTE: return a >= b;
        }
    }
    else if (n->type == NODE_VARIABLE)
    {
        if (!n || !n->var_name) return 0;
        Variable *var = get_variable(n->var_name);
        if (var)
        {
            if (var->type == TYPE_INT) return var->value.intValue;
            if (var->type == TYPE_DOUBLE) return var->value.doubleValue;
            if (var->type == TYPE_STRING) {
                // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                if (n->string_value) free(n->string_value);
                n->string_value = malloc(strlen(var->value.stringValue) + 1);
                strcpy(n->string_value, var->value.stringValue);
                return 0;
            }
            if (var->type == TYPE_BOOL) {
                // n->type = NODE_BOOL; // DO NOT MUTATE AST TYPE
                return var->value.intValue;
            }
            if (var->type == TYPE_ARRAY) {
                n->array_value = var->value.arrayValue;
                return 0;
            }
            if (var->type == TYPE_DICT) {
                n->dict_value = var->value.dictValue;
                return 0;
            }
            if (var->type == TYPE_INSTANCE) {
                return_instance = var->value.instanceValue;
                return 0;
            }
            return 0;
        }
        printf("Nieznana zmienna: %s\n", n->var_name ? n->var_name : "(null)");
        return 0;
    }
    else if (n->type == NODE_METHOD_CALL) {
        // Evaluate object
        // We need to know what the object is.
        // If it's a variable, we get it.
        // If it's a literal, we evaluate it.
        
        // Handle built-in objects Matma and Plik
        if (n->method.obj->type == NODE_VARIABLE) {
            if (strcmp(n->method.obj->var_name, "Matma") == 0) {
                if (strcmp(n->method.name, "sin") == 0) {
                    if (n->method.arg_count >= 1) return sin(eval(n->method.args[0]));
                    return 0;
                }
                if (strcmp(n->method.name, "cos") == 0) {
                    if (n->method.arg_count >= 1) return cos(eval(n->method.args[0]));
                    return 0;
                }
                if (strcmp(n->method.name, "pierwiastek") == 0) {
                    if (n->method.arg_count >= 1) return sqrt(eval(n->method.args[0]));
                    return 0;
                }
                if (strcmp(n->method.name, "losuj") == 0) {
                    return (double)rand() / RAND_MAX;
                }
                if (strcmp(n->method.name, "czas") == 0) {
                    return (double)time(NULL);
                }
                if (strcmp(n->method.name, "potega") == 0) {
                    if (n->method.arg_count >= 2) {
                        double base = eval(n->method.args[0]);
                        double exp = eval(n->method.args[1]);
                        return pow(base, exp);
                    }
                    return 0;
                }
                return 0;
            }
            if (strcmp(n->method.obj->var_name, "Plik") == 0) {
                if (strcmp(n->method.name, "czytaj") == 0) {
                    if (n->method.arg_count >= 1) {
                        eval(n->method.args[0]);
                        char *path = n->method.args[0]->string_value;
                        if (path) {
                            FILE *f = fopen(path, "rb");
                            if (f) {
                                fseek(f, 0, SEEK_END);
                                long fsize = ftell(f);
                                fseek(f, 0, SEEK_SET);
                                char *string = malloc(fsize + 1);
                                if (string) {
                                    fread(string, 1, fsize, f);
                                    string[fsize] = 0;
                                }
                                fclose(f);
                                
                // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                if (n->string_value) free(n->string_value);
                n->string_value = string;
                                return 0;
                            } else {
                                printf("Błąd: nie można otworzyć pliku %s\n", path);
                            }
                        } else {
                             printf("Błąd: ścieżka musi być napisem\n");
                        }
                    }
                    return 0;
                }
                if (strcmp(n->method.name, "zapisz") == 0 || strcmp(n->method.name, "dopisz") == 0) {
                    if (n->method.arg_count >= 2) {
                        eval(n->method.args[0]);
                        eval(n->method.args[1]);
                        
                        char *path = n->method.args[0]->string_value;
                        char *content = n->method.args[1]->string_value;
                        char temp_buf[64];
                        
                        if (!content) {
                             sprintf(temp_buf, "%g", n->method.args[1]->value);
                             content = temp_buf;
                        }
                        
                        if (path) {
                            const char *mode = (strcmp(n->method.name, "zapisz") == 0) ? "w" : "a";
                            FILE *f = fopen(path, mode);
                            if (f) {
                                fprintf(f, "%s", content);
                                fclose(f);
                            } else {
                                printf("Błąd: nie można otworzyć pliku %s\n", path);
                            }
                        } else {
                             printf("Błąd: ścieżka musi być napisem\n");
                        }
                    }
                    return 0;
                }
                return 0;
            }
        }

        Variable *var = NULL;
        if (n->method.obj->type == NODE_VARIABLE) {
            var = get_variable(n->method.obj->var_name);
        }

        if (var && var->type == TYPE_INSTANCE) {
            Instance *inst = var->value.instanceValue;
            Class *cls = inst->cls;
            
            Function *method = NULL;
            Class *curr_cls = cls;
            while (curr_cls) {
                for (int i=0; i<curr_cls->method_count; i++) {
                    if (strcmp(curr_cls->methods[i].name, n->method.name) == 0) {
                        method = &curr_cls->methods[i];
                        break;
                    }
                }
                if (method) break;
                
                if (curr_cls->parent) {
                    Variable *v = env_get(global_env, curr_cls->parent);
                    if (v && v->type == TYPE_CLASS) {
                        curr_cls = v->value.classValue;
                    } else {
                        curr_cls = NULL;
                    }
                } else {
                    curr_cls = NULL;
                }
            }
            
            if (method) {
                // Pre-evaluate args
                double arg_vals[16];
                char *arg_strs[16] = {0};
                struct Instance *arg_insts[16] = {0};
                
                for (int i=0; i<n->method.arg_count && i < 16; i++) {
                    arg_vals[i] = eval(n->method.args[i]);
                    if (return_instance) {
                        arg_insts[i] = return_instance;
                        return_instance = NULL;
                    }
                    if (n->method.args[i]->string_value) {
                        arg_strs[i] = strdup(n->method.args[i]->string_value);
                    }
                }

                Env *prev_env = current_env;
                Instance *prev_inst = current_instance;
                
                current_env = create_env(global_env);
                current_instance = inst;
                
                for (int i=0; i<method->arg_count; i++) {
                    double val = 0;
                    char *str = NULL;
                    struct Instance *ins = NULL;
                    
                    if (i < n->method.arg_count) {
                        val = arg_vals[i];
                        str = arg_strs[i];
                        ins = arg_insts[i];
                    }
                    
                    Variable *arg = env_define(current_env, method->args[i]);
                    if (ins) {
                        arg->type = TYPE_INSTANCE;
                        arg->value.instanceValue = ins;
                    } else if (str) {
                        arg->type = TYPE_STRING;
                        strcpy(arg->value.stringValue, str);
                        free(str);
                    } else {
                        arg->type = TYPE_DOUBLE;
                        arg->value.doubleValue = val;
                    }
                }
                
                eval(method->body);
                
                // Cleanup
                for(int i=0; i<n->method.arg_count; i++) {
                    if (arg_strs[i]) free(arg_strs[i]);
                }

                current_env = prev_env;
                current_instance = prev_inst;
                
                if (is_returning) {
                    is_returning = 0;
                    return return_value;
                }
                return 0;
            } else {
                printf("Błąd: nieznana metoda %s\n", n->method.name);
                return 0;
            }
        }

        // String methods
        char *str_obj = NULL;
        if (var && var->type == TYPE_STRING) {
            str_obj = var->value.stringValue;
        } else if (n->method.obj->type == NODE_STRING) {
            str_obj = n->method.obj->string_value;
        } else if (n->method.obj->string_value) {
            str_obj = n->method.obj->string_value;
        }

        if (str_obj) {
            if (strcmp(n->method.name, "wGore") == 0) {
                // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                if (n->string_value) free(n->string_value);
                n->string_value = strdup(str_obj);
                for (int i = 0; n->string_value[i]; i++) {
                    n->string_value[i] = toupper((unsigned char)n->string_value[i]);
                }
                return 0;
            }
            if (strcmp(n->method.name, "wDol") == 0) {
                // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                if (n->string_value) free(n->string_value);
                n->string_value = strdup(str_obj);
                for (int i = 0; n->string_value[i]; i++) {
                    n->string_value[i] = tolower((unsigned char)n->string_value[i]);
                }
                return 0;
            }
            if (strcmp(n->method.name, "wytnij") == 0) {
                if (n->method.arg_count >= 2) {
                    int start = (int)eval(n->method.args[0]);
                    int len = (int)eval(n->method.args[1]);
                    int str_len = strlen(str_obj);
                    
                    if (start < 0) start = 0;
                    if (start > str_len) start = str_len;
                    if (len < 0) len = 0;
                    if (start + len > str_len) len = str_len - start;
                    
                    // n->type = NODE_STRING; // DO NOT MUTATE AST TYPE
                    if (n->string_value) free(n->string_value);
                    n->string_value = malloc(len + 1);
                    strncpy(n->string_value, str_obj + start, len);
                    n->string_value[len] = '\0';
                }
                return 0;
            }
            if (strcmp(n->method.name, "podziel") == 0) {
                char *delim = " ";
                if (n->method.arg_count >= 1) {
                    eval(n->method.args[0]);
                    if (n->method.args[0]->string_value) {
                        delim = n->method.args[0]->string_value;
                    }
                }
                
                Array *arr = create_array();
                char *temp = strdup(str_obj);
                char *token = strtok(temp, delim);
                while (token) {
                    array_push(arr, 0, token, NULL, NULL, NULL);
                    token = strtok(NULL, delim);
                }
                free(temp);
                
                // n->type = NODE_ARRAY_LITERAL; // DO NOT MUTATE AST TYPE
                n->array_value = arr;
                return 0;
            }
        }
        
        if (var && var->type == TYPE_ARRAY) {
            if (strcmp(n->method.name, "dodaj") == 0) {
                // printf("DEBUG: dodaj called\n");
                if (n->method.arg_count >= 1) {
                    double val = eval(n->method.args[0]);
                    if (n->method.args[0]->string_value) {
                        array_push(var->value.arrayValue, 0, n->method.args[0]->string_value, NULL, NULL, NULL);
                    } else if (n->method.args[0]->array_value) {
                        array_push(var->value.arrayValue, 0, NULL, n->method.args[0]->array_value, NULL, NULL);
                    } else if (n->method.args[0]->dict_value) {
                        array_push(var->value.arrayValue, 0, NULL, NULL, n->method.args[0]->dict_value, NULL);
                    } else {
                        array_push(var->value.arrayValue, val, NULL, NULL, NULL, NULL);
                    }
                }
                return 0;
            }
            if (strcmp(n->method.name, "usun") == 0) {
                // Pop or remove at index? Let's say usun(index)
                if (n->method.arg_count >= 1) {
                    int idx = (int)eval(n->method.args[0]);
                    Array *arr = var->value.arrayValue;
                    if (idx >= 0 && idx < arr->count) {
                        // Shift elements
                        // Free element if string
                        if (arr->elements[idx].type == 1 && arr->elements[idx].value.stringValue) {
                            free(arr->elements[idx].value.stringValue);
                        }
                        for (int i=idx; i<arr->count-1; i++) {
                            arr->elements[i] = arr->elements[i+1];
                        }
                        arr->count--;
                    }
                }
                return 0;
            }
        }
        return 0;
    }
    else if (n->type == NODE_MEMBER_ACCESS) {
        Node *obj = n->member.obj;
        if (obj) {
            // Handle static objects like Matma
            if (obj->type == NODE_VARIABLE && strcmp(obj->var_name, "Matma") == 0) {
                if (strcmp(n->member.name, "PI") == 0) {
                    return M_PI;
                }
            }

            if (obj->type != NODE_VARIABLE) {
                eval(obj);
            }
            
            Array *arr = NULL;
            Dict *dict = NULL;
            char *str = NULL;
            
            if (obj->type == NODE_VARIABLE) {
                Variable *var = get_variable(obj->var_name);
                if (var) {
                    if (var->type == TYPE_INSTANCE) {
                        Instance *inst = var->value.instanceValue;
                        ArrayElement *el = dict_get(inst->fields, n->member.name);
                        if (el) {
                            if (el->type == 0) return el->value.doubleValue;
                            if (el->type == 1) {
                                if (n->string_value) free(n->string_value);
                                n->string_value = strdup(el->value.stringValue);
                                return 0;
                            }
                            if (el->type == 2) {
                                n->array_value = el->value.arrayValue;
                                return 0;
                            }
                            if (el->type == 4) {
                                n->dict_value = el->value.dictValue;
                                return 0;
                            }
                            if (el->type == 6) {
                                return_instance = el->value.instanceValue;
                                return 0;
                            }
                        }
                        return 0;
                    } else if (var->type == TYPE_DICT) {
                        Dict *dict = var->value.dictValue;
                        ArrayElement *el = dict_get(dict, n->member.name);
                        if (el) {
                            if (el->type == 0) return el->value.doubleValue;
                            if (el->type == 1) {
                                if (n->string_value) free(n->string_value);
                                n->string_value = strdup(el->value.stringValue);
                                return 0;
                            }
                            if (el->type == 2) {
                                n->array_value = el->value.arrayValue;
                                return 0;
                            }
                            if (el->type == 4) {
                                n->dict_value = el->value.dictValue;
                                return 0;
                            }
                            if (el->type == 6) {
                                return_instance = el->value.instanceValue;
                                return 0;
                            }
                        }
                        return 0;
                    }
                }
            }
            
            if (return_instance) {
                Instance *inst = return_instance;
                return_instance = NULL;
                ArrayElement *el = dict_get(inst->fields, n->member.name);
                if (el) {
                    if (el->type == 0) return el->value.doubleValue;
                    if (el->type == 1) {
                        if (n->string_value) free(n->string_value);
                        n->string_value = strdup(el->value.stringValue);
                        return 0;
                    }
                    if (el->type == 2) {
                        n->array_value = el->value.arrayValue;
                        return 0;
                    }
                    if (el->type == 4) {
                        n->dict_value = el->value.dictValue;
                        return 0;
                    }
                    if (el->type == 6) {
                            return_instance = el->value.instanceValue;
                            return 0;
                    }
                }
                return 0;
            }

            // Handle dict literal or other expressions returning dict
            if (obj->dict_value) {
                Dict *dict = obj->dict_value;
                ArrayElement *el = dict_get(dict, n->member.name);
                if (el) {
                    if (el->type == 0) return el->value.doubleValue;
                    if (el->type == 1) {
                        if (n->string_value) free(n->string_value);
                        n->string_value = strdup(el->value.stringValue);
                        return 0;
                    }
                    if (el->type == 2) {
                        n->array_value = el->value.arrayValue;
                        return 0;
                    }
                    if (el->type == 4) {
                        n->dict_value = el->value.dictValue;
                        return 0;
                    }
                    if (el->type == 6) {
                        return_instance = el->value.instanceValue;
                        return 0;
                    }
                }
                return 0;
            }
            
            if (obj->type == NODE_THIS) {
                if (current_instance) {
                    Instance *inst = current_instance;
                    ArrayElement *el = dict_get(inst->fields, n->member.name);
                    if (el) {
                        printf("DEBUG: Reading member %s = %f\n", n->member.name, el->value.doubleValue);
                        if (el->type == 0) return el->value.doubleValue;
                        if (el->type == 1) {
                            if (n->string_value) free(n->string_value);
                            n->string_value = strdup(el->value.stringValue);
                            return 0;
                        }
                        if (el->type == 2) {
                            n->array_value = el->value.arrayValue;
                            return 0;
                        }
                        if (el->type == 4) {
                            n->dict_value = el->value.dictValue;
                            return 0;
                        }
                        if (el->type == 6) {
                             return_instance = el->value.instanceValue;
                             return 0;
                        }
                    }
                    return 0;
                }
            }

            if (obj->type == NODE_ARRAY_LITERAL || obj->array_value) {
                arr = obj->array_value;
            } else if (obj->type == NODE_DICT_LITERAL || obj->dict_value) {
                dict = obj->dict_value;
            } else if (obj->type == NODE_VARIABLE) {
                 arr = obj->array_value;
                 dict = obj->dict_value;
                 if (obj->string_value) str = obj->string_value;
            } else if (obj->string_value) {
                str = obj->string_value;
            }
            
            if (arr) {
                if (strcmp(n->member.name, "dlugosc") == 0) {
                    return (double)arr->count;
                }
            }
            
            if (str) {
                if (strcmp(n->member.name, "dlugosc") == 0) {
                    return (double)strlen(str);
                }
            }
            
            if (dict) {
                ArrayElement *el = dict_get(dict, n->member.name);
                if (el) {
                    if (el->type == 0) return el->value.doubleValue;
                    if (el->type == 1) {
                        if (n->string_value) free(n->string_value);
                        n->string_value = strdup(el->value.stringValue);
                        return 0;
                    }
                    if (el->type == 2) {
                        n->array_value = el->value.arrayValue;
                        return 0;
                    }
                    if (el->type == 3) return el->value.intValue;
                    if (el->type == 4) {
                        n->dict_value = el->value.dictValue;
                        return 0;
                    }
                }
                return 0;
            }
        }
        return 0;
    }
    else if (n->type == NODE_BREAK) {
        is_breaking = 1;
        return 0;
    }
    else if (n->type == NODE_CONTINUE) {
        is_continuing = 1;
        return 0;
    }
    else if (n->type == NODE_STRING)
    {
        return 0;
    }
    else if (n->type == NODE_BOOL)
    {
        return n->value;
    }
    return 0;
}

void free_node(Node *n) {
    // Simplified free to avoid double free issues with functions
    // In a real interpreter we would need reference counting or GC
    if (!n) return;
    // free(n); // Leaking memory intentionally for stability in this simple version
}

void parse()
{
    while (pos < token_count && tokens[pos].type != TOKEN_EOF)
    {
        // printf("DEBUG: Parsing token %d type %d text %s\n", pos, tokens[pos].type, tokens[pos].text);
        Node *stmt = parse_stmt();
        if (stmt)
        {
            eval(stmt);
        }
        else
        {
            // printf("DEBUG: Parse error at token %d\n", pos);
            pos++;
        }
    }
}

// Forward declaration for interpolation
void lex(const char *src);

int main(int argc, char *argv[])
{
    // printf("DEBUG: Main start\n");
    // fflush(stdout);
    if (argc < 2)
    {
        printf("Uzycie: mylang <plik>\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Nie można otworzyć pliku: %s\n", argv[1]);
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *src = malloc(fsize + 1);
    fread(src, 1, fsize, file);
    src[fsize] = 0;
    fclose(file);

    lex(src);

    // printf("DEBUG: File size: %ld\n", fsize);
    // printf("DEBUG: Token count: %d\n", token_count);
    // fflush(stdout);
    global_env = create_env(NULL);
    current_env = global_env;

    // Define global constants
    Variable *pi_var = env_define(global_env, "pi");
    if (pi_var) {
        pi_var->type = TYPE_DOUBLE;
        pi_var->value.doubleValue = 3.14159265358979323846;
    }

    // for (int i = 0; i < token_count; i++)
    // {
    //     printf("Token %d: typ=%d, tekst='%s'\n", i, tokens[i].type, tokens[i].text);
    // }

    parse();
    
    // Print global variables only if not returning from main (which shouldn't happen)
    // Actually, let's comment this out to avoid cluttering output of scripts
    /*
    for (int i = 0; i < global_env->var_count; i++)
    {
        printf("Zmienna %s = ", global_env->variables[i].name);
        // ...
    }
    */
    return 0;
}