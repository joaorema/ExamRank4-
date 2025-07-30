#include "vbc.h"

int check_parenthesis(char *str)  //checks nbr of ().
{
    int balance = 0;
    int i = -1;

    while(str[++i])
    {
        if(str[i] == '(')
            balance++;
        else if(str[i] == ')')
        {
            balance--;
            if(balance < 0)
                return -1;
        }
    }
    return(balance);
}

node *parse_nbr_or_group(char **str)
{
    node *res = NULL;
    node temp;

    if(**str == '(')
    {
        (*str)++;
        res = parse_add(str);
        if(!str || **str != ')')
        {
            destroy_tree(res);
            unexpected(**str);
            return NULL;
        }
        (*str)++;
        return(res);
    }
    if(isdigit(**str))
    {
        temp.type = VAL;
        temp.val = (**str) - '0';
        res = new_node(temp);
        (*str)++;
        return (res);
    }
    unexpected(**str);
    return NULL;
}

node *parse_multi(char **str)
{
    node *l;
    node *r;
    node temp;

    l = parse_nbr_or_group(str);
    if(!l)
        return NULL;
    while(**str == '*')
    {
        (*str)++;
        r = parse_nbr_or_group(str);
        if(!r)
        {
            destroy_tree(l);
            return NULL;
        }
        temp.type = MULTI;
        temp.l = l;
        temp.r = r;
        l = new_node(temp);
    }
    return (l);
}

node *parse_add(char **str)
{
    node *l;
    node *r;
    node temp;

    l = parse_multi(str);
    if(!l)
        return NULL;
    while(**str == '+')
    {
        (*str)++;
        r = parse_multi(str);
        if(!r)
        {
            destroy_tree(l);
            return NULL;
        }
        temp.type = ADD;
        temp.l = l;
        temp.r = r;
        l = new_node(temp);
    }
    return l;
}

//given 

int eval_tree(node *tree)
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
}

int check_nbrs(char *input)
{
    int i = -1;

    while(input[++i])
    {
        if(isdigit(input[i]) && isdigit(input[i + 1]))
        {
            unexpected(input[i + 1]);
            return 1;
        }
    }
    return 0;
}

int main(int ac, char *av[])
{
    node *tree;
    char *input;

    if(ac != 2)
        return 1;
    input = av[1];
    if(check_nbrs(input) != 0)
        return 1;
    if(check_parentheses(input) < 0)
    {
        unexpected(')');
        return 1;
    }
    else if(check_parentheses(input) != 0)
    {
        unexpected('(');
        return 1;
    }
    tree = parse_add(&input);
    if(!tree)
        return 1;
    if(*input)
    {
        unexpected(*input);
        destroy_tree(tree);
        return 1;
    }
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return 0;
}