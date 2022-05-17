/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replace_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyojlee <hyojlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/27 15:37:13 by hyojlee           #+#    #+#             */
/*   Updated: 2022/05/17 14:36:45 by hyojlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
** split('$')의 결과임
** split 호출 전에 strchr('$')을 하기 때문에 무조건 환경변수이름으로 시작하는 문자열(str)이 들어옴
** get_env한 결과(rpl에 들어가는 값)는 항상 동적할당되어있다. 빈 문자열인 경우 ""을 strdup해주기 때문.
*/
static char	*replace_env(char *data, int start, int end)
{
	int		idx;
	char	*env;
	char	*rpl;
	char	*ret;
	char	*str;

	idx = 0;
	if (end - start < 1)
		return (ft_strdup("$"));
	str = ft_substr(data, start, end - start);
	while (str[idx] && !ft_isblank(str[idx]))
		idx++;
	env = ft_substr(str, 0, idx);
	rpl = get_env_or_status(env);
	free(env);
	env = 0;
	if (!str[idx])
		ret = ft_strdup(rpl);
	else
		ret = ft_strjoin(rpl, str + idx);
	free(rpl);
	rpl = 0;
	free(str);
	str = 0;
	return (ret);
}

static void	init_variable(int *dquote, int *front, int *end)
{
	*dquote = FALSE;
	*end = -1;
	*front = *end + 1;
}

static void	join_squote(char **res, char *data, int *front, int *end)
{
	join_str(res, data, front, *end);
	*end = ft_strlen(data) - ft_strlen(ft_strchr(data + *front, SQUOTE));
	join_str(res, data, front, *end);
}

static void	replace_token(char **res, char *data)
{
	int		dquote;
	int		front;
	int		end;

	init_variable(&dquote, &front, &end);
	while (data[++end])
	{
		if (data[end] == SQUOTE && dquote == FALSE)
			join_squote(res, data, &front, &end);
		else if (data[end] == DQUOTE)
		{
			dquote = !dquote;
			join_str(res, data, &front, end);
		}
		else if (data[end] == '$')
		{
			join_str(res, data, &front, end++);
			find_end_pos(data, &end);
			join_envp(res, replace_env(data, front, end), &front, &end);
		}
		else if (!data[end + 1])
			join_str(res, data, &front, end + 1);
	}
}

void	replace_recur(t_node *node)
{
	char	*org_data;

	if (!node)
		return ;
	if (ft_strchr(node->data, '$'))
	{
		org_data = node->data;
		node->data = ft_strdup("");
		replace_token(&(node->data), org_data);
		free(org_data);
		org_data = 0;
	}
	if (!ft_strcmp(node->data, "~"))
	{
		org_data = node->data;
		node->data = ft_strdup(getenv("HOME"));
		free(org_data);
		org_data = 0;
	}
	replace_recur(node->left);
	replace_recur(node->right);
}
