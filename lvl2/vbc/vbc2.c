#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

typedef struct	node {
	enum {
		ADD,
		MULTI,
		VAL} type;
		int	val;
		struct node	*l;
		struct node *r;
} node;

node	*parse_val(char **s);
node	*parse_multi(char **s);
node	*parse_add(char **s);

node *new_node(int type, int val, node *l, node *r)
{
	node	*ret = calloc(1, sizeof(node));
	if (!ret)
		return (NULL);
	ret->type = type;
	ret->val = val;
	ret->l = l;
	ret->r = r;
	return (ret);
}

void	destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");
}

int	accept (char **s, char c)
{
	if (**s == c)
	{
		(*s)++;
		return (1);
	}
	return (0);
}

node	*parse_val(char **s)
{
	node	*ret = NULL;

	if (accept(s, '('))
	{
		ret = parse_add(s);
		if (!ret)
			return (NULL);
		if (!accept(s, ')'))
		{
			destroy_tree(ret);
			if (**s)
				return (unexpected(**s), NULL);
			else
				return (unexpected(0), NULL);
		}
	}
	else if (isdigit(**s))
	{
		ret = new_node(VAL, **s - '0', NULL, NULL);
		if (!ret)
			return (NULL);
		(*s)++;
	}
	else if (**s)
		return (unexpected(**s), NULL);
	else
		return (unexpected(0), NULL);
	return (ret);
}

node	*parse_multi(char **s)
{
	node	*left = NULL;
	node	*right = NULL;
	node	*tmp = NULL;

	left = parse_val(s);
	if (!left)
		return (NULL);
	while (accept(s, '*'))
	{
		right = parse_val(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node(MULTI, -1, left, right);
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
		left = tmp;
	}
	return (left);
}

node	*parse_add(char **s)
{
	node	*left = NULL;
	node	*right = NULL;
	node	*tmp = NULL;

	left = parse_multi(s);
	if (!left)
		return (NULL);
	while (accept(s, '+'))
	{
		right = parse_multi(s);
		if (!right)
			return (destroy_tree(left), NULL);
		tmp = new_node(ADD, -1, left, right);
		if (!tmp)
			return (destroy_tree(left), destroy_tree(right), NULL);
		left = tmp;
	}
	return (left);
}

node	*parse_expr(char *s)
{
	node	*ret = NULL;

	ret = parse_add(&s);
	if (!ret)
		return (NULL);
	if (*s)
	{
		unexpected(*s);
		destroy_tree(ret);
		return (NULL);
	}
	return (ret);
}

int	eval_tree(node *tree)
{
	switch (tree->type)
	{
		case ADD:
			return (eval_tree(tree->l) + eval_tree(tree->r));
		case MULTI:
			return (eval_tree(tree->l) * eval_tree(tree->r));
		case VAL:
			return (tree->val);
	}
	return (printf("Wrong type in tree?\n"), 0);
}

int	main(int ac, char **av)
{
	if (ac != 2)
		return (1);
	node *tree = parse_expr(av[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
