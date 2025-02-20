#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define TABLE_SIZE 100

#define TOKEN_H

typedef struct token
{
    char token_name[50];
    int row;
    int col;
    char type[20];
    int index;

} token;

typedef struct Symbol
{
    int serial_number;
    char lex_name[50];
    char type[20];
    int size;
    char argument[50];
    int is_occupied;
} Symbol;

typedef struct
{
    Symbol table[TABLE_SIZE];
} SymbolTable;

SymbolTable main_symbol_table;

int checkspecialsymbols(char n, char next, token *t, int rn, int cn)
{
    t->row = rn;
    t->col = cn;
    t->index = 0;

    // Multi-character operators
    if (n == '+' && next == '+')
    {
        strcpy(t->type, "increment_operator");
        strcpy(t->token_name, "++");
        return 2;
    }
    if (n == '-' && next == '-')
    {
        strcpy(t->type, "decrement_operator");
        strcpy(t->token_name, "--");
        return 2;
    }
    if (n == '=' && next == '=')
    {
        strcpy(t->type, "comparison_operator");
        strcpy(t->token_name, "==");
        return 2;
    }
    if (n == '!' && next == '=')
    {
        strcpy(t->type, "comparison_operator");
        strcpy(t->token_name, "!=");
        return 2;
    }
    if (n == '>' && next == '=')
    {
        strcpy(t->type, "comparison_operator");
        strcpy(t->token_name, ">=");
        return 2;
    }
    if (n == '<' && next == '=')
    {
        strcpy(t->type, "comparison_operator");
        strcpy(t->token_name, "<=");
        return 2;
    }
    if (n == '&' && next == '&')
    {
        strcpy(t->type, "logical_operator");
        strcpy(t->token_name, "&&");
        return 2;
    }
    if (n == '|' && next == '|')
    {
        strcpy(t->type, "logical_operator");
        strcpy(t->token_name, "||");
        return 2;
    }

    // Single-character operators
    if (n == '+' || n == '-' || n == '*' || n == '/' || n == '%')
    {
        strcpy(t->type, "math_operator");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == '&' || n == '|' || n == '^' || n == '~')
    {
        strcpy(t->type, "bitwise_operator");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == '=')
    {
        strcpy(t->type, "assignment_operator");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == '?')
    {
        strcpy(t->type, "ternary_operator");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == ':')
    {
        strcpy(t->type, "ternary_operator");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == ';')
    {
        strcpy(t->type, "punctuation");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }
    if (n == '{' || n == '}' || n == '(' || n == ')' || n == '[' || n == ']')
    {
        strcpy(t->type, "bracket");
        t->token_name[0] = n;
        t->token_name[1] = '\0';
        return 1;
    }

    // Not a special symbol
    return 0;
}
static const char *keywords[] = {
    "auto", "break", "case", "char", "continue", "do", "default", "const",
    "double", "else", "enum", "extern", "for", "if", "goto", "float", "int",
    "long", "register", "return", "signed", "static", "sizeof", "short",
    "struct", "switch", "typedef", "union", "void", "while", "volatile",
    "unsigned"};

int isakeyword(const char *word)
{
    for (int i = 0; i < 32; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void add_token(token *tt, int *tp, char *word, char *type, int r, int c)
{
    strcpy(tt[*tp].token_name, word);
    strcpy(tt[*tp].type, type);
    tt[*tp].row = r;
    tt[*tp].col = c;
    tt[*tp].index = *tp;

    (*tp)++;
}

int getnexttoken(FILE *fp, token *tt, int *tp)
{
    char word[50];
    static int k = 0, rn = 0, cn = -1;
    static char n, next;
    int x = fread(&n, 1, 1, fp);
    while (x)
    {
        cn++;
        if (isalpha(n) || n == '_')
        {

            word[k++] = n;
        }
        else if (isdigit(n))
        {

            word[k++] = n;
            while (fread(&n, 1, 1, fp) && isdigit(n))
            {
                word[k++] = n;
            }
            word[k] = '\0';
            add_token(tt, tp, word, "Numeric Literal", rn, cn);
            fseek(fp, -1, SEEK_CUR);
            k = 0;
            return 1;
        }

        else if (k > 0)
        {
            word[k] = '\0';

            if (isakeyword(word))
            {
                add_token(tt, tp, word, "keyword", rn, cn - k - 1);
            }
            else
            {

                add_token(tt, tp, word, "identifier", rn, cn - k - 1);
                fseek(fp, -1, SEEK_CUR);
                cn--;
            }
            k = 0;

            return 1;
        }

        else if (n == '#')
        {
            while (fread(&n, 1, 1, fp) && n != '\n')
                ;
        }
        else if (n == '/')
        {
            if (fread(&n, 1, 1, fp) && n == '/')
                while (fread(&n, 1, 1, fp) && n != '\n')
                    ;
        }
        else if (n == '/')
        {
            if (fread(&n, 1, 1, fp) && n == '*')
                while (fread(&n, 1, 1, fp) && n != '*')
                    ;
            fread(&n, 1, 1, fp);
        }

        else if (n == '"')
        {
            word[k++] = n;
            while (fread(&n, 1, 1, fp) && n != '"')
            {
                word[k++] = n;
            }
            word[k++] = '"';
            word[k] = '\0';
            add_token(tt, tp, word, "literal string", rn, cn);
            k = 0;
            return 1;
        }
        else if (n == '\n')
        {
            rn++;
            cn = 0;
            return 1;
        }
        else
        {
            token t;

            if (fread(&next, 1, 1, fp))
            {
                int result = checkspecialsymbols(n, next, &t, rn, cn);
                if (result == 2)
                {
                    add_token(tt, tp, t.token_name, t.type, rn, cn);
                    cn++;
                    return 1;
                }
                else if (result == 1)
                {
                    add_token(tt, tp, t.token_name, t.type, rn, cn - 1);
                    fseek(fp, -1, SEEK_CUR);

                    return 1;
                }
                else
                {
                    fseek(fp, -1, SEEK_CUR);

                    return 1;
                }
            }
            else
            {

                if (checkspecialsymbols(n, '\0', &t, rn, cn))
                {
                    add_token(tt, tp, t.token_name, t.type, rn, cn - 1);
                    return -1;
                }
            }
        }

        x = fread(&n, 1, 1, fp);
    }

    return -1;
}

int search_symbol(SymbolTable *st, const char *lex_name)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (st->table[i].is_occupied && strcmp(st->table[i].lex_name, lex_name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int insert_symbol(SymbolTable *st, const char *lex_name, const char *type, int size, char *argument)
{
    if (search_symbol(st, lex_name) != -1)
    {
        return -2;
    }
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (!st->table[i].is_occupied)
        {
            st->table[i].serial_number = i + 1;
            strcpy(st->table[i].lex_name, lex_name);
            strcpy(st->table[i].type, type);
            st->table[i].size = size;
            strcpy(st->table[i].argument, argument);
            st->table[i].is_occupied = 1;
            return 0;
        }
    }
    return -1;
}

int get_type_size(const char *type)
{
    if (strcmp(type, "int") == 0)
        return sizeof(int);
    if (strcmp(type, "float") == 0)
        return sizeof(float);
    if (strcmp(type, "double") == 0)
        return sizeof(double);
    if (strcmp(type, "char") == 0)
        return sizeof(char);
    return -1;
}

int main()
{
    FILE *fr, *fw1, *fw2;

    token tt[100];
    int tp = 0;

    fw1 = fopen("example.c", "r");
    while (getnexttoken(fw1, tt, &tp) != -1)
        ;
    fclose(fw1);

    int in_main = 0;
    for (int i = 0; i < tp; i++)
    {
        printf("Tokens are : %s of type : %s  row : %d  col : %d\n", tt[i].token_name, tt[i].type, tt[i].row, tt[i].col);
    }

    for (int i = 0; i < tp; i++)
    {
        token current_token = tt[i];

        if (strcmp(current_token.type, "keyword") == 0)
        {
            char *var_type = current_token.token_name;

            while (strcmp(tt[i + 1].type, "identifier") == 0)
            {
                char *var_name = tt[i + 1].token_name;

                if (strcmp(tt[i + 2].token_name, "(") == 0)
                {
                    char argument[100];
                    memset(argument, 0, sizeof(argument));
                    i += 2;
                    while (strcmp(tt[i].token_name, ")") != 0)
                    {
                        if (strcmp(tt[i].type, "identifier") == 0)
                        {
                            strcat(argument, tt[i].token_name);
                            strcat(argument, " , ");
                        }

                        argument[strlen(argument)] = '\0';

                        i++;
                    }
                    int var_size = get_type_size(var_type);
                    insert_symbol(&main_symbol_table, var_name, var_type, var_size, argument);
                    memset(argument, 0, sizeof(argument));
                }

                else if (strcmp(tt[i + 2].token_name, "[") == 0)
                {
                    i += 2;
                    int var_size = get_type_size(var_type) * atoi(tt[i + 1].token_name);
                    insert_symbol(&main_symbol_table, var_name, var_type, var_size, "");
                    while (strcmp(tt[i].token_name, "]") != 0)
                    {
                        i++;
                    };
                    i -= 1;
                }
                else
                {

                    int var_size = get_type_size(var_type);
                    insert_symbol(&main_symbol_table, var_name, var_type, var_size, "");
                }

                i += 1;
            }
        }
    }

    printf("\nSymbol Table for main():\n");
    printf("SN\tlex\ttype\tsize\targument\n");
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (main_symbol_table.table[i].is_occupied)
        {
            printf("%d\t%s\t%s\t%d\t%s\n",
                   main_symbol_table.table[i].serial_number,
                   main_symbol_table.table[i].lex_name,
                   main_symbol_table.table[i].type,
                   main_symbol_table.table[i].size,
                   main_symbol_table.table[i].argument);
        }
    }

    return 0;
}
