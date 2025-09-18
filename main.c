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
    // Pola wspólne dla wszystkich typów
    double value;      // dla liczb i obliczonych wartości
    char *var_name;    // dla zmiennych
    struct Node *expr; // dla wyrażeń (PRINT itp.)
    
    // Pola specyficzne dla operatorów
    struct
    {
        char sign;      // np. '+', '*'
        struct Node *a; // lewe poddrzewo
        struct Node *b; // prawe poddrzewo
    } op;
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

#define MAX_VARS 32

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
} VarType;

typedef struct {
    char name[64]; // <-- tu trzymamy nazwę zmiennej
    VarType type;
    union {
        int intValue;
        double doubleValue;
        char stringValue[64];
    } value;
} Variable;

Variable variables[MAX_VARS];
int var_count = 0;

Variable *get_variable(const char *name)
{
    if (!name) return NULL; // sprawdź NULL
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            return &variables[i];
        }
    }
    return NULL;
}

// Deklaracje funkcji
double eval(Node *n);

Node *make_number(double value)
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
        left = make_number(atof(tokens[pos].text));
        pos++;
    }
    else if (tokens[pos].type == TOKEN_IDENT)
    {
        if(!get_variable(tokens[pos].text))
        {
            printf("Błąd: nieznana zmienna %s\n", tokens[pos].text);
            return NULL;
        }
        char *var_name = tokens[pos].text;
        pos++;
        left = malloc(sizeof(Node));
        left->type = NODE_VARIABLE;
        // Bezpieczne kopiowanie nazwy zmiennej
        left->var_name = malloc(strlen(var_name) + 1);
        strcpy(left->var_name, var_name);
        left->value = 0; // Wartość będzie ustawiona przez eval jeśli potrzebne
    }
    else if (tokens[pos].type == TOKEN_LPAREN) // <-- NOWE
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

Node *parse_stmt()
{
    if (tokens[pos].type == TOKEN_KEYWORD && strcmp(tokens[pos].text, "zmienna") == 0)
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
                if (tokens[pos].type == TOKEN_SEMICOLON)
                {
                    pos++;
                }

                // Oblicz wartość wyrażenia
                double result = eval(expr);

                // Sprawdź czy zmienna już istnieje (tylko jesli nie jest to pierwsza deklaracja)
                if (var_count > 0) {
                    Variable *existing_var = get_variable(var_name);
                    if (existing_var)
                    {
                        printf("Błąd: zmienna %s już istnieje\n", var_name);
                        return NULL;
                    }
                }

                // Dodaj zmienną do tablicy variables
                if (var_count < MAX_VARS)
                {
                    Variable new_var;
                    strncpy(new_var.name, var_name, 63);
                    new_var.name[63] = '\0';
                    new_var.type = TYPE_DOUBLE;
                    new_var.value.doubleValue = result;
                    
                    variables[var_count++] = new_var;
                }
                else
                {
                    printf("Błąd: przekroczono limit zmiennych\n");
                    return NULL;
                }

                Node *n = malloc(sizeof(Node));
                n->type = NODE_VARIABLE;
                // Bezpieczne kopiowanie nazwy zmiennej
                n->var_name = malloc(strlen(var_name) + 1);
                strcpy(n->var_name, var_name);
                n->value = result;
                return n;
            }
            return NULL;
        }
        else
        {
            printf("Błąd: oczekiwano identyfikatora po 'zmienna'\n");
            return NULL;
        }
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
            c == '(' || c == ')' || c == ';' || c == '{' || c == '}' || c == ',' )
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
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        }
    }
    else if (n->type == NODE_VARIABLE)
    {
        // Sprawdź czy n i n->var_name nie są NULL
        if (!n || !n->var_name) {
            printf("Błąd: nieprawidłowy węzeł zmiennej\n");
            return 0;
        }
        
        // Szukamy zmiennej w tablicy variables
        Variable *var = get_variable(n->var_name);
        if (var)
        {
            if (var->type == TYPE_INT)
                return var->value.intValue;
            if (var->type == TYPE_DOUBLE)
                return var->value.doubleValue;
            printf("Błąd: zmienna %s nie jest liczbą\n", n->var_name);
            return 0;
        }
        printf("Nieznana zmienna: %s\n", n->var_name ? n->var_name : "(null)");
        return 0;
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
        else if (tokens[pos].type == TOKEN_KEYWORD)
        {
            Node *stmt = parse_stmt();
            if(stmt)
            {
                // Dla deklaracji zmiennej nie wywołujemy eval - zmienna już została zapisana w parse_stmt
                free(stmt);
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
    
    for(int i = 0; i < token_count; i++)
    {
        printf("Token %d: typ=%d, tekst='%s'\n", i, tokens[i].type, tokens[i].text);
    }
    
    parse();
    for (int i = 0; i < var_count; i++)
    {
        printf("Zmienna %s = ", variables[i].name);
        if (variables[i].type == TYPE_INT)
            printf("%d\n", variables[i].value.intValue);
        else if (variables[i].type == TYPE_DOUBLE)
            printf("%lf\n", variables[i].value.doubleValue);
        else if (variables[i].type == TYPE_STRING)
            printf("'%s'\n", variables[i].value.stringValue);
        else
            printf("Nieznany typ\n");
    }
    return 0;
}