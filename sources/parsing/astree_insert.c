/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   astree_insert.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyojlee <hyojlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/04 21:07:48 by hyojlee           #+#    #+#             */
/*   Updated: 2022/05/17 16:46:49 by hyojlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	insert_pipe_heredoc(t_astree *tree, t_node *node)
{
	t_node	*cur;
	t_node	*pre;

	cur = tree->root;
	pre = cur;
	while (cur && cur->type == HEREDOC)
	{
		pre = cur;
		cur = cur->left;
	}
	if (!pre || pre == cur)
		tree->root = node;
	else
		pre->left = node;
	node->left = cur;
}

void	insert_redir(t_astree *tree, t_node *node)
{
	t_node	*cur;
	t_node	*pre;

	cur = tree->root;
	pre = cur;
	while (cur)
	{
		if (cur->type == TOKEN || cur->type == SQUOTE || cur->type == DQUOTE)
			break ;
		pre = cur;
		if (cur->type == PIPE)
			cur = cur->right;
		else
			cur = cur->left;
	}
	if (!pre || pre == cur)
		tree->root = node;
	else if (pre->type == PIPE)
		pre->right = node;
	else
		pre->left = node;
	node->left = cur;
}

/*
** pipe가 여러 개 존재하는 경우, 나중에 온 파이프가 위에 위치한다.
** redir 또는 heredoc이 여러 개 존재하는 경우, 나중에 온 친구가 아래에 위치한다.
** 
** 1. 위에서부터 pipe인지 검사하고 오른쪽이 비어있으면 insert
** 2. pipe인데 오른쪽이 채워져있으면 이때 리다이렉션인지 검사하고 마지막 리다이렉션을 찾아 왼쪽으로 이동
**	  -> 마지막 리다이렉션까지 이동 후 왼쪽이 비워져 있으면 거기에 insert, 채워져 있으면 왼쪽 끝까지 이동 후 insert
** 3. heredoc인 경우, pipe와 마찬가지
*/
void	insert_path(t_astree *tree, t_node *node)	//path와 args에 들어갈 insert 함수
{
	t_node	*cur;
	t_node	*pre;

	cur = tree->root;
	pre = cur;
	while (cur)
	{
		pre = cur;
		if (cur->type == PIPE)
			cur = cur->right;
		else
			cur = cur->left;
	}
	if (!pre)
		tree->root = node;
	else if (pre->type == PIPE)
		pre->right = node;
	else
		pre->left = node;
}

// 반드시 리다이렉션이 나오고 호출되기 때문에 루트가 비어있는지 확안헐 필요는 없음
void	insert_filename(t_astree *tree, t_node *node)
{
	t_node	*cur;
	t_node	*pre;

	cur = tree->root;
	while (cur) //root는 반드시 채워져 있어야하므로 pre = cur가 while 전에 있을 필요는 없음
	{
		pre = cur;
		if (cur->type == PIPE)
			cur = cur->right;
		else if (cur->type == REDIR || cur->type == HEREDOC)
		{
			if (cur->right)
				cur = cur->left;
			else
				cur = cur->right;
		}
		else
			cur = cur->left;
	}
	if (pre->type == PIPE || !pre->right)
		pre->right = node;
	else
		pre->left = node;
}

void	insert_heredoc_redir(void)
{
	t_tok	*new_tok;
	t_node	*new_node;
	char	*itoa;

	new_tok = (t_tok *)malloc(sizeof(t_tok));
	if (!new_tok)
		print_strerr(errno);
	new_tok->type = REDIR;
	new_tok->data = ft_strdup("<");
	new_tok->next = 0;
	new_node = create_node(new_tok);
	insert_redir(get_info()->tree, new_node);
	new_tok->type = TOKEN;
	free(new_tok->data);
	itoa = ft_itoa(get_info()->h_count++);
	new_tok->data = ft_strjoin(".heredoc_", itoa);
	free(itoa);
	itoa = 0;
	insert_filename(get_info()->tree, create_node(new_tok));
	free(new_tok->data);
	new_tok->data = 0;
	free(new_tok);
	new_tok = 0;
}
