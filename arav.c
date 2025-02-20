#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct token
{
    char name[100];
    char type[100];
    int row, col;
};
struct symbol
{
    char name[100];
    char type[100];
    char arguements[100];
    int size;
};

struct token token_table[100];
struct symbol symbol_table[100];
static int symbol_table_iter = 0;
static int tok_tab_iter = 0;

void add_token(char name[50], char type[50], int row, int col)
{
    strcpy(token_table[tok_tab_iter].name, name);
    strcpy(token_table[tok_tab_iter].type, type);
    token_table[tok_tab_iter].row = row;
    token_table[tok_tab_iter++].col = col;
}
int iskeyword(char *str)
{
    const char *keywords[] = {
        "auto", "default", "signed", "enum", "extern", "for", "register", "if", "scanf",
        "else", "int", "while", "do", "break", "continue", "double", "float",
        "return", "char", "case", "const", "sizeof", "long", "short", "typedef", "void",
        "switch", "unsigned", "void", "static", "struct", "goto", "union", "volatile", "bool", "printf", "main",
        NULL};

    for (int i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1; // It's a keyword
        }
    }

    return 0; // Not a keyword
}
int in_symbol_table(char str[100])
{
    for (int i = 0; i < symbol_table_iter; i++)
        if (strcmp(str, symbol_table[i].name) == 0)
            return 1;
    return 0;
}

int getnexttoken(FILE *fptr)
{
    static int row = 0;
    static int col = 0;
    int i = 0;
    char word[50];
    char c = fgetc(fptr);
    col++;

    if (c == EOF)
    {
        return 999;
    }

    // String literal handling
    if (c == '"')
    {
        i = 0;
        word[i++] = c;
        c = fgetc(fptr);
        col++;

        while (c != '"' && c != EOF)
        {
            word[i++] = c;
            c = fgetc(fptr);
            col++;
        }

        word[i++] = '"'; // Include closing quote
        word[i] = '\0';

        add_token(word, "string literal", row, col - i + 1);
        return 1;
    }

    // Identifiers and keywords
    if (isalpha(c))
    {
        i = 0;
        while (isalpha(c))
        {
            word[i++] = c;
            c = fgetc(fptr);
            col++;
        }
        ungetc(c, fptr);
        col--;
        word[i] = '\0';

        if (iskeyword(word))
        {
            add_token(word, "keyword", row, col - i + 1);
        }
        else
        {
            add_token(word, "identifier", row, col - i + 1);
        }
        return 1;
    }

    // Numbers
    if (isdigit(c))
    {
        i = 0;
        while (isdigit(c))
        {
            word[i++] = c;
            c = fgetc(fptr);
            col++;
        }
        ungetc(c, fptr);
        col--;
        word[i] = '\0';

        add_token(word, "number", row, col - i + 1);
        return 1;
    }

    // Comments
    if (c == '/')
    {
        char next = fgetc(fptr);
        col++;
        if (next == '/')
        {
            while (next != '\n' && next != EOF)
            {
                next = fgetc(fptr);
            }
            row++;
            col = 0;
            return 0;
        }
        ungetc(next, fptr);
        col--;
    }

    // Handle newlines and spaces
    if (c == '\n')
    {
        row++;
        col = 0;
        return 0;
    }
    if (c == ' ')
    {
        return 0;
    }

    // Brackets
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']')
    {
        word[0] = c;
        word[1] = '\0';
        add_token(word, "bracket", row, col);
        return 1;
    }

    // Operators and relational symbols
    char next = fgetc(fptr);
    col++;

    if (c == '+' && next == '+')
    {
        add_token("++", "unary", row, col - 1);
        return 1;
    }
    if (c == '-' && next == '-')
    {
        add_token("--", "unary", row, col - 1);
        return 1;
    }
    if (c == '>' && next == '=')
    {
        add_token(">=", "relational", row, col - 1);
        return 1;
    }
    if (c == '<' && next == '=')
    {
        add_token("<=", "relational", row, col - 1);
        return 1;
    }
    if (c == '=' && next == '=')
    {
        add_token("==", "relational", row, col - 1);
        return 1;
    }

    ungetc(next, fptr);
    col--;

    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
    {
        word[0] = c;
        word[1] = '\0';
        add_token(word, "arithmetic", row, col);
        return 1;
    }
    if (c == '<' || c == '>' || c == '!')
    {
        word[0] = c;
        word[1] = '\0';
        add_token(word, "relational", row, col);
        return 1;
    }

    return 0;
}

int main()
{
    FILE *fptr;
    struct token temp;
    struct symbol curr;
    char arg[100];
    int j;
    fptr = fopen("example.c", "r");
    while (1)
    {
        int val = getnexttoken(fptr);
        if (val == 999)
            break;
        else if (val == 0)
            continue;
        else if (val == 1)
            printf("%d %s %s %d %d\n", tok_tab_iter - 1, token_table[tok_tab_iter - 1].name, token_table[tok_tab_iter - 1].type, token_table[tok_tab_iter - 1].row, token_table[tok_tab_iter - 1].col);
    }

    // Creation of symbol table;
    for (int i = 0; i < tok_tab_iter; i++)
    {
        strcpy(arg, "");
        j = i;
        if (strcmp(token_table[i].type, "identifier") == 0 && !in_symbol_table(token_table[i].name))
        {
            while (j > 0 && strcmp(token_table[j].type, "keyword") != 0)
            {
                j--;
            }
            strcpy(curr.name, token_table[i].name);
            strcpy(curr.type, token_table[j].name);
            if (strcmp(curr.type, "int") == 0)
                curr.size = sizeof(int);
            else if (strcmp(curr.type, "char") == 0)
                curr.size = sizeof(char);
            else if (strcmp(curr.type, "bool") == 0)
                curr.size = 1;
            else
                curr.size = -1;

            if (strcmp(token_table[i + 1].name, "(") == 0)
            {
                j = i + 2;

                while (strcmp(token_table[j].name, ")") != 0)
                {
                    strcat(arg, token_table[j].name);
                    strcat(arg, "  ");
                    j++;
                }

                strcat(curr.name, "()");
                curr.size = 0;
            }
            if (strcmp(token_table[i + 1].name, "[") == 0)
            {
                if (strcmp(token_table[i + 2].type, "number") == 0)
                {
                    curr.size *= atoi(token_table[i + 2].name);
                }
                strcat(curr.name, "[]");
                strcat(curr.type, " array");
            }
            strcpy(curr.arguements, arg);
            symbol_table[symbol_table_iter++] = curr;
        }
    }
    printf("%d", symbol_table_iter);
    printf("\nSymbol Table for main():\n");
    printf("SN\tlex\t\ttype\t\tsize\targument\n");
    for (int i = 0; i < symbol_table_iter; i++)
    {
        printf("%d\t%s\t\t%s\t\t%d\t%s\n",
               i,
               symbol_table[i].name,
               symbol_table[i].type,
               symbol_table[i].size,
               symbol_table[i].arguements);
    }
    return 0;
}