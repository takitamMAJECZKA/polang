#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    NODE_NUMBER,
    NODE_OPERATION
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        int value; // jeśli to liczba
        struct {
            char sign;        // np. '+', '*'
            struct Node *a;   // lewe poddrzewo
            struct Node *b;   // prawe poddrzewo
        } op;
    };
} Node;

typedef struct {
    TokenType type;
    char text[64];
} Token;

#define MAX_TOKENS 256
Token tokens[MAX_TOKENS];
int token_count = 0;

typedef enum {
    TOKEN_IDENT,      // nazwa zmiennej lub polecenia
    TOKEN_NUMBER,     // liczba
    TOKEN_STRING,     // tekst w cudzysłowie
    TOKEN_KEYWORD,    // słowo kluczowe (np. print)
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ASSIGN,
    TOKEN_OPERATOR,   // operator (np. +, -, *, /)
    TOKEN_EOF         // koniec pliku
} TokenType;

Node* make_number(int value) {
    Node* n = malloc(sizeof(Node));
    n->type = NODE_NUMBER;
    n->value = value;
    return n;
}

Node* make_op(char sign, Node* a, Node* b) {
    Node* n = malloc(sizeof(Node));
    n->type = NODE_OPERATION;
    n->op.sign = sign;
    n->op.a = a;
    n->op.b = b;
    return n;
}


void add_token(TokenType type, const char* text) {
    if (token_count < MAX_TOKENS) {
        tokens[token_count].type = type;
        strncpy(tokens[token_count].text, text, 63);
        tokens[token_count].text[63] = '\0';
        token_count++;
    }
}

// Tablica słów kluczowych
const char* keywords[] = {
    "pisz",
    "podaj",
    "usun",
    "jeżeli",
    "inaczej",
    "dla",
    "funkcja",
    "zwróć",
    "zmienna",
    "wkółko"
};
const int keywords_count = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(const char* text) {
    for (int i = 0; i < keywords_count; i++) {
        if (strcmp(text, keywords[i]) == 0) return 1;
    }
    return 0;
}

void lex(const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        if (isspace(src[i])) { i++; continue; }


        if (src[i] == '"') { // string w cudzysłowie
            i++; // pomiń otwierający cudzysłów
            int start = i;
            while (src[i] != '"' && src[i] != '\0') i++;
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            add_token(TOKEN_STRING, buf);
            if (src[i] == '"') i++; // pomiń zamykający cudzysłów
            continue;
        }

        if (isalpha(src[i])) { // ident lub keyword
            int start = i;
            while (isalnum(src[i])) i++;
            int len = i - start;
            char buf[64];
            strncpy(buf, src + start, len);
            buf[len] = '\0';
            if (is_keyword(buf)) {
                add_token(TOKEN_KEYWORD, buf);
            } else {
                add_token(TOKEN_IDENT, buf);
            }
            continue;
        }

        if (isdigit(src[i])) { // liczba
            int start = i;
            while (isdigit(src[i])) i++;
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
            c == '(' || c == ')' || c == ';' || c == '{' || c == '}') {
            switch(c) {
                case '=': add_token(TOKEN_ASSIGN, buf); break;
                case '+': add_token(TOKEN_OPERATOR, buf); break;
                case '-': add_token(TOKEN_OPERATOR, buf); break;
                case '*': add_token(TOKEN_OPERATOR, buf); break;
                case '/': add_token(TOKEN_OPERATOR, buf); break;
                case '(': add_token(TOKEN_LPAREN, buf); break;
                case ')': add_token(TOKEN_RPAREN, buf); break;
                case '{': add_token(TOKEN_LBRACE, buf); break;
                case '}': add_token(TOKEN_RBRACE, buf); break;
                case ';': add_token(TOKEN_SEMICOLON, buf); break;
                case ',': add_token(TOKEN_COMMA, buf); break;
            }
        } else {
            add_token(TOKEN_IDENT, buf); // traktuj nieznane jako ident
        }
        i++;
    }
    add_token(TOKEN_EOF, "EOF");
}

// Prosty parser: rozpoznaje 'pisz' <ident>

// Prosta tablica zmiennych
#define MAX_VARS 32
char var_names[MAX_VARS][64];
char var_values[MAX_VARS][64];
int var_count = 0;

// Zapisz wartosc zmiennej
void set_var(const char* name, const char* value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_names[i], name) == 0) {
            strncpy(var_values[i], value, 63);
            var_values[i][63] = '\0';
            return;
        }
    }
    if (var_count < MAX_VARS) {
        strncpy(var_names[var_count], name, 63);
        var_names[var_count][63] = '\0';
        strncpy(var_values[var_count], value, 63);
        var_values[var_count][63] = '\0';
        var_count++;
    }
}

// Pobierz wartosc zmiennej
const char* get_var(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_names[i], name) == 0) {
            return var_values[i];
        }
    }
    return NULL;
}

void parse() {
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_KEYWORD) {
            if (strcmp(tokens[i].text, "pisz") == 0) {
                if (i + 1 < token_count) {
                    if (tokens[i+1].type == TOKEN_IDENT) {
                        const char* val = get_var(tokens[i+1].text);
                        if (val) {
                            printf("Wypisz: %s\n", val);
                        } else {
                            printf("Wypisz: %s\n", tokens[i+1].text);
                        }
                        i++;
                    } else if (tokens[i+1].type == TOKEN_STRING) {
                        printf("Wypisz: %s\n", tokens[i+1].text);
                        i++;
                    }else if (tokens[i+1].type == TOKEN_NUMBER) {
                        char buf[64];
                        sprintf(buf, "%d", atoi(tokens[i+1].text));
                        printf("Wypisz: %s\n", buf);
                        i++;
                    } else {
                        printf("Błąd składni przy 'pisz'\n");
                    }
                }
            } else if (strcmp(tokens[i].text, "podaj") == 0) {
                if (i + 1 < token_count && tokens[i+1].type == TOKEN_IDENT) {
                    char buf[64];
                    printf("Podaj wartosc dla %s: ", tokens[i+1].text);
                    fgets(buf, sizeof(buf), stdin);
                    buf[strcspn(buf, "\n")] = '\0';
                    set_var(tokens[i+1].text, buf);
                    i++;
                }
            } else if (strcmp(tokens[i].text, "zmienna") == 0) {
                if (i + 2 < token_count && tokens[i+1].type == TOKEN_IDENT && tokens[i+2].type == TOKEN_ASSIGN && i + 3 < token_count) {
                    char buf[64];
                    if (tokens[i+3].type == TOKEN_NUMBER || tokens[i+3].type == TOKEN_STRING) {
                        strncpy(buf, tokens[i+3].text, 63);
                        buf[63] = '\0';
                    } else if (tokens[i+3].type == TOKEN_IDENT) {
                        const char* val = get_var(tokens[i+3].text);
                        if (val) {
                            strncpy(buf, val, 63);
                            buf[63] = '\0';
                        } else {
                            printf("Błąd: niezdefiniowana zmienna %s\n", tokens[i+3].text);
                            i += 3;
                            continue;
                        }
                    } else {
                        printf("Błąd składni przy deklaracji zmiennej\n");
                        i += 3;
                        continue;
                    }
                    set_var(tokens[i+1].text, buf);
                    i += 3;
                }
            } else if (strcmp(tokens[i].text, "usun") == 0) {
                if (i + 1 < token_count && tokens[i+1].type == TOKEN_IDENT) {
                    set_var(tokens[i+1].text, NULL);
                    i++;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Użycie: %s <plik.mylang>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Nie można otworzyć pliku: %s\n", argv[1]);
        return 1;
    }

    char src[1024] = {0};
    fread(src, 1, sizeof(src) - 1, file);
    fclose(file);

    lex(src);
    // Wypisz wszystkie tokeny
    for (int i = 0; i < token_count; i++) {
        printf("Token %d: typ=%d, wartosc='%s'\n", i, tokens[i].type, tokens[i].text);
    }
    parse();
    return 0;
}