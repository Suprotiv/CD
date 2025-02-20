#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 100
#define MAX_TOKENS 1000

typedef struct token
{
    char token_name[MAX_TOKEN_SIZE];
    int row;
    int col;
    char type[50];
} token;

token tokens[MAX_TOKENS];
int token_index = 0;

void add_token(char *name, char *type, int row, int col)
{
    strcpy(tokens[token_index].token_name, name);
    strcpy(tokens[token_index].type, type);
    tokens[token_index].row = row;
    tokens[token_index].col = col;
    token_index++;
}

int is_tag(char ch)
{
    return ch == '<' || ch == '>';
}

int is_whitespace(char ch)
{
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r';
}

int is_alphanumeric(char ch)
{
    return isalpha(ch) || isdigit(ch) || ch == '-';
}

void parse_html(FILE *file)
{
    char ch, prev_ch = '\0', token_buffer[MAX_TOKEN_SIZE];
    int row = 1, col = 0, buffer_index = 0;
    int in_tag = 0, in_attr = 0, in_comment = 0;

    while ((ch = fgetc(file)) != EOF)
    {
        col++;

        if (ch == '\n')
        {
            row++;
            col = 0;
        }

        if (ch == '<')
        {
            // Detect comments
            if (fgetc(file) == '!' && fgetc(file) == '-' && fgetc(file) == '-')
            {
                // Begin comment
                in_comment = 1;
                add_token("<!--", "comment_start", row, col);
                col += 3;
                while ((ch = fgetc(file)) != EOF)
                {
                    col++;
                    if (ch == '\n')
                    {
                        row++;
                        col = 0;
                    }
                    if (ch == '-' && fgetc(file) == '-' && fgetc(file) == '>')
                    {
                        add_token("-->", "comment_end", row, col);
                        col += 2;
                        in_comment = 0;
                        break;
                    }
                }
            }
            else
            {
                // Detect opening or closing tag
                in_tag = 1;
                add_token("<", "tag_start", row, col);
                fseek(file, -1, SEEK_CUR);
            }
        }
        else if (ch == '>')
        {
            in_tag = 0;
            token_buffer[buffer_index] = '\0';
            if (buffer_index > 0)
            {
                add_token(token_buffer, "tag", row, col - buffer_index);
                buffer_index = 0;
            }
            add_token(">", "tag_end", row, col);
        }
        else if (in_tag && (isalpha(ch) || isdigit(ch) || ch == '/' || ch == '-'))
        {
            // Capture tag names or attributes
            token_buffer[buffer_index++] = ch;
        }
        else if (in_tag && is_whitespace(ch))
        {
            if (buffer_index > 0)
            {
                token_buffer[buffer_index] = '\0';
                if (prev_ch == '<' || prev_ch == '/')
                {
                    add_token(token_buffer, "tag", row, col - buffer_index);
                }
                else
                {
                    add_token(token_buffer, "attribute_name", row, col - buffer_index);
                }
                buffer_index = 0;
            }
        }
        else if (ch == '=' && in_tag)
        {
            // Attribute value assignment
            add_token("=", "assignment", row, col);
            buffer_index = 0;
        }
        else if (ch == '"' && in_tag)
        {
            // Attribute value in quotes
            buffer_index = 0;
            while ((ch = fgetc(file)) != EOF && ch != '"')
            {
                token_buffer[buffer_index++] = ch;
                col++;
            }
            token_buffer[buffer_index] = '\0';
            add_token(token_buffer, "attribute_value", row, col - buffer_index);
            buffer_index = 0;
        }
        else if (!in_tag && !is_whitespace(ch))
        {
            // Capture text node
            token_buffer[buffer_index++] = ch;
        }
        else if (!in_tag && is_whitespace(ch) && buffer_index > 0)
        {
            // Add text node as a token
            token_buffer[buffer_index] = '\0';
            add_token(token_buffer, "text", row, col - buffer_index);
            buffer_index = 0;
        }

        prev_ch = ch;
    }
}

void print_tokens()
{
    printf("Tokens:\n");
    for (int i = 0; i < token_index; i++)
    {
        printf("Token: %-15s Type: %-20s Row: %d Col: %d\n", tokens[i].token_name, tokens[i].type, tokens[i].row, tokens[i].col);
    }
}

int main()
{
    FILE *html_file = fopen("index.html", "r");
    if (!html_file)
    {
        printf("Failed to open HTML file.\n");
        return 1;
    }

    parse_html(html_file);
    fclose(html_file);

    print_tokens();

    return 0;
}
