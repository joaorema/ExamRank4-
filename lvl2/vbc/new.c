#include "vbc.h"

//given
node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n)); //creates a new node and allocates memory
    if (!ret)
        return (NULL);
    *ret = n; //copies contents from temporary n to the new node
    return (ret); //returns pointer to the new node
}

//given
void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL) //if ADD or MULTI
    {
        destroy_tree(n->l); //frees recursively left
        destroy_tree(n->r); //frees recursively left
    }
    free(n);
}

//given
void    unexpected(char c) //handling syntax errors
{
    if (c) //if not null - print token
        printf("Unexpected token '%c'\n", c);
    else //if null, problem with end
        printf("Unexpected end of input\n"); //changed file to input to agree with sbject
}

//given
// int accept(char **s, char c) //not used in my version
// {
//     if (**s == c) //changed if (**s) to if(**s == c)
//     {
//         (*s)++;
//         return (1);
//     }
//     return (0);
// }

// //given
// int expect(char **s, char c) //not used in my version
// {
//     if (accept(s, c))
//         return (1);
//     unexpected(**s); //prints error if c is not found
//     return (0);
// }

//my input from here onwards
int	ch_balance(char *s) //looks for imbalanced number of parethisis
{
	int balance = 0;
	int i = -1;

	while (s[++i])
	{
		if (s[i] == '(') // +1 for each (
			balance++;
		else if(s[i] == ')') // -1 for each (
		{
			balance--;
			if (balance < 0) //if less than 0 - imbalanced parenthisis
				return (-1);
		}
	}
	return (balance);
}

node	*parse_nb_or_group(char **s) //base case - checks if we have a number or a grouped expression
{
	node	*res = NULL; //final result
	node	tmp; //tmp to turn into node

	if(**s == '(') //checks for grouped expression (like (3+4*5))
	{
		(*s)++; //moves past the '('
		res = parse_add(s); //addition is top level operator - first call
		if(!res || **s != ')') //if NULL or there wasn't a closing ')' -> error
		{
			destroy_tree(res); //cleans
			unexpected(**s); //prints error
			return (NULL); //exit
		}
		(*s)++; //moves passed the closing ')'
		return (res); //returns the subtree built for what is inside the ()
	}
	if (isdigit(**s)) //if digit between 0 and 9
	{
		tmp.type = VAL; //attribute type
		tmp.val = **s - '0'; //in ASCII we subtract 48 ('0') //attribute value
		res = new_node(tmp); //create new node
		if (!res)
			return (NULL);
		(*s)++;
		return (res);
	}
	unexpected(**s); //if not ( or digit it's an error
	return (NULL);
}

node	*parse_mult(char **s)
{
	node *l; //left
	node *r; //right
	node tmp; //tmp to turn into node

	l = parse_nb_or_group(s); //find out if there is a left-hand value (digit or group)
	if (!l)
		return (NULL);
	while (**s == '*') //keep building multi nodes while current is *
	{
		(*s)++; //move past *
		r = parse_nb_or_group(s); //find out if there is a right-hand value (digit or group)
		if (!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		tmp.type = MULTI; //attribute type
		tmp.l = l; //attribute left
		tmp.r = r; //attribute right
		l = new_node(tmp); //store in k to continue left-associative chaiining - eg. 2*3*4 becomes MULTI(MULTI(2, 3), 4)
	}
	return (l); //return new node
}

node	*parse_add(char **s)
{
	node *l; //left
	node *r; //right
	node tmp; //tmp to create new node

	l = parse_mult(s); //respect precedence
	if(!l)
		return (NULL);
	while(**s == '+') //while we have +
	{
		(*s)++; //move passed the '+'
		r = parse_mult(s); //get right hand operand
		if(!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		tmp.type = ADD; //attribute type
		tmp.l = l; //attribute left
		tmp.r = r; //attribute right
		l = new_node(tmp); //create new node and store in left -> 1+2+3 → ADD(ADD(1,2),3)
	}
	return (l); //return new node
}

//given
int eval_tree(node *tree) //to perform the operation
{
    switch (tree->type) //recursively evaluates tree
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r)); //returns sum
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r)); //returns multi
        case VAL:
            return (tree->val); //returns number
    }
	return (0);
}

//given
int main(int argc, char **argv)
{
	char *input = argv[1]; //added
	node *tree;

	if (argc != 2)
        return (1);
	if (ch_balance(argv[1]) == -1) //added //if -1, too many )
		return (printf("Unexpected token ')'"), 1); //added
	tree = parse_add(&input); //changed argv[1] to &input
	if (!tree)
        return (1);
	if (*input) // to check if there is any symbols after the operation (like '3+5x')
	{
		unexpected(*input); //print error
		destroy_tree(tree); //clean 
		return (1);
	}
    printf("%d\n", eval_tree(tree)); //print result
    destroy_tree(tree); //clean
}
