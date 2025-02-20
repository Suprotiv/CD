#include <stdio.h>

#include <stdlib.h> // For exit()

#include <string.h>

#include <ctype.h>

// Define a structure to represent a token

struct token
{

    char token_name[100]; // Token value

    int index; // Token index (could be used for debugging or identifying position)

    unsigned int row, col; // Line and column numbers

    char type[20]; // Type of the token (e.g., operator, keyword, etc.)
};

int isKeyword(char *str);

struct token getNextToken(FILE *fp);

int in_symbol_table(struct token key, struct token arr[], int len)
{

    for (int i = 0; i < len; i++)
    {

        if (strcmp(key.token_name, arr[i].token_name) == 0)

            return 1;
    }

    return 0;
}

int main()
{

    struct token symbol_table[100];

    struct token token_table[100];

    int len = 0, tok_len = 0;

    FILE *fptr1, *fptr2;

    char filename[100], c;

    fptr1 = fopen("example.c", "r");

    fptr2 = fopen("temp", "w+");

    c = fgetc(fptr1);

    while (c != EOF)
    {

        if (c == '#' || c == '{')
        {

            while (c != '\n' && c != EOF)
            {

                c = fgetc(fptr1);
            }

            if (c == '\n')
            {

                fputc(c, fptr2);
            }

            c = fgetc(fptr1);

            continue;
        }

        fputc(c, fptr2);

        c = fgetc(fptr1);
    }

    fclose(fptr2);

    FILE *fp = fopen("temp", "r");

    if (fp == NULL)
    {

        printf("Cannot open file\n");

        exit(0);
    }

    struct token current_token;

    // Process tokens from file until EOF

    while (1)
    {

        current_token = getNextToken(fp);

        // If token_name is empty, it indicates EOF

        if (strlen(current_token.token_name) == 0)

            break;

        token_table[tok_len++] = current_token;

        if (strcmp(current_token.type, "Identifier") == 0)
        {

            if (!in_symbol_table(current_token, symbol_table, len))
            {

                symbol_table[len++] = current_token;

                int iter = tok_len - 1;

                printf("%s %s ", current_token.token_name, current_token.type);

                for (iter = tok_len - 1; iter > 0; iter--)
                {

                    // printf("%s <%s,%d,%d>\n",token_table[iter].token_name, token_table[iter].type, token_table[iter].row, token_table[iter].col);

                    if (strcmp(token_table[iter].type, "Keyword") == 0)
                    {

                        break;
                    }
                }

                printf("%s ", token_table[iter].token_name);

                if (strcmp(token_table[iter].token_name, "int") == 0)

                    printf("%d \n", sizeof(int));

                if (strcmp(token_table[iter].token_name, "char") == 0)

                    printf("%d \n", sizeof(char));

                if (strcmp(token_table[iter].token_name, "bool") == 0)

                    printf("1\n");

                // printf("%d \n",sizeof(int));
            }
        }

        // Print the token type in the requested format

        // printf("%s <%s,%d,%d>\n",current_token.token_name, current_token.type, current_token.row, current_token.col);
    }

    fclose(fp);

    // for(int i=0;i<len;i++)

    // printf("%s <%s,%d,%d>\n",symbol_table[i].token_name, symbol_table[i].type, symbol_table[i].row, symbol_table[i].col);

    return 0;
}

// Function to check if a string is a keyword

int isKeyword(char *str)
{

    const char *keywords[] = {

        "auto", "default", "signed", "enum", "extern", "for", "register", "if", "scanf",

        "else", "int", "while", "do", "break", "continue", "double", "float",

        "return", "char", "case", "const", "sizeof", "long", "short", "typedef", "void",

        "switch", "unsigned", "void", "static", "struct", "goto", "union", "volatile", "bool", "printf", "main",

        NULL

    };

    for (int i = 0; keywords[i] != NULL; i++)
    {

        if (strcmp(str, keywords[i]) == 0)
        {

            return 1; // It's a keyword
        }
    }

    return 0; // Not a keyword
}

// Function to extract and return the next token from the file

struct token getNextToken(FILE *fp)
{

    struct token current_token;

    char c;

    int row = 1, col = 1;

    int i = 0;

    current_token.token_name[0] = '\0'; // Clear token name

    current_token.type[0] = '\0'; // Clear token type

    c = fgetc(fp);

    while (c != EOF)
    {

        // Skip over single-line comments

        if (c == '/' && (c = fgetc(fp)) == '/')
        {

            while (c != '\n' && c != EOF)
            {

                c = fgetc(fp);
            }

            if (c == '\n')
            {

                row++;

                col = 1;
            }

            c = fgetc(fp);

            continue;
        }

        // Skip over multi-line comments

        if (c == '/' && (c = fgetc(fp)) == '*')
        {

            while (c != EOF)
            {

                c = fgetc(fp);

                if (c == '*' && (c = fgetc(fp)) == '/')
                {

                    break;
                }
            }

            c = fgetc(fp);

            continue;
        }

        // Skip over string literals

        if (c == '"')
        {

            while ((c = fgetc(fp)) != '"' && c != EOF)
            {

                if (c == '\n')
                {

                    row++;

                    col = 1;
                }
            }

            c = fgetc(fp);

            continue;
        }

        // Skip white spaces

        if (isspace(c))
        {

            if (c == '\n')
            {

                row++;

                col = 1;
            }

            c = fgetc(fp);

            continue;
        }

        // Process arithmetic operators (e.g., '+', '-', '*', '/', '%')

        if (c == '+')
        {

            char next = fgetc(fp);

            if (next == '+')
            {

                // It's a unary operator (++)

                current_token.token_name[i++] = c;

                current_token.token_name[i++] = next;

                current_token.token_name[i] = '\0';

                strcpy(current_token.type, "Unary Operator");

                current_token.row = row;

                current_token.col = col;

                col += i;

                return current_token;
            }
            else
            {

                // It's an arithmetic operator (+)

                current_token.token_name[i++] = c;

                current_token.token_name[i] = '\0';

                strcpy(current_token.type, "Arithmetic Operator");

                current_token.row = row;

                current_token.col = col;

                col += i;

                ungetc(next, fp); // Put the character back for next token processing

                return current_token;
            }
        }

        // Process other arithmetic operators

        else if (c == '-' || c == '*' || c == '/' || c == '%')
        {

            current_token.token_name[i++] = c;

            current_token.token_name[i] = '\0';

            strcpy(current_token.type, "Arithmetic Operator");

            current_token.row = row;

            current_token.col = col;

            col += i;

            return current_token;
        }

        // Process relational operators (e.g., '==', '!=', '<=', '>=')

        else if (c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|')
        {

            current_token.token_name[i++] = c;

            current_token.token_name[i] = '\0';

            // Handle multi-character relational operators

            if (c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|')
            {

                char next = fgetc(fp);

                if ((c == '=' && (next == '=' || next == '>')) ||

                    (c == '!' && next == '=') ||

                    (c == '<' && (next == '=' || next == '>')) ||

                    (c == '>' && next == '=') ||

                    (c == '&' && next == '&') ||

                    (c == '|' && next == '|'))
                {

                    current_token.token_name[i++] = next;

                    current_token.token_name[i] = '\0';

                    c = fgetc(fp);
                }
                else
                {

                    ungetc(next, fp);
                }
            }

            strcpy(current_token.type, "Relational Operator");

            current_token.row = row;

            current_token.col = col;

            col += i;

            return current_token;
        }

        // Process logical operators

        else if (c == '&' || c == '|')
        {

            current_token.token_name[i++] = c;

            current_token.token_name[i] = '\0';

            // Handle logical operators (e.g., '&&', '||')

            if (c == '&' || c == '|')
            {

                char next = fgetc(fp);

                if (c == next)
                {

                    current_token.token_name[i++] = next;

                    current_token.token_name[i] = '\0';

                    c = fgetc(fp);
                }
                else
                {

                    ungetc(next, fp);
                }
            }

            strcpy(current_token.type, "Logical Operator");

            current_token.row = row;

            current_token.col = col;

            col += i;

            return current_token;
        }

        // Process negation operator

        else if (c == '!')
        {

            current_token.token_name[i++] = c;

            current_token.token_name[i] = '\0';

            strcpy(current_token.type, "Logical Operator");

            current_token.row = row;

            current_token.col = col;

            col++;

            return current_token;
        }

        // Process identifiers and keywords

        else if (isalpha(c) || c == '_')
        {

            current_token.token_name[i++] = c;

            c = fgetc(fp);

            while (isalnum(c) || c == '_')
            {

                current_token.token_name[i++] = c;

                c = fgetc(fp);
            }

            current_token.token_name[i] = '\0';

            if (isKeyword(current_token.token_name))
            {

                strcpy(current_token.type, "Keyword");
            }
            else
            {

                strcpy(current_token.type, "Identifier");
            }

            current_token.row = row;

            current_token.col = col;

            col += i;

            return current_token;
        }

        // Process numbers (integers and floating-point)

        else if (isdigit(c))
        {

            current_token.token_name[i++] = c;

            c = fgetc(fp);

            while (isdigit(c))
            {

                current_token.token_name[i++] = c;

                c = fgetc(fp);
            }

            if (c == '.')
            {

                current_token.token_name[i++] = c;

                c = fgetc(fp);

                while (isdigit(c))
                {

                    current_token.token_name[i++] = c;

                    c = fgetc(fp);
                }
            }

            current_token.token_name[i] = '\0';

            strcpy(current_token.type, "Number");

            current_token.row = row;

            current_token.col = col;

            col += i;

            return current_token;
        }

        // Handle other individual characters (e.g., punctuation like ';', '(', ')', etc.)

        else
        {

            current_token.token_name[i++] = c;

            current_token.token_name[i] = '\0';

            strcpy(current_token.type, "Punctuation");

            current_token.row = row;

            current_token.col = col;

            col++;

            return current_token;
        }

        c = fgetc(fp);
    }

    // If we reach here, it means EOF, return an empty token to indicate the end

    current_token.token_name[0] = '\0';

    return current_token;
}