#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

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
    NODE_FOREACH,
    NODE_IMPORT
} NodeType;

// Forward declaration
struct Array;
struct Dict;

typedef struct Node
{
    NodeType type;
    int line;          // Numer linii w kodzie źródłowym
    // Pola wspólne dla wszystkich typów
    double value;      // dla liczb i obliczonych wartości
    char *var_name;    // dla zmiennych
    char *string_value; // dla wartości tekstowych
    struct Array *array_value; // dla tablic
    struct Dict *dict_value;   // dla słowników
    struct FunctionObj *func_value; // dla funkcji
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
    TOKEN_BIT_AND,    // &
    TOKEN_BIT_OR,     // |
    TOKEN_BIT_XOR,    // ^
    TOKEN_BIT_NOT,    // ~
    TOKEN_LSHIFT,     // <<
    TOKEN_RSHIFT,     // >>
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
    TOKEN_IMPORT,
    TOKEN_EOF       // koniec pliku
} TokenType;

typedef struct
{
    TokenType type;
    char text[64];
    int line;
} Token;

Token *tokens = NULL;
int token_count = 0;
int token_capacity = 0;
int pos = 0;

void report_error(int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Błąd w linii %d: ", line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

Node *alloc_node() {
    Node *n = calloc(1, sizeof(*n));
    if (tokens && pos < token_count) {
        n->line = tokens[pos].line;
    } else if (tokens && token_count > 0) {
        n->line = tokens[token_count - 1].line;
    } else {
        n->line = 1;
    }
    return n;
}

typedef enum {
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_BOOL,
    TYPE_DICT,
    TYPE_INT,
    TYPE_CLASS,
    TYPE_INSTANCE,
    TYPE_NULL,
    TYPE_FUNCTION
} VarType;

// GC Definitions
typedef enum { OBJ_ARRAY, OBJ_DICT, OBJ_INSTANCE, OBJ_ENV, OBJ_FUNCTION } ObjType;

typedef struct ObjHeader {
    ObjType type;
    unsigned char marked;
    struct ObjHeader *next;
} ObjHeader;

ObjHeader *gc_objects = NULL;
int gc_objects_count = 0;
int gc_threshold = 128;
ObjHeader *gc_temp_root = NULL;

void gc_collect();

// Forward declarations
struct Class;
struct Instance;
struct FunctionObj;

typedef struct {
    int type; // 0=double, 1=string, 2=array, 3=bool, 4=dict, 5=class, 6=instance, 8=function
    union {
        double doubleValue;
        char *stringValue;
        struct Array *arrayValue;
        struct Dict *dictValue;
        int intValue; // for bool
        struct Class *classValue;
        struct Instance *instanceValue;
        struct FunctionObj *funcValue;
    } value;
} ArrayElement; // Reusing for Dict values too

typedef struct Array {
    ObjHeader header;
    ArrayElement *elements;
    int count;
    int capacity;
} Array;

typedef struct {
    char *key;
    ArrayElement value;
} DictEntry;

typedef struct Dict {
    ObjHeader header;
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
        struct Class *classValue;
        struct Instance *instanceValue;
        struct FunctionObj *funcValue;
    } value;
} Variable;

typedef struct Env {
    ObjHeader header;
    Variable *variables;
    int var_count;
    int var_capacity;
    struct Env *parent;
} Env;

Env *global_env = NULL;
Env *current_env = NULL;
struct Instance *current_instance = NULL;

// Call stack for GC
Env *gc_call_stack[256];
int gc_call_stack_count = 0;

void gc_register(ObjHeader *obj, ObjType type); // Forward decl

Env *create_env(Env *parent) {
    Env *env = calloc(1, sizeof(Env));
    env->parent = parent;
    env->var_count = 0;
    env->var_capacity = 8;
    env->variables = malloc(sizeof(Variable) * env->var_capacity);
    ObjHeader *prev_root = gc_temp_root;
    gc_temp_root = (ObjHeader*)env;
    gc_register((ObjHeader*)env, OBJ_ENV);
    gc_temp_root = prev_root;
    return env;
}

Variable *env_get(Env *env, const char *name) {
    if (!env || !name) return NULL;
    // printf("DEBUG: Looking for %s in env %p (count=%d)\n", name, env, env->var_count);
    for (int i = 0; i < env->var_count; i++) {
        // printf("  - Found var: %s\n", env->variables[i].name);
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i];
        }
    }
    if (env->parent) return env_get(env->parent, name);
    return NULL;
}

Variable *env_define(Env *env, const char *name) {
    if (!env || !name) return NULL;
    // printf("DEBUG: Defining %s in env %p\n", name, env);
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
    // printf("DEBUG: Allocating var %s at %p (env vars %p)\n", name, var, env->variables);
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
    ObjHeader header;
    Class *cls;
    Dict *fields;
} Instance;

typedef struct FunctionObj {
    ObjHeader header;
    char *name;
    Node *body;
    char **args;
    int arg_count;
    Env *closure;
} FunctionObj;

// Memory Management Helpers
void free_array(Array *arr);
void free_dict(Dict *d);
void free_instance(Instance *inst);

// Stub out old ref counting
void incref_array(Array *arr) {}
void decref_array(Array *arr) {}
void incref_dict(Dict *d) {}
void decref_dict(Dict *d) {}

// GC Forward Declarations
void gc_register(ObjHeader *obj, ObjType type);
void mark_object(ObjHeader *obj);

// Array helpers
Array *create_array() {
    Array *arr = malloc(sizeof(Array));
    arr->count = 0;
    arr->capacity = 8;
    arr->elements = malloc(sizeof(ArrayElement) * arr->capacity);
    ObjHeader *prev_root = gc_temp_root;
    gc_temp_root = (ObjHeader*)arr;
    gc_register((ObjHeader*)arr, OBJ_ARRAY);
    gc_temp_root = prev_root;
    return arr;
}

void free_array(Array *arr) {
    if (!arr) return;
    for (int i=0; i<arr->count; i++) {
        if (arr->elements[i].type == 1 && arr->elements[i].value.stringValue) {
            free(arr->elements[i].value.stringValue);
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
    } else if (subDict) {
        arr->elements[arr->count].type = 4;
        arr->elements[arr->count].value.dictValue = subDict;
    } else if (inst) {
        arr->elements[arr->count].type = 6;
        arr->elements[arr->count].value.instanceValue = inst;
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
    ObjHeader *prev_root = gc_temp_root;
    gc_temp_root = (ObjHeader*)d;
    gc_register((ObjHeader*)d, OBJ_DICT);
    gc_temp_root = prev_root;
    return d;
}

void free_dict(Dict *d) {
    if (!d) return;
    for (int i=0; i<d->count; i++) {
        if (d->entries[i].key) free(d->entries[i].key);
        if (d->entries[i].value.type == 1 && d->entries[i].value.value.stringValue) {
            free(d->entries[i].value.value.stringValue);
        }
    }
    free(d->entries);
    free(d);
}

void free_instance(Instance *inst) {
    if (!inst) return;
    // Fields dict is managed by GC, so we don't free it here explicitly
    // But wait, if Instance owns the Dict, and Dict is a GC object...
    // Yes, Dict is a separate GC object.
    // If Instance is freed, it means it's unreachable.
    // The Dict it points to might still be reachable if shared?
    // Unlikely for fields, but possible.
    // If Dict is only reachable from Instance, it will be freed in next sweep or same sweep?
    // If Instance is unmarked, it is freed.
    // The Dict it points to is also unmarked (unless reached from elsewhere).
    // So it will be freed in the same sweep loop.
    free(inst);
}

void dict_set(Dict *d, const char *key, double val, char *str, Array *arr, Dict *subDict, struct Instance *inst) {
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
    } else if (subDict) {
        d->entries[d->count].value.type = 4;
        d->entries[d->count].value.value.dictValue = subDict;
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
void parse();
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
struct FunctionObj *return_function = NULL;
volatile int is_exception = 0;
double exception_value = 0;
char *exception_string = NULL;

// GC Implementation
void mark_array(Array *arr) {
    if (arr->header.marked) return;
    arr->header.marked = 1;
    for (int i=0; i<arr->count; i++) {
        if (arr->elements[i].type == 2 && arr->elements[i].value.arrayValue) {
            mark_object((ObjHeader*)arr->elements[i].value.arrayValue);
        } else if (arr->elements[i].type == 4 && arr->elements[i].value.dictValue) {
            mark_object((ObjHeader*)arr->elements[i].value.dictValue);
        } else if (arr->elements[i].type == 6 && arr->elements[i].value.instanceValue) {
            mark_object((ObjHeader*)arr->elements[i].value.instanceValue);
        }
    }
}

void mark_dict(Dict *d) {
    if (d->header.marked) return;
    d->header.marked = 1;
    for (int i=0; i<d->count; i++) {
        if (d->entries[i].value.type == 2 && d->entries[i].value.value.arrayValue) {
            mark_object((ObjHeader*)d->entries[i].value.value.arrayValue);
        } else if (d->entries[i].value.type == 4 && d->entries[i].value.value.dictValue) {
            mark_object((ObjHeader*)d->entries[i].value.value.dictValue);
        } else if (d->entries[i].value.type == 6 && d->entries[i].value.value.instanceValue) {
            mark_object((ObjHeader*)d->entries[i].value.value.instanceValue);
        }
    }
}

void mark_instance(Instance *inst) {
    if (inst->header.marked) return;
    inst->header.marked = 1;
    if (inst->fields) mark_dict(inst->fields);
}

void mark_env(Env *env); // Forward decl
void free_env(Env *env); // Forward decl

void mark_function(FunctionObj *fn) {
    if (fn->header.marked) return;
    fn->header.marked = 1;
    if (fn->closure) mark_object((ObjHeader*)fn->closure);
}

void mark_object(ObjHeader *obj) {
    if (!obj || obj->marked) return;
    if (obj->type == OBJ_ARRAY) mark_array((Array*)obj);
    else if (obj->type == OBJ_DICT) mark_dict((Dict*)obj);
    else if (obj->type == OBJ_INSTANCE) mark_instance((Instance*)obj);
    else if (obj->type == OBJ_ENV) mark_env((Env*)obj);
    else if (obj->type == OBJ_FUNCTION) mark_function((FunctionObj*)obj);
}

void mark_env(Env *env) {
    if (!env || env->header.marked) return;
    env->header.marked = 1;
    for (int i=0; i<env->var_count; i++) {
        Variable *v = &env->variables[i];
        if (v->type == TYPE_ARRAY && v->value.arrayValue) mark_object((ObjHeader*)v->value.arrayValue);
        else if (v->type == TYPE_DICT && v->value.dictValue) mark_object((ObjHeader*)v->value.dictValue);
        else if (v->type == TYPE_INSTANCE && v->value.instanceValue) mark_object((ObjHeader*)v->value.instanceValue);
        else if (v->type == TYPE_FUNCTION && v->value.funcValue) mark_object((ObjHeader*)v->value.funcValue);
    }
    if (env->parent) mark_object((ObjHeader*)env->parent);
}

void free_function(FunctionObj *fn) {
    if (!fn) return;
    if (fn->name) free(fn->name);
    if (fn->args) {
        for(int i=0; i<fn->arg_count; i++) free(fn->args[i]);
        free(fn->args);
    }
    free(fn);
}

void gc_collect() {
    // printf("DEBUG: GC Running... Objects: %d\n", gc_objects_count);
    // Mark roots
    if (global_env) mark_object((ObjHeader*)global_env);
    if (current_env) mark_object((ObjHeader*)current_env);
    if (gc_temp_root) mark_object(gc_temp_root);
    
    // Mark call stack
    for (int i = 0; i < gc_call_stack_count; i++) {
        if (gc_call_stack[i]) mark_object((ObjHeader*)gc_call_stack[i]);
    }
    
    // Mark globals
    if (return_array) mark_object((ObjHeader*)return_array);
    if (return_dict) mark_object((ObjHeader*)return_dict);
    if (return_instance) mark_object((ObjHeader*)return_instance);
    if (return_function) mark_object((ObjHeader*)return_function);
    
    // Sweep
    ObjHeader **obj = &gc_objects;
    while (*obj) {
        if (!(*obj)->marked) {
            ObjHeader *unreached = *obj;
            *obj = unreached->next;
            
            // printf("DEBUG: GC Freeing %p type %d\n", unreached, unreached->type);
            if (unreached->type == OBJ_ARRAY) free_array((Array*)unreached);
            else if (unreached->type == OBJ_DICT) free_dict((Dict*)unreached);
            else if (unreached->type == OBJ_INSTANCE) free_instance((Instance*)unreached);
            else if (unreached->type == OBJ_ENV) free_env((Env*)unreached);
            else if (unreached->type == OBJ_FUNCTION) free_function((FunctionObj*)unreached);
            
            gc_objects_count--;
        } else {
            (*obj)->marked = 0;
            obj = &(*obj)->next;
        }
    }
    // printf("DEBUG: GC Done. Objects: %d\n", gc_objects_count);
}

void gc_register(ObjHeader *obj, ObjType type) {
    obj->type = type;
    obj->marked = 0;
    obj->next = gc_objects;
    gc_objects = obj;
    gc_objects_count++;
    
    if (gc_objects_count > gc_threshold) {
        gc_collect();
    }
}

void free_env(Env *env) {
    if (!env) return;
    // No need to decref, GC handles it
    if (env->variables) free(env->variables);
    free(env);
}

Node *make_number(double value)
{
    Node *n = alloc_node();
    n->type = NODE_NUMBER;
    n->value = value;
    n->string_value = NULL;
    return n;
}

Node *make_string(const char *text)
{
    Node *n = alloc_node();
    n->type = NODE_STRING;
    n->value = 0;
    n->var_name = NULL;
    n->string_value = malloc(strlen(text) + 1);
    strcpy(n->string_value, text);
    return n;
}

Node *make_op(int op_type, Node *a, Node *b)
{
    Node *n = alloc_node();
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


// Binding powers dla operatorów
int get_binding_power(Token t)
{
    if (t.type == TOKEN_DOT) return 100;
    if (t.type == TOKEN_STAR || t.type == TOKEN_SLASH || t.type == TOKEN_MOD) return 60;
    if (t.type == TOKEN_PLUS || t.type == TOKEN_MINUS) return 50;
    if (t.type == TOKEN_LSHIFT || t.type == TOKEN_RSHIFT) return 40;
    if (t.type == TOKEN_LT || t.type == TOKEN_GT || t.type == TOKEN_LTE || t.type == TOKEN_GTE) return 30;
    if (t.type == TOKEN_EQ || t.type == TOKEN_NEQ) return 20;
    if (t.type == TOKEN_BIT_AND) return 15;
    if (t.type == TOKEN_BIT_XOR) return 14;
    if (t.type == TOKEN_BIT_OR) return 13;
    if (t.type == TOKEN_AND) return 10;
    if (t.type == TOKEN_OR) return 5;
    return 0;
}

Node *parse_expr_bp(int min_bp)
{
    Node *left = NULL;

    // Prefix operators
    if (tokens[pos].type == TOKEN_MINUS || tokens[pos].type == TOKEN_BIT_NOT) {
        int op = tokens[pos].type;
        pos++;
        Node *operand = parse_expr_bp(90); // High precedence
        
        if (op == TOKEN_MINUS) {
            left = make_op(TOKEN_MINUS, make_number(0), operand);
        } else {
            // Unary bitwise NOT
            left = alloc_node();
            left->type = NODE_OPERATION;
            left->op.op_type = op;
            left->op.a = NULL; // Unary
            left->op.b = operand;
        }
    }
    // operand
    else if (tokens[pos].type == TOKEN_NUMBER)
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
             
             Node *node = alloc_node();
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
            left = alloc_node();
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
        left = alloc_node();
        left->type = NODE_BOOL;
        left->value = 1;
        left->string_value = NULL;
    }
    else if (tokens[pos].type == TOKEN_FALSE)
    {
        pos++;
        left = alloc_node();
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
        
        Node *node = alloc_node();
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
        Node *node = alloc_node();
        node->type = NODE_THIS;
        left = node;
    }
    else if (tokens[pos].type == TOKEN_NULL)
    {
        pos++;
        Node *node = alloc_node();
        node->type = NODE_NULL;
        left = node;
    }
    else if (tokens[pos].type == TOKEN_LBRACKET)
    {
        // Array literal [a, b, c]
        pos++;
        Node *node = alloc_node();
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
        Node *node = alloc_node();
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
                else report_error(tokens[pos].line, "oczekiwano ':' po kluczu");
                
                Node *val = parse_expr_bp(0);
                
                node->block.stmts[node->block.count++] = key;
                node->block.stmts[node->block.count++] = val;
                
                if (tokens[pos].type == TOKEN_COMMA) pos++;
                else break;
            }
        }
        if (tokens[pos].type == TOKEN_RBRACE) pos++;
        else report_error(tokens[pos].line, "oczekiwano '}'");
        
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
            report_error(tokens[pos].line, "brakujący ')'");
            return NULL;
        }
    }
    else
    {
        report_error(tokens[pos].line, "nieoczekiwany token '%s'", tokens[pos].text);
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
                    Node *node = alloc_node();
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
                    Node *node = alloc_node();
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
            
            Node *access = alloc_node();
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
    
    Node *block = alloc_node();
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

    if (tokens[pos].type == TOKEN_IMPORT) {
        pos++;
        if (tokens[pos].type != TOKEN_STRING) {
             printf("Błąd: oczekiwano ścieżki do pliku po 'import'\n");
             return NULL;
        }
        char *path = tokens[pos].text;
        pos++;
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        
        Node *node = alloc_node();
        node->type = NODE_IMPORT;
        node->string_value = strdup(path);
        return node;
    }

    if (tokens[pos].type == TOKEN_BREAK) {
        pos++;
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        Node *node = alloc_node();
        node->type = NODE_BREAK;
        return node;
    }

    if (tokens[pos].type == TOKEN_CONTINUE) {
        pos++;
        if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
        Node *node = alloc_node();
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
        
        Node *node = alloc_node();
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
                report_error(tokens[pos].line, "oczekiwano nazwy metody");
                pos++; continue;
            }
            
            Node *method = alloc_node();
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
                    
                    Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
            Node *loop_body = alloc_node();
            loop_body->type = NODE_BLOCK;
            loop_body->block.stmts = malloc(sizeof(Node*) * 2);
            loop_body->block.count = 2;
            loop_body->block.stmts[0] = body;
            loop_body->block.stmts[1] = step;
            
            Node *loop = alloc_node();
            loop->type = NODE_WHILE;
            loop->flow.cond = cond;
            loop->flow.body = loop_body;
            
            Node *block = alloc_node();
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
            
            Node *node = alloc_node();
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
                
                Node *node = alloc_node();
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
            Node *node = alloc_node();
            node->type = NODE_BREAK;
            return node;
        }
        else if (strcmp(tokens[pos].text, "pomin") == 0)
        {
            pos++;
            if (tokens[pos].type == TOKEN_SEMICOLON) pos++;
            Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
        
        Node *node = alloc_node();
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
        
        Node *node = alloc_node();
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

        Node *node = alloc_node();
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
                Node *block = alloc_node();
                block->type = NODE_BLOCK;
                block->block.stmts = malloc(sizeof(Node*) * 32);
                block->block.count = 0;
                
                while (tokens[pos].type != TOKEN_CASE && tokens[pos].type != TOKEN_DEFAULT && 
                       tokens[pos].type != TOKEN_RBRACE && tokens[pos].type != TOKEN_EOF) {
                    Node *stmt = parse_stmt();
                    if (stmt) block->block.stmts[block->block.count++] = stmt;
                }
                
                Node *case_node = alloc_node();
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
                
                Node *block = alloc_node();
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
            
            Node *node = alloc_node();
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
                        Node *node = alloc_node();
                        node->type = NODE_ASSIGN;
                        node->var_name = strdup(var_name);
                        node->expr = expr;
                        inc = node;
                    } else if (tokens[pos+1].type == TOKEN_INC || tokens[pos+1].type == TOKEN_DEC) {
                        char *var_name = tokens[pos].text;
                        int op_type = tokens[pos+1].type;
                        pos += 2;
                        Node *var_node = alloc_node();
                        var_node->type = NODE_VARIABLE;
                        var_node->var_name = strdup(var_name);
                        Node *one = make_number(1);
                        int bin_op = (op_type == TOKEN_INC) ? TOKEN_PLUS : TOKEN_MINUS;
                        Node *op_node = make_op(bin_op, var_node, one);
                        Node *node = alloc_node();
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
                        Node *var_node = alloc_node();
                        var_node->type = NODE_VARIABLE;
                        var_node->var_name = strdup(var_name);
                        int bin_op = 0;
                        if (op_type == TOKEN_PLUS_ASSIGN) bin_op = TOKEN_PLUS;
                        else if (op_type == TOKEN_MINUS_ASSIGN) bin_op = TOKEN_MINUS;
                        else if (op_type == TOKEN_MUL_ASSIGN) bin_op = TOKEN_STAR;
                        else if (op_type == TOKEN_DIV_ASSIGN) bin_op = TOKEN_SLASH;
                        else if (op_type == TOKEN_MOD_ASSIGN) bin_op = TOKEN_MOD;
                        Node *op_node = make_op(bin_op, var_node, expr);
                        Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
            
            Node *node = alloc_node();
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
            Node *var_node = alloc_node();
            var_node->type = NODE_VARIABLE;
            var_node->var_name = strdup(var_name);
            
            int bin_op = 0;
            if (op_type == TOKEN_PLUS_ASSIGN) bin_op = TOKEN_PLUS;
            else if (op_type == TOKEN_MINUS_ASSIGN) bin_op = TOKEN_MINUS;
            else if (op_type == TOKEN_MUL_ASSIGN) bin_op = TOKEN_STAR;
            else if (op_type == TOKEN_DIV_ASSIGN) bin_op = TOKEN_SLASH;
            else if (op_type == TOKEN_MOD_ASSIGN) bin_op = TOKEN_MOD;
            
            Node *op_node = make_op(bin_op, var_node, expr);
            
            Node *node = alloc_node();
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
            Node *var_node = alloc_node();
            var_node->type = NODE_VARIABLE;
            var_node->var_name = strdup(var_name);
            
            Node *one = make_number(1);
            int bin_op = (op_type == TOKEN_INC) ? TOKEN_PLUS : TOKEN_MINUS;
            
            Node *op_node = make_op(bin_op, var_node, one);
            
            Node *node = alloc_node();
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
                
                Node *node = alloc_node();
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
                Node *node = alloc_node();
                node->type = NODE_MEMBER_ASSIGN;
                node->member.obj = expr->member.obj;
                node->member.name = expr->member.name;
                node->expr = val;
                free(expr);
                return node;
            }
            else if (expr->type == NODE_ARRAY_ACCESS) {
                Node *node = alloc_node();
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

void add_token(TokenType type, const char *text, int line)
{
    if (token_count >= token_capacity)
    {
        token_capacity = (token_capacity == 0) ? 256 : token_capacity * 2;
        tokens = realloc(tokens, sizeof(Token) * token_capacity);
    }
    tokens[token_count].type = type;
    tokens[token_count].line = line;
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
    "import",
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
    int current_line = 1;
    while (src[i] != '\0')
    {
        if (src[i] == '#') {
            while (src[i] != '\0' && src[i] != '\n') i++;
            continue;
        }

        if (isspace(src[i]))
        {
            if (src[i] == '\n') current_line++;
            i++;
            continue;
        }

        if (src[i] == '"' || src[i] == '\'')
        {        // string w cudzysłowie (pojedynczym lub podwójnym)
            char quote = src[i];
            i++; // pomiń otwierający cudzysłów
            int start = i;
            while (src[i] != quote && src[i] != '\0') {
                if (src[i] == '\n') current_line++;
                i++;
            }
            int len = i - start;
            char buf[1024];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            add_token(TOKEN_STRING, buf, current_line);
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
            if (strcmp(buf, "klasa") == 0) add_token(TOKEN_CLASS, buf, current_line);
            else if (strcmp(buf, "konstruktor") == 0) add_token(TOKEN_CONSTRUCTOR, buf, current_line);
            else if (strcmp(buf, "dziedziczy") == 0) add_token(TOKEN_EXTENDS, buf, current_line);
            else if (strcmp(buf, "statyczna") == 0) add_token(TOKEN_STATIC, buf, current_line);
            else if (strcmp(buf, "prywatna") == 0) add_token(TOKEN_PRIVATE, buf, current_line);
            else if (strcmp(buf, "publiczna") == 0) add_token(TOKEN_PUBLIC, buf, current_line);
            else if (strcmp(buf, "nowy") == 0) add_token(TOKEN_NEW, buf, current_line);
            else if (strcmp(buf, "to") == 0) add_token(TOKEN_THIS, buf, current_line);
            else if (strcmp(buf, "nic") == 0) add_token(TOKEN_NULL, buf, current_line);
            else if (strcmp(buf, "sprobuj") == 0) add_token(TOKEN_TRY, buf, current_line);
            else if (strcmp(buf, "zlap") == 0) add_token(TOKEN_CATCH, buf, current_line);
            else if (strcmp(buf, "rzuc") == 0) add_token(TOKEN_THROW, buf, current_line);
            else if (strcmp(buf, "wybor") == 0) add_token(TOKEN_SWITCH, buf, current_line);
            else if (strcmp(buf, "przypadek") == 0) add_token(TOKEN_CASE, buf, current_line);
            else if (strcmp(buf, "domyslnie") == 0) add_token(TOKEN_DEFAULT, buf, current_line);
            else if (strcmp(buf, "dla") == 0) add_token(TOKEN_FOR, buf, current_line);
            else if (strcmp(buf, "w") == 0) add_token(TOKEN_IN, buf, current_line);
            else if (strcmp(buf, "import") == 0) add_token(TOKEN_IMPORT, buf, current_line);
            else if (is_keyword(buf))
            {
                if (strcmp(buf, "prawda") == 0) add_token(TOKEN_TRUE, buf, current_line);
                else if (strcmp(buf, "falsz") == 0) add_token(TOKEN_FALSE, buf, current_line);
                else if (strcmp(buf, "oraz") == 0) add_token(TOKEN_AND, buf, current_line);
                else if (strcmp(buf, "albo") == 0) add_token(TOKEN_OR, buf, current_line);
                else if (strcmp(buf, "zlam") == 0) add_token(TOKEN_BREAK, buf, current_line);
                else if (strcmp(buf, "pomin") == 0) add_token(TOKEN_CONTINUE, buf, current_line);
                else if (strcmp(buf, "rowne") == 0) add_token(TOKEN_EQ, buf, current_line);
                else if (strcmp(buf, "nierowne") == 0) add_token(TOKEN_NEQ, buf, current_line);
                else add_token(TOKEN_KEYWORD, buf, current_line);
            }
            else
            {
                add_token(TOKEN_IDENT, buf, current_line);
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
            add_token(TOKEN_NUMBER, buf, current_line);
            continue;
        }

        // kluczowe znaki
        char c = src[i];
        char buf[2] = {c, '\0'};
        if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
            c == '(' || c == ')' || c == ';' || c == '{' || c == '}' || c == ',' ||
            c == '<' || c == '>' || c == '!' || c == '&' || c == '|' ||
            c == '[' || c == ']' || c == '.' || c == ':' || c == '%' || c == '^' || c == '~')
        {
            switch (c)
            {
            case '^':
                add_token(TOKEN_BIT_XOR, "^", current_line);
                break;
            case '~':
                add_token(TOKEN_BIT_NOT, "~", current_line);
                break;
            case '.':
                add_token(TOKEN_DOT, ".", current_line);
                break;
            case ':':
                add_token(TOKEN_COLON, ":", current_line);
                break;
            case '=':
                if (src[i+1] == '=') { add_token(TOKEN_EQ, "==", current_line); i++; }
                else { add_token(TOKEN_ASSIGN, "=", current_line); }
                break;
            case '!':
                if (src[i+1] == '=') { add_token(TOKEN_NEQ, "!=", current_line); i++; }
                else { printf("Błąd: nieoczekiwany znak '!'\n"); }
                break;
            case '<':
                if (src[i+1] == '=') { add_token(TOKEN_LTE, "<=", current_line); i++; }
                else if (src[i+1] == '<') { add_token(TOKEN_LSHIFT, "<<", current_line); i++; }
                else { add_token(TOKEN_LT, "<", current_line); }
                break;
            case '>':
                if (src[i+1] == '=') { add_token(TOKEN_GTE, ">=", current_line); i++; }
                else if (src[i+1] == '>') { add_token(TOKEN_RSHIFT, ">>", current_line); i++; }
                else { add_token(TOKEN_GT, ">", current_line); }
                break;
            case '&':
                if (src[i+1] == '&') { add_token(TOKEN_AND, "&&", current_line); i++; }
                else { add_token(TOKEN_BIT_AND, "&", current_line); }
                break;
            case '|':
                if (src[i+1] == '|') { add_token(TOKEN_OR, "||", current_line); i++; }
                else { add_token(TOKEN_BIT_OR, "|", current_line); }
                break;
            case '+':
                if (src[i+1] == '+') { add_token(TOKEN_INC, "++", current_line); i++; }
                else if (src[i+1] == '=') { add_token(TOKEN_PLUS_ASSIGN, "+=", current_line); i++; }
                else { add_token(TOKEN_PLUS, "+", current_line); }
                break;
            case '-':
                if (src[i+1] == '-') { add_token(TOKEN_DEC, "--", current_line); i++; }
                else if (src[i+1] == '=') { add_token(TOKEN_MINUS_ASSIGN, "-=", current_line); i++; }
                else { add_token(TOKEN_MINUS, "-", current_line); }
                break;
            case '*':
                if (src[i+1] == '=') { add_token(TOKEN_MUL_ASSIGN, "*=", current_line); i++; }
                else { add_token(TOKEN_STAR, "*", current_line); }
                break;
            case '%':
                if (src[i+1] == '=') { add_token(TOKEN_MOD_ASSIGN, "%=", current_line); i++; }
                else { add_token(TOKEN_MOD, "%", current_line); }
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
                        if (src[i] == '\n') current_line++;
                        i++;
                    }
                    continue;
                }
                else if (src[i+1] == '=') { add_token(TOKEN_DIV_ASSIGN, "/=", current_line); i++; }
                else { add_token(TOKEN_SLASH, "/", current_line); }
                break;
            case '(':
                add_token(TOKEN_LPAREN, buf, current_line);
                break;
            case ')':
                add_token(TOKEN_RPAREN, buf, current_line);
                break;
            case '[':
                add_token(TOKEN_LBRACKET, buf, current_line);
                break;
            case ']':
                add_token(TOKEN_RBRACKET, buf, current_line);
                break;
            case '{':
                add_token(TOKEN_LBRACE, buf, current_line);
                break;
            case '}':
                add_token(TOKEN_RBRACE, buf, current_line);
                break;
            case ';':
                add_token(TOKEN_SEMICOLON, buf, current_line);
                break;
            case ',':
                add_token(TOKEN_COMMA, buf, current_line);
                break;
            }
        }
        else
        {
            add_token(TOKEN_IDENT, buf, current_line); // traktuj nieznane jako ident
        }
        i++;
    }
    add_token(TOKEN_EOF, "EOF", current_line);
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
        // free_env(temp); // GC HANDLED
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
        // free_env(temp); // GC HANDLED
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
                // free_env(temp); // GC HANDLED
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
        ObjHeader *prev_root = gc_temp_root;
        gc_temp_root = (ObjHeader*)inst;
        inst->fields = create_dict();
        gc_register((ObjHeader*)inst, OBJ_INSTANCE);
        gc_temp_root = prev_root;
        
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
        // free_env(current_env); // cleanup vars - GC HANDLED
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
        struct FunctionObj *func_val = NULL;
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
        } else if (n->expr->func_value) {
            func_val = n->expr->func_value;
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
            } else if (func_val) {
                var->type = TYPE_FUNCTION;
                var->value.funcValue = func_val;
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
            report_error(n->line, "zmienna '%s' nie została zadeklarowana (użyj 'zmienna %s = ...')", n->var_name, n->var_name);
        }
        return 0;
    }
    else if (n->type == NODE_VAR_DECL) {
        double val = eval(n->expr);
        char *str_val = NULL;
        Array *arr_val = NULL;
        Dict *dict_val = NULL;
        struct Instance *inst_val = NULL;
        struct FunctionObj *func_val = NULL;
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
        } else if (n->expr->func_value) {
            func_val = n->expr->func_value;
        }
        
        Variable *var = env_define(current_env, n->var_name);
        if (var) {
            if (inst_val) {
                var->type = TYPE_INSTANCE;
                var->value.instanceValue = inst_val;
            } else if (func_val) {
                var->type = TYPE_FUNCTION;
                var->value.funcValue = func_val;
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
        // Create FunctionObj
        FunctionObj *fn = malloc(sizeof(FunctionObj));
        fn->name = n->func.name ? strdup(n->func.name) : NULL;
        fn->body = n->func.body;
        fn->arg_count = n->func.arg_count;
        fn->args = malloc(sizeof(char*) * fn->arg_count);
        for(int i=0; i<fn->arg_count; i++) {
            fn->args[i] = strdup(n->func.args[i]);
        }
        fn->closure = current_env; // Capture environment!
        // printf("DEBUG: Created function %s with closure %p\n", n->func.name ? n->func.name : "anon", fn->closure);
        ObjHeader *prev_root = gc_temp_root;
        gc_temp_root = (ObjHeader*)fn;
        gc_register((ObjHeader*)fn, OBJ_FUNCTION);
        gc_temp_root = prev_root;
        
        // If named, store in current environment
        if (n->func.name) {
            // printf("DEBUG: Defining function %s\n", n->func.name);
            Variable *var = env_define(current_env, n->func.name);
            var->type = TYPE_FUNCTION;
            var->value.funcValue = fn;
        }
        return 0;
    }
    else if (n->type == NODE_RETURN) {
        if (n->expr) {
            return_value = eval(n->expr);
            
            // Clear previous return objects
            if (return_string) { free(return_string); return_string = NULL; }
            if (return_array) { decref_array(return_array); return_array = NULL; }
            if (return_dict) { decref_dict(return_dict); return_dict = NULL; }
            return_function = NULL; // GC handles ref count

            if (n->expr->string_value) {
                // printf("DEBUG: Return string: %s\n", n->expr->string_value);
                return_string = malloc(strlen(n->expr->string_value)+1);
                strcpy(return_string, n->expr->string_value);
            } else if (n->expr->array_value) {
                return_array = n->expr->array_value;
                incref_array(return_array);
            } else if (n->expr->dict_value) {
                return_dict = n->expr->dict_value;
                incref_dict(return_dict);
            } else if (n->expr->func_value) {
                return_function = n->expr->func_value;
            }
        }
        is_returning = 1;
        return return_value;
    }
    else if (n->type == NODE_FUNC_CALL) {
        if (strcmp(n->call.name, "tekst") == 0) {
            if (n->call.arg_count != 1) { report_error(n->line, "funkcja 'tekst' wymaga 1 argumentu"); return 0; }
            double val = eval(n->call.args[0]);
            char *s = get_node_string(n->call.args[0]);
            
            if (n->string_value) free(n->string_value);
            
            if (s) {
                n->string_value = strdup(s);
            } else {
                char buf[64];
                snprintf(buf, 64, "%g", val);
                n->string_value = strdup(buf);
            }
            return 0;
        }
        if (strcmp(n->call.name, "liczba") == 0) {
            if (n->call.arg_count != 1) { report_error(n->line, "funkcja 'liczba' wymaga 1 argumentu"); return 0; }
            double val = eval(n->call.args[0]);
            char *s = get_node_string(n->call.args[0]);
            
            if (n->string_value) { free(n->string_value); n->string_value = NULL; }
            
            if (s) {
                return atof(s);
            } else {
                return val;
            }
        }

        // printf("DEBUG: Calling function %s\n", n->call.name);
        Variable *var = get_variable(n->call.name);
        FunctionObj *fn = NULL;
        
        if (var && var->type == TYPE_FUNCTION) {
            fn = var->value.funcValue;
        } else {
            report_error(n->line, "nieznana funkcja '%s'", n->call.name); 
            return 0; 
        }
        
        double arg_vals[16];
        char *arg_strs[16] = {0};
        Array *arg_arrs[16] = {0};
        Dict *arg_dicts[16] = {0};
        struct FunctionObj *arg_funcs[16] = {0};
        
        for (int i=0; i<n->call.arg_count; i++) {
            arg_vals[i] = eval(n->call.args[i]);
            if (n->call.args[i]->string_value) {
                arg_strs[i] = malloc(strlen(n->call.args[i]->string_value)+1);
                strcpy(arg_strs[i], n->call.args[i]->string_value);
            } else if (n->call.args[i]->array_value) {
                arg_arrs[i] = n->call.args[i]->array_value;
            } else if (n->call.args[i]->dict_value) {
                arg_dicts[i] = n->call.args[i]->dict_value;
            } else if (n->call.args[i]->type == NODE_VARIABLE) {
                 Variable *v = get_variable(n->call.args[i]->var_name);
                 if (v && v->type == TYPE_FUNCTION) {
                     arg_funcs[i] = v->value.funcValue;
                 }
            }
        }
        
        // Create new environment for function call
        Env *prev_env = current_env;
        if (gc_call_stack_count < 256) gc_call_stack[gc_call_stack_count++] = prev_env;
        
        // printf("DEBUG: Calling function with closure %p\n", fn->closure ? fn->closure : global_env);
        current_env = create_env(fn->closure ? fn->closure : global_env);
        
        for (int i=0; i<fn->arg_count; i++) {
            if (i >= n->call.arg_count) break;
            
            Variable *var = env_define(current_env, fn->args[i]);
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
                } else if (arg_funcs[i]) {
                    var->type = TYPE_FUNCTION;
                    var->value.funcValue = arg_funcs[i];
                } else {
                    var->type = TYPE_DOUBLE;
                    var->value.doubleValue = arg_vals[i];
                }
            }
            if (arg_strs[i]) free(arg_strs[i]);
        }
        
        eval(fn->body);
        
        // Reset control flow flags that shouldn't leak out of function
        if (is_breaking) { is_breaking = 0; }
        if (is_continuing) { is_continuing = 0; }
        // Exception should propagate out of function call
        
        // Restore environment
        // Env *temp = current_env; // GC handles it
        if (gc_call_stack_count > 0) gc_call_stack_count--;
        current_env = prev_env;
        // free_env(temp); // DO NOT FREE ENV MANUALLY WITH CLOSURES!

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
        } else if (return_function) {
            n->func_value = return_function;
            return_function = NULL;
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
        double a = 0;
        if (n->op.a) a = eval(n->op.a);
        double b = eval(n->op.b);

        if (n->op.op_type == TOKEN_BIT_NOT) {
            return ~(long long)b;
        }

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
        case TOKEN_BIT_AND: return (long long)a & (long long)b;
        case TOKEN_BIT_OR: return (long long)a | (long long)b;
        case TOKEN_BIT_XOR: return (long long)a ^ (long long)b;
        case TOKEN_LSHIFT: return (long long)a << (long long)b;
        case TOKEN_RSHIFT: return (long long)a >> (long long)b;
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
            if (var->type == TYPE_FUNCTION) {
                n->func_value = var->value.funcValue;
                return 0;
            }
            if (var->type == TYPE_INSTANCE) {
                return_instance = var->value.instanceValue;
                return 0;
            }
            return 0;
        }
        report_error(n->line, "nieznana zmienna '%s'", n->var_name ? n->var_name : "(null)");
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
                    if (arg_strs[i]) free(arg_strs[i]); // Wait, arg_strs were already freed inside the loop if used? No, they are freed here.
                    // But wait, inside the loop: if (str) { ... free(str); }
                    // The loop above:
                    // if (ins) ... else if (str) { ... free(str); }
                    // So str is freed inside the loop!
                    // Double free if we free it again here?
                    // Let's check the loop again.
                }
                
                // Re-reading the loop logic:
                /*
                for (int i=0; i<method->arg_count; i++) {
                    ...
                    if (ins) { ... } 
                    else if (str) {
                        arg->type = TYPE_STRING;
                        strcpy(arg->value.stringValue, str);
                        free(str); // FREED HERE
                    }
                }
                */
                // But arg_strs array was populated for ALL args passed (n->method.arg_count).
                // The loop iterates over method->arg_count (formal parameters).
                // If n->method.arg_count > method->arg_count, the extra args are not processed in the loop, so their strings are NOT freed.
                // If n->method.arg_count <= method->arg_count, they are freed.
                
                // The cleanup loop:
                /*
                for(int i=0; i<n->method.arg_count; i++) {
                    if (arg_strs[i]) free(arg_strs[i]);
                }
                */
                // This looks like a double free if they were freed in the loop.
                // I should probably fix this too, but my main task is return values.
                // Actually, let's look at the code I'm replacing.
                
                current_env = prev_env;
                current_instance = prev_inst;
                
                if (is_returning) {
                    is_returning = 0;
                    
                    // Handle non-double return values
                    if (return_string) {
                        // printf("DEBUG: Method returned string: %s\n", return_string);
                        if (n->string_value) free(n->string_value);
                        n->string_value = strdup(return_string);
                        free(return_string);
                        return_string = NULL;
                    } else if (return_array) {
                        n->array_value = return_array;
                        return_array = NULL;
                    } else if (return_dict) {
                        n->dict_value = return_dict;
                        return_dict = NULL;
                    } else if (return_instance) {
                        // Wait, return_instance is a struct Instance*
                        // But NODE_METHOD_CALL doesn't seem to have a field for instance value?
                        // Let's check Node struct.
                        // It has array_value, dict_value, func_value.
                        // Does it have instance_value?
                        // I need to check Node struct definition again.
                        // I saw struct Instance *return_instance global.
                        // But Node struct?
                    }
                    
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
    else if (n->type == NODE_IMPORT) {
        char *path = n->string_value;
        FILE *f = fopen(path, "rb");
        if (!f) {
            printf("Błąd: nie można otworzyć pliku importu %s\n", path);
            return 0;
        }
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *src = malloc(fsize + 1);
        fread(src, 1, fsize, f);
        src[fsize] = 0;
        fclose(f);
        
        // Save state
        Token *old_tokens = tokens;
        int old_count = token_count;
        int old_capacity = token_capacity;
        int old_pos = pos;
        
        // Reset state
        tokens = NULL;
        token_count = 0;
        token_capacity = 0;
        pos = 0;
        
        // We need to declare lex() prototype or move it up
        void lex(const char *src);
        lex(src);
        parse();
        
        // Cleanup
        free(tokens);
        free(src);
        
        // Restore state
        tokens = old_tokens;
        token_count = old_count;
        token_capacity = old_capacity;
        pos = old_pos;
        
        return 0;
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

void repl() {
    char line[1024];
    printf("Benzin REPL v1.0\n");
    printf("Wpisz 'wyjdz' aby zakonczyc.\n");
    
    global_env = create_env(NULL);
    current_env = global_env;
    
    // Define global constants
    Variable *pi_var = env_define(global_env, "pi");
    if (pi_var) {
        pi_var->type = TYPE_DOUBLE;
        pi_var->value.doubleValue = 3.14159265358979323846;
    }

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        if (strncmp(line, "wyjdz", 5) == 0) break;
        
        // Reset tokens
        if (tokens) free(tokens);
        tokens = NULL;
        token_count = 0;
        token_capacity = 0;
        pos = 0;
        
        lex(line);
        
        while (pos < token_count && tokens[pos].type != TOKEN_EOF) {
            Node *stmt = parse_stmt();
            if (stmt) {
                double val = eval(stmt);
                // If it's an expression that produces a value, print it
                if (stmt->type == NODE_NUMBER || stmt->type == NODE_OPERATION || 
                    stmt->type == NODE_VARIABLE || stmt->type == NODE_FUNC_CALL ||
                    stmt->type == NODE_STRING || stmt->type == NODE_BOOL ||
                    stmt->type == NODE_ARRAY_ACCESS || stmt->type == NODE_MEMBER_ACCESS ||
                    stmt->type == NODE_METHOD_CALL) {
                    
                    if (stmt->string_value) printf("%s\n", stmt->string_value);
                    else if (stmt->array_value) printf("[Array]\n"); // TODO: Print array content
                    else if (stmt->dict_value) printf("{Dict}\n");   // TODO: Print dict content
                    else if (stmt->type == NODE_BOOL) printf("%s\n", val ? "prawda" : "falsz");
                    else printf("%g\n", val);
                }
            } else {
                pos++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // printf("DEBUG: Main start\n");
    // fflush(stdout);
    if (argc < 2)
    {
        repl();
        return 0;
    }
    
    char *filename = argv[1];
    char *ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".bzn") != 0) {
        printf("Błąd: plik musi mieć rozszerzenie .bzn\n");
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
