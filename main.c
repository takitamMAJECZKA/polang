#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum
{
    NODE_NUMBER,
    NODE_OPERATION,
    NODE_PRINT,
    NODE_INPUT,
    NODE_VARIABLE,
} NodeType;

typedef struct Node
{
    NodeType type;
    union
    {
        double value; // jeśli to liczba
        struct
        {
            char sign;      // np. '+', '*'
            struct Node *a; // lewe poddrzewo
            struct Node *b; // prawe poddrzewo
        } op;
        struct Node *expr; // dla PRINT – co wypisać
        char *var_name;    // dla INPUT i VAR
    };
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
    TOKEN_OPERATOR, // operator (np. +, -, *, /)
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

Node *make_number(int value)
{
    Node *n = malloc(sizeof(Node));
    n->type = NODE_NUMBER;
    n->value = value;
    return n;
}

Node *make_op(char sign, Node *a, Node *b)
{
    Node *n = malloc(sizeof(Node));
    n->type = NODE_OPERATION;
    n->op.sign = sign;
    n->op.a = a;
    n->op.b = b;
    return n;
}

int pos = 0;

// Binding powers dla operatorów
int get_binding_power(const char *op)
{
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0)
        return 20;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0)
        return 10;
    return 0;
}

Node *parse_expr_bp(int min_bp)
{
    Node *left = NULL;

    // operand
    if (tokens[pos].type == TOKEN_NUMBER)
    {
        left = make_number(atoi(tokens[pos].text));
        pos++;
    }
    else if (tokens[pos].type == TOKEN_LPAREN)   // <-- NOWE
    {
        pos++; // pomiń '('
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
        printf("Błąd składni\n");
        return NULL;
    }

    // operator loop
    while (tokens[pos].type == TOKEN_OPERATOR)
    {
        int bp = get_binding_power(tokens[pos].text);
        if (bp < min_bp)
            break;

        char op = tokens[pos].text[0];
        pos++;

        Node *right = parse_expr_bp(bp + 1);
        left = make_op(op, left, right);
    }

    return left;
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
    "jeżeli",
    "inaczej",
    "dla",
    "funkcja",
    "zwróć",
    "zmienna",
    "wkółko"};
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

void lex(const char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
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

        if (isalpha(src[i]))
        { // ident lub keyword
            int start = i;
            while (isalnum(src[i]))
                i++;
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            if (is_keyword(buf))
            {
                add_token(TOKEN_KEYWORD, buf);
            }
            else
            {
                add_token(TOKEN_IDENT, buf);
            }
            continue;
        }

        if (isdigit(src[i]))
        { // liczba
            int start = i;
            while (isdigit(src[i]))
                i++;
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
            c == '(' || c == ')' || c == ';' || c == '{' || c == '}' || c == ',')
        {
            switch (c)
            {
            case '=':
                add_token(TOKEN_ASSIGN, buf);
                break;
            case '+':
                add_token(TOKEN_OPERATOR, buf);
                break;
            case '-':
                add_token(TOKEN_OPERATOR, buf);
                break;
            case '*':
                add_token(TOKEN_OPERATOR, buf);
                break;
            case '/':
                add_token(TOKEN_OPERATOR, buf);
                break;
            case '(':
                add_token(TOKEN_LPAREN, buf);
                break;
            case ')':
                add_token(TOKEN_RPAREN, buf);
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

// Prosty parser: rozpoznaje 'pisz' <ident>

// Prosta tablica zmiennych
#define MAX_VARS 32
typedef enum{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
} VarType;

typedef struct {
    char name[64];
    VarType type;
    union {
        int intValue;
        double doubleValue;
        char stringValue[64];
    } value;
} Variable;

Variable variables[MAX_VARS];
int var_count = 0;

void token_to_variable(const char* name, const char* value) {
    Variable var;
    char* end;
    if(var_count >= MAX_VARS) {
        printf("Za dużo zmiennych!\n");
        return;
    }
    strncpy(var.name, name, 63);
    var.name[63] = '\0';
    long intVal = strtol(value, &end, 10);
    if(end != value && *end == '\0') {
        var.type = TYPE_INT;
        var.value.intValue = (int)intVal;
        variables[var_count++] = var;
        return;
    }
    double doubleVal = strtod(value, &end);
    if(end != value && *end == '\0') {
        var.type = TYPE_DOUBLE;
        var.value.doubleValue = doubleVal;
        variables[var_count++] = var;
        return;
    }
    var.type = TYPE_STRING;
    strncpy(var.value.stringValue, value, 63);
    var.value.stringValue[63] = '\0';
    variables[var_count++] = var;
}

Variable* get_variable(const char* name) {
    for(int i = 0; i < var_count; i++) {
        if(strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

double eval(Node *n)
{
    if (n->type == NODE_NUMBER)
    {
        return n->value;
    }
    else if (n->type == NODE_OPERATION)
    {
        double a = eval(n->op.a);
        double b = eval(n->op.b);
        switch (n->op.sign)
        {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        }
    }
    else if (n->type == NODE_VARIABLE)
    {
        for (int i = 0; i < var_count; i++)
        {
            Variable *var = get_variable(n->var_name);
        }
        printf("Nieznana zmienna: %s\n", n->var_name);
    }
    return 0;
}
void parse()
{
    while (pos < token_count && tokens[pos].type != TOKEN_EOF)
    {
        // wyrażenie może zaczynać się od liczby, identyfikatora albo nawiasu
        if (tokens[pos].type == TOKEN_NUMBER ||
            tokens[pos].type == TOKEN_IDENT ||
            tokens[pos].type == TOKEN_LPAREN)
        {
            Node *expr = parse_expr_bp(0); // wciąga całe wyrażenie
            if (expr)
            {
                printf("Wynik: %lf\n", eval(expr));
            }
            if (tokens[pos].type == TOKEN_SEMICOLON)
            {
                pos++; // pomiń średnik
            }
        }
        else
        {
            // jeśli to nie początek wyrażenia, pomiń token (np. EOF albo coś nieoczekiwanego)
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
    // Wypisz wszystkie tokeny
    // for (int i = 0; i < token_count; i++) {
    //     printf("Token %d: typ=%d, wartosc='%s'\n", i, tokens[i].type, tokens[i].text);
    // }
    parse();
    return 0;
}