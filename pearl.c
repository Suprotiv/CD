#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure for Tokens
struct Token
{
    char lexeme[64];
    char type[20];
};

// Symbol Table (to store variables and functions)
struct Symbol
{
    char name[64];
    char type[20];
} symbolTable[100];

int symbolCount = 0;

char keywords[][10] = {"my", "sub", "print", "foreach", "if", "else"};
char *operators[] = {"+=", "=", "+", "-", "*", "/", "eq", "ne"};
char *specialSymbols = "$@%(){},;";

// Function to check if a word is a keyword
int isKeyword(char *word)
{
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// Function to check if a word is an operator
int isOperator(char *word)
{
    for (int i = 0; i < 8; i++)
    {
        if (strcmp(word, operators[i]) == 0)
            return 1;
    }
    return 0;
}

// Function to check if a character is a special symbol
int isSpecialSymbol(char c)
{
    return strchr(specialSymbols, c) != NULL;
}

// Function to add an entry to the symbol table
void addToSymbolTable(char *name, char *type)
{
    strcpy(symbolTable[symbolCount].name, name);
    strcpy(symbolTable[symbolCount].type, type);
    symbolCount++;
}

// Function to print the symbol table
void printSymbolTable()
{
    printf("\nSymbol Table:\n");
    printf("----------------------\n");
    for (int i = 0; i < symbolCount; i++)
    {
        printf("%s -> %s\n", symbolTable[i].name, symbolTable[i].type);
    }
}

// Function to get the next token from the file
struct Token getNextToken(FILE *fin)
{
    struct Token token;
    int c;

    while ((c = fgetc(fin)) != EOF)
    {
        if (isspace(c))
            continue;

        if (c == '#')
        { // Ignore comments
            while ((c = fgetc(fin)) != '\n' && c != EOF)
                ;
            continue;
        }

        if (isSpecialSymbol(c))
        { // Handle Perl variables and symbols
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            strcpy(token.type, "SpecialSymbol");
            return token;
        }

        if (isdigit(c))
        { // Handle numbers
            int i = 0;
            token.lexeme[i++] = c;
            while (isdigit(c = fgetc(fin)))
                token.lexeme[i++] = c;
            token.lexeme[i] = '\0';
            strcpy(token.type, "Number");
            fseek(fin, -1, SEEK_CUR);
            return token;
        }

        if (isalpha(c) || c == '_')
        { // Handle keywords, identifiers, functions
            int i = 0;
            token.lexeme[i++] = c;
            while (isalnum(c = fgetc(fin)) || c == '_')
                token.lexeme[i++] = c;
            token.lexeme[i] = '\0';
            fseek(fin, -1, SEEK_CUR);

            if (isKeyword(token.lexeme))
                strcpy(token.type, "Keyword");
            else
            {
                strcpy(token.type, "Identifier");
                addToSymbolTable(token.lexeme, "Variable");
            }
            return token;
        }

        if (c == '"')
        { // Handle string literals
            int i = 0;
            token.lexeme[i++] = c;
            while ((c = fgetc(fin)) != '"' && c != EOF)
                token.lexeme[i++] = c;
            token.lexeme[i++] = '"';
            token.lexeme[i] = '\0';
            strcpy(token.type, "StringLiteral");
            return token;
        }

        if (ispunct(c))
        { // Handle operators
            char op[3] = {c, '\0', '\0'};
            int d = fgetc(fin);
            op[1] = d;

            if (isOperator(op))
            {
                strcpy(token.lexeme, op);
                strcpy(token.type, "Operator");
            }
            else
            {
                fseek(fin, -1, SEEK_CUR);
                token.lexeme[0] = c;
                token.lexeme[1] = '\0';
                strcpy(token.type, "Operator");
            }
            return token;
        }
    }

    strcpy(token.lexeme, "EOF");
    strcpy(token.type, "EndOfFile");
    return token;
}

int main()
{
    FILE *fin = fopen("example.pl", "r");
    if (!fin)
    {
        printf("Error opening file!\n");
        return 1;
    }

    struct Token token;
    while (strcmp((token = getNextToken(fin)).type, "EndOfFile") != 0)
    {
        printf("<%s, %s>\n", token.lexeme, token.type);
    }

    printSymbolTable();
    fclose(fin);

    return 0;
}
