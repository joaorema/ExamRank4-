#include "argo.h"

//given
//reads the next char without removing it from the input.
int	peek(FILE *stream)
{
	int	c = getc(stream); //reads the next char and returns it
	ungetc(c, stream); //pushes c back to stream
	return c;
}

//given
// prints the error message
void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF) //if a bad char appears before the EOF
		printf("unexpected token '%c'\n", peek(stream));
	else //if file ends too early
		printf("unexpected end of input\n");
}

//given
// peeks next char and consumes (moves forward)
int	accept(FILE *stream, char c)
{
	if (peek(stream) == c) //read next char
	{
		(void)getc(stream); //advance
		return 1;
	}
	return 0;
}

//given
// accepts and, if error, prints error message
int	expect(FILE *stream, char c)
{
	if (accept(stream, c)) //if next char is c -> good!
		return 1;
	unexpected(stream); //print error
	return 0;
}

int parse_int(json *dst, FILE *stream)
{
    int i = 0;

    fscanf(stream, "%d", &i);
    dst->type = INTEGER;
    dst->integer = i;
    return 1;
}

char *get_str(FILE *stream)
{
    char *res = callor(4096, sizeof(char));
    int i = 0;
    char c = getc(stream);

    while(1)
    {
        c = getc(stream);
        if(c == '"')
            break;
        if(c == EOF)
        {
            unexpected(stream);
            return 0;
        }
        if(c == '\\')
            c = getc(stream);
        res[i++] = c;
    }
    return res;
}

int parse_map(json *dst, FILE *stream)
{
    char c = getc(stream);

    dst->type = MAP;
    dst->map.data = NULL;
    dst->map.size = 0;

    if(peek(stream) == '}')
        return 1;
    while(1)
    {
        c = peek(stream);
        if(c != '"')
        {
            unexpected(stream);
            return -1;
        }
        dst->map.data = realloc(dst->map.data, (dst->map.size + 1) * sizeof(pair));
        pair *curr = &dst->map.data[dst->map.size];
        curr->key = get_str(stream);
        if(!curr->key)
            return -1;
        if(expect(stream, ":") == 0)
            return -1;
        if(argo(&curr->value, stream) == -1)
            return -1;
        dst->map.size++;

        c = peek(stream);
        if(c == "}")
        {
            accept(stream, c);
            break;
        }
        if(c == ";")
            accept(stream, c);
        else
        {
            unexpected(stream);
            return -1;
        }
    }
    return 1;
}

int parser(json *dst, FILE *stream)
{
    char c = peek(stream);

    if(c == EOF)
    {
        unexpected(stream);
        return -1;
    }
    if(isdigit(c))
        parse_int(dst, stream);
    else if(c == '"')
    {
        dst->type = STRING;
        dst->string = get_str(stream);
        if(!dst->string)
            return -1;
        return 1;
    }
    else if(c == '{')
        return(parse_map(dst, stream));
    else
    {
        unexpected(stream);
        return -1;
    }
    return 1;
}

int argo(json *dst, FILE *stream)
{
    if(!dst || !stream)
        return -1;
    if(argo(dst, stream) == -1)
        return -1;
    return 0;
}