#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
    NODE_VAR_DECL
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
    TOKEN_EOF       // koniec pliku
} TokenType;

typedef struct
{
    TokenType type;
    char text[64];
} Token;

#define MAX_TOKENS 256
Token tokens[MAX_TOKENS];
int token_count = 0;

#define MAX_VARS 32

typedef enum
{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_BOOL,
    TYPE_DICT
} VarType;

typedef struct {
    int type; // 0=double, 1=string, 2=array, 3=bool, 4=dict
    union {
        double doubleValue;
        char *stringValue;
        struct Array *arrayValue;
        struct Dict *dictValue;
        int intValue; // for bool
    } value;
} ArrayElement; // Reusing for Dict values too

typedef struct Array {
    ArrayElement *elements;
    int count;
    int capacity;
} Array;

typedef struct {
    char *key;
    ArrayElement value;
} DictEntry;

typedef struct Dict {
    DictEntry *entries;
    int count;
    int capacity;
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
    } value;
} Variable;

typedef struct Env {
    Variable variables[MAX_VARS];
    int var_count;
    struct Env *parent;
} Env;

Env *global_env = NULL;
Env *current_env = NULL;

Env *create_env(Env *parent) {
    Env *env = calloc(1, sizeof(Env));
    env->parent = parent;
    env->var_count = 0;
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
    // Check if already defined in CURRENT scope (shadowing allowed, redefinition in same scope maybe not?)
    // Let's allow redefinition for simplicity or check?
    // Usually redefinition in same scope is error or update.
    // Let's check if exists in current scope.
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i]; // Return existing to update
        }
    }
    if (env->var_count >= MAX_VARS) {
        printf("Błąd: zbyt wiele zmiennych w zasięgu\n");
        return NULL;
    }
    Variable *var = &env->variables[env->var_count++];
    strncpy(var->name, name, 63);
    return var;
}

typedef struct {
    char name[64];
    Node *body;
    char **args;
    int arg_count;
} Function;

Function functions[32];
int func_count = 0;

Function *get_function(const char *name) {
    for (int i=0; i<func_count; i++) {
        if (strcmp(functions[i].name, name) == 0) return &functions[i];
    }
    return NULL;
}

// Array helpers
Array *create_array() {
    Array *arr = malloc(sizeof(Array));
    arr->count = 0;
    arr->capacity = 8;
    arr->elements = malloc(sizeof(ArrayElement) * arr->capacity);
    return arr;
}

void array_push(Array *arr, double val, char *str, Array *subArr, Dict *subDict) {
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
    } else if (subDict) {
        arr->elements[arr->count].type = 4;
        arr->elements[arr->count].value.dictValue = subDict;
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
    d->entries = malloc(sizeof(DictEntry) * d->capacity);
    return d;
}

void dict_set(Dict *d, const char *key, double val, char *str, Array *arr, Dict *subDict) {
    // Check if key exists
    for (int i=0; i<d->count; i++) {
        if (strcmp(d->entries[i].key, key) == 0) {
            // Update
            if (d->entries[i].value.type == 1 && d->entries[i].value.value.stringValue) {
                free(d->entries[i].value.value.stringValue);
            }
            if (str) {
                d->entries[i].value.type = 1;
                d->entries[i].value.value.stringValue = strdup(str);
            } else if (arr) {
                d->entries[i].value.type = 2;
                d->entries[i].value.value.arrayValue = arr;
            } else if (subDict) {
                d->entries[i].value.type = 4;
                d->entries[i].value.value.dictValue = subDict;
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
    } else if (subDict) {
        d->entries[d->count].value.type = 4;
        d->entries[d->count].value.value.dictValue = subDict;
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
int is_returning = 0;

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
    if (t.type == TOKEN_STAR || t.type == TOKEN_SLASH) return 20;
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
        left = make_string(tokens[pos].text);
        pos++;
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
    else if (tokens[pos].type == TOKEN_KEYWORD && strcmp(tokens[pos].text, "podaj") == 0)
    {
        pos++;
        left = malloc(sizeof(Node));
        left->type = NODE_INPUT;
        left->value = 0;
        left->string_value = NULL;
        left->var_name = NULL;
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
    if (tokens[pos].type == TOKEN_LBRACE) {
        return parse_block();
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
                }
                
                node->func.body = parse_stmt(); // blok
                return node;
            }
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
                 tokens[pos+1].type == TOKEN_MUL_ASSIGN || tokens[pos+1].type == TOKEN_DIV_ASSIGN) {
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
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        return expr;
    }
    
    return NULL;
}

void add_token(TokenType type, const char *text)
{
    if (token_count < MAX_TOKENS)
    {
        tokens[token_count].type = type;
        strncpy(tokens[token_count].text, text, 63);
        tokens[token_count].text[63] = '\0';
        token_count++;
    }
}

// Tablica słów kluczowych
const char *keywords[] = {
    "pisz",
    "podaj",
    "usun",
    "jezeli",
    "inaczej",
    "dla",
    "funkcja",
    "zwroc",
    "zmienna",
    "wkolko",
    "prawda",
    "falsz",
    "oraz",
    "albo",
    "rowne",
    "nierowne",
    "dla"};
const int keywords_count = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(const char *text)
{
    for (int i = 0; i < keywords_count; i++)
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

        if (src[i] == '"')
        {        // string w cudzysłowie
            i++; // pomiń otwierający cudzysłów
            int start = i;
            while (src[i] != '"' && src[i] != '\0')
                i++;
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            add_token(TOKEN_STRING, buf);
            if (src[i] == '"')
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
            if (is_keyword(buf))
            {
                if (strcmp(buf, "prawda") == 0) add_token(TOKEN_TRUE, buf);
                else if (strcmp(buf, "falsz") == 0) add_token(TOKEN_FALSE, buf);
                else if (strcmp(buf, "oraz") == 0) add_token(TOKEN_AND, buf);
                else if (strcmp(buf, "albo") == 0) add_token(TOKEN_OR, buf);
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
            c == '[' || c == ']' || c == '.' || c == ':')
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
}

double eval(Node *n)
{
    if (!n) return 0;
    if (is_returning) return 0;

    if (n->type == NODE_BLOCK) {
        current_env = create_env(current_env); // Push scope
        for (int i=0; i<n->block.count; i++) {
            eval(n->block.stmts[i]);
            if (is_returning) break;
        }
        Env *parent = current_env->parent;
        free(current_env); // Simple free, ideally free vars too
        current_env = parent; // Pop scope
        return 0;
    }
    else if (n->type == NODE_ARRAY_LITERAL) {
        if (n->array_value) return 0;
        Array *arr = create_array();
        for (int i=0; i<n->block.count; i++) {
            double val = eval(n->block.stmts[i]);
            if (n->block.stmts[i]->string_value) {
                array_push(arr, 0, n->block.stmts[i]->string_value, NULL, NULL);
            } else if (n->block.stmts[i]->array_value) {
                array_push(arr, 0, NULL, n->block.stmts[i]->array_value, NULL);
            } else if (n->block.stmts[i]->dict_value) {
                array_push(arr, 0, NULL, NULL, n->block.stmts[i]->dict_value);
            } else {
                array_push(arr, val, NULL, NULL, NULL);
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
                dict_set(d, key, 0, valNode->string_value, NULL, NULL);
            } else if (valNode->array_value) {
                dict_set(d, key, 0, NULL, valNode->array_value, NULL);
            } else if (valNode->dict_value) {
                dict_set(d, key, 0, NULL, NULL, valNode->dict_value);
            } else {
                dict_set(d, key, val, NULL, NULL, NULL);
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
                char *key = NULL;
                if (n->array_op.index->string_value) {
                    key = n->array_op.index->string_value;
                } else {
                    printf("Błąd: klucz słownika musi być napisem\n");
                    return 0;
                }
                
                ArrayElement *el = dict_get(dict, key);
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
            
            if (arr) {
                double idx_d = eval(n->array_op.index);
                int idx = (int)idx_d;
                if (idx < 0 || idx >= arr->count) {
                    printf("Błąd: indeks %d poza zakresem (rozmiar %d)\n", idx, arr->count);
                    return 0;
                }
                if (arr->elements[idx].type == 0) return arr->elements[idx].value.doubleValue;
                if (arr->elements[idx].type == 1) {
                    if (n->string_value) free(n->string_value);
                    n->string_value = strdup(arr->elements[idx].value.stringValue);
                    return 0;
                }
                if (arr->elements[idx].type == 2) {
                    n->array_value = arr->elements[idx].value.arrayValue;
                    return 0;
                }
                if (arr->elements[idx].type == 4) {
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
        Variable *var = get_variable(n->array_op.name);
        if (!var) { printf("Błąd: nieznana zmienna %s\n", n->array_op.name); return 0; }
        
        if (var->type == TYPE_DICT) {
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
                dict_set(var->value.dictValue, key, 0, NULL, n->array_op.value->array_value, NULL);
            } else if (n->array_op.value->type == NODE_DICT_LITERAL) {
                dict_set(var->value.dictValue, key, 0, NULL, NULL, n->array_op.value->dict_value);
            } else if (n->array_op.value->string_value) {
                dict_set(var->value.dictValue, key, 0, n->array_op.value->string_value, NULL, NULL);
            } else {
                dict_set(var->value.dictValue, key, val, NULL, NULL, NULL);
            }
            return 0;
        }
        
        if (var->type != TYPE_ARRAY) {
            printf("Błąd: zmienna %s nie jest tablicą ani słownikiem\n", n->array_op.name);
            return 0;
        }
        double idx_d = eval(n->array_op.index);
        int idx = (int)idx_d;
        double val = eval(n->array_op.value);
        
        if (n->array_op.value->string_value) {
            array_set(var->value.arrayValue, idx, 0, n->array_op.value->string_value);
        } else {
            array_set(var->value.arrayValue, idx, val, NULL);
        }
        return 0;
    }
    else if (n->type == NODE_ASSIGN) {
        double val = eval(n->expr);
        char *str_val = NULL;
        Array *arr_val = NULL;
        Dict *dict_val = NULL;
        int is_bool = (n->expr->type == NODE_BOOL);
        
        // Check if expression resulted in boolean (e.g. comparison)
        if (n->expr->type == NODE_OPERATION) {
             int op = n->expr->op.op_type;
             if (op == TOKEN_EQ || op == TOKEN_NEQ || op == TOKEN_LT || op == TOKEN_GT || 
                 op == TOKEN_LTE || op == TOKEN_GTE || op == TOKEN_AND || op == TOKEN_OR) {
                 is_bool = 1;
             }
        }
        
        if (n->expr->type == NODE_ARRAY_LITERAL) {
            arr_val = n->expr->array_value;
        } else if (n->expr->type == NODE_DICT_LITERAL) {
            dict_val = n->expr->dict_value;
        } else if (n->expr->string_value) {
            str_val = n->expr->string_value;
        }
        
        Variable *var = get_variable(n->var_name);
        if (var) {
            if (arr_val) {
                var->type = TYPE_ARRAY;
                var->value.arrayValue = arr_val;
            } else if (dict_val) {
                var->type = TYPE_DICT;
                var->value.dictValue = dict_val;
            } else if (str_val) {
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, str_val, 63);
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
        int is_bool = (n->expr->type == NODE_BOOL);
        
        if (n->expr->type == NODE_OPERATION) {
             int op = n->expr->op.op_type;
             if (op == TOKEN_EQ || op == TOKEN_NEQ || op == TOKEN_LT || op == TOKEN_GT || 
                 op == TOKEN_LTE || op == TOKEN_GTE || op == TOKEN_AND || op == TOKEN_OR) {
                 is_bool = 1;
             }
        }
        
        if (n->expr->type == NODE_ARRAY_LITERAL) {
            arr_val = n->expr->array_value;
        } else if (n->expr->type == NODE_DICT_LITERAL) {
            dict_val = n->expr->dict_value;
        } else if (n->expr->string_value) {
            str_val = n->expr->string_value;
        }
        
        Variable *var = env_define(current_env, n->var_name);
        if (var) {
            if (arr_val) {
                var->type = TYPE_ARRAY;
                var->value.arrayValue = arr_val;
            } else if (dict_val) {
                var->type = TYPE_DICT;
                var->value.dictValue = dict_val;
            } else if (str_val) {
                var->type = TYPE_STRING;
                strncpy(var->value.stringValue, str_val, 63);
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
        while (eval(n->flow.cond) != 0) {
            eval(n->flow.body);
            if (is_returning) break;
        }
        return 0;
    }
    else if (n->type == NODE_FUNC_DEF) {
        if (func_count < 32) {
            Function f;
            strncpy(f.name, n->func.name, 63);
            f.body = n->func.body;
            f.args = n->func.args;
            f.arg_count = n->func.arg_count;
            functions[func_count++] = f;
        }
        return 0;
    }
    else if (n->type == NODE_RETURN) {
        if (n->expr) {
            return_value = eval(n->expr);
            if (n->expr->string_value) {
                if (return_string) free(return_string);
                return_string = malloc(strlen(n->expr->string_value)+1);
                strcpy(return_string, n->expr->string_value);
            } else {
                if (return_string) { free(return_string); return_string = NULL; }
            }
        }
        is_returning = 1;
        return return_value;
    }
    else if (n->type == NODE_FUNC_CALL) {
        Function *f = get_function(n->call.name);
        if (!f) { printf("Błąd: nieznana funkcja %s\n", n->call.name); return 0; }
        
        double arg_vals[16];
        char *arg_strs[16] = {0};
        
        for (int i=0; i<n->call.arg_count; i++) {
            arg_vals[i] = eval(n->call.args[i]);
            if (n->call.args[i]->string_value) {
                arg_strs[i] = malloc(strlen(n->call.args[i]->string_value)+1);
                strcpy(arg_strs[i], n->call.args[i]->string_value);
            }
        }
        
        // Create new environment for function call
        // Parent is global_env (static scoping for top-level functions)
        // If we supported nested functions, we'd need closure env.
        Env *prev_env = current_env;
        current_env = create_env(global_env);
        
        for (int i=0; i<f->arg_count; i++) {
            if (i >= n->call.arg_count) break;
            
            Variable *var = env_define(current_env, f->args[i]);
            if (var) {
                if (arg_strs[i]) {
                    var->type = TYPE_STRING;
                    strncpy(var->value.stringValue, arg_strs[i], 63);
                } else {
                    var->type = TYPE_DOUBLE;
                    var->value.doubleValue = arg_vals[i];
                }
            }
            if (arg_strs[i]) free(arg_strs[i]);
        }
        
        eval(f->body);
        
        // Restore environment
        Env *temp = current_env;
        current_env = prev_env;
        free(temp); // Should free vars inside too

        
        double ret = return_value;
        if (return_string) {
            // n->type = NODE_STRING; // NIE ZMIENIAJ TYPU!
            if (n->string_value) free(n->string_value);
            n->string_value = malloc(strlen(return_string)+1);
            strcpy(n->string_value, return_string);
        } else {
            if (n->string_value) { free(n->string_value); n->string_value = NULL; }
        }
        
        is_returning = 0;
        return_value = 0;
        if (return_string) { free(return_string); return_string = NULL; }
        
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
            if (*endptr == '\0') {
                return val;
            } else {
                n->type = NODE_STRING;
                n->string_value = malloc(strlen(buffer) + 1);
                strcpy(n->string_value, buffer);
                return 0;
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
            if (n->op.a->string_value || n->op.b->string_value)
            {
                char bufA[64], bufB[64];
                const char *strA = n->op.a->string_value;
                const char *strB = n->op.b->string_value;
                
                if (!strA) { snprintf(bufA, 64, "%g", a); strA = bufA; }
                if (!strB) { snprintf(bufB, 64, "%g", b); strB = bufB; }
                
                n->type = NODE_STRING;
                if (n->string_value) free(n->string_value);
                n->string_value = malloc(strlen(strA) + strlen(strB) + 1);
                strcpy(n->string_value, strA);
                strcat(n->string_value, strB);
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
        if (n->op.op_type == TOKEN_EQ) return a == b;
        if (n->op.op_type == TOKEN_NEQ) return a != b;

        switch (n->op.op_type)
        {
        case TOKEN_PLUS: return a + b;
        case TOKEN_MINUS: return a - b;
        case TOKEN_STAR: return a * b;
        case TOKEN_SLASH: return b != 0 ? a / b : 0;
        case TOKEN_LT: return a < b;
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
                n->type = NODE_STRING;
                if (n->string_value) free(n->string_value);
                n->string_value = malloc(strlen(var->value.stringValue) + 1);
                strcpy(n->string_value, var->value.stringValue);
                return 0;
            }
            if (var->type == TYPE_BOOL) {
                n->type = NODE_BOOL;
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
                                
                                n->type = NODE_STRING;
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
        
        if (var && var->type == TYPE_ARRAY) {
            if (strcmp(n->method.name, "dodaj") == 0) {
                if (n->method.arg_count >= 1) {
                    double val = eval(n->method.args[0]);
                    if (n->method.args[0]->string_value) {
                        array_push(var->value.arrayValue, 0, n->method.args[0]->string_value, NULL, NULL);
                    } else if (n->method.args[0]->array_value) {
                        array_push(var->value.arrayValue, 0, NULL, n->method.args[0]->array_value, NULL);
                    } else if (n->method.args[0]->dict_value) {
                        array_push(var->value.arrayValue, 0, NULL, NULL, n->method.args[0]->dict_value);
                    } else {
                        array_push(var->value.arrayValue, val, NULL, NULL, NULL);
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

            eval(obj);
            
            Array *arr = NULL;
            Dict *dict = NULL;
            char *str = NULL;
            
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
        Node *stmt = parse_stmt();
        if (stmt)
        {
            eval(stmt);
        }
        else
        {
            pos++;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Użycie: %s <plik.mylang>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        printf("Nie można otworzyć pliku: %s\n", argv[1]);
        return 1;
    }

    char src[1024] = {0};
    fread(src, 1, sizeof(src) - 1, file);
    fclose(file);

    lex(src);

    // Initialize global environment
    global_env = create_env(NULL);
    current_env = global_env;

    // for (int i = 0; i < token_count; i++)
    // {
    //     printf("Token %d: typ=%d, tekst='%s'\n", i, tokens[i].type, tokens[i].text);
    // }

    parse();
    for (int i = 0; i < global_env->var_count; i++)
    {
        printf("Zmienna %s = ", global_env->variables[i].name);
        if (global_env->variables[i].type == TYPE_INT)
            printf("%d\n", global_env->variables[i].value.intValue);
        else if (global_env->variables[i].type == TYPE_DOUBLE)
            printf("%lf\n", global_env->variables[i].value.doubleValue);
        else if (global_env->variables[i].type == TYPE_STRING)
            printf("'%s'\n", global_env->variables[i].value.stringValue);
        else if (global_env->variables[i].type == TYPE_BOOL)
            printf("%s\n", global_env->variables[i].value.intValue ? "prawda" : "falsz");
        else if (global_env->variables[i].type == TYPE_DICT) {
            printf("{");
            Dict *d = global_env->variables[i].value.dictValue;
            for (int j=0; j<d->count; j++) {
                if (j > 0) printf(", ");
                printf("\"%s\": ", d->entries[j].key);
                if (d->entries[j].value.type == 1) printf("\"%s\"", d->entries[j].value.value.stringValue);
                else if (d->entries[j].value.type == 2) printf("[...]");
                else if (d->entries[j].value.type == 4) printf("{...}");
                else printf("%g", d->entries[j].value.value.doubleValue);
            }
            printf("}\n");
        }
        else if (global_env->variables[i].type == TYPE_ARRAY) {
            printf("[");
            Array *arr = global_env->variables[i].value.arrayValue;
            for (int j=0; j<arr->count; j++) {
                if (j > 0) printf(", ");
                if (arr->elements[j].type == 1) printf("\"%s\"", arr->elements[j].value.stringValue);
                else if (arr->elements[j].type == 2) printf("[...]");
                else if (arr->elements[j].type == 4) printf("{...}");
                else printf("%g", arr->elements[j].value.doubleValue);
            }
            printf("]\n");
        }
        else
            printf("Nieznany typ\n");
    }
    return 0;
}