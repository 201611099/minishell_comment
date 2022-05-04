/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyojlee <hyojlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 15:40:18 by hyojlee           #+#    #+#             */
/*   Updated: 2022/05/04 21:00:01 by hyojlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./minishell.h"

static void handler(int signo)
{
	if (signo == SIGINT)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}	
}

static void	init(t_info *info)
{
	struct termios t;

	ft_bzero(info, sizeof(t_info));
	info->list = create_list();
	info->tree = create_tree();
	info->file = (t_file *)malloc(sizeof(t_file));
	ft_bzero(info->file, sizeof(t_file));
	tcgetattr(0, &t);
	t.c_lflag &= ~(ICANON | ECHOCTL);
	tcsetattr(0, TCSANOW, &t);
}

int main(int argc, char **argv, char **envp)
{
	char	*line;
	t_info	info;
	
	(void)argc;
	(void)argv;
	signal(SIGINT, handler);
	signal(SIGQUIT, SIG_IGN);
	init(&info);
	env_preprocess(&info, envp);
	while (1)
	{
		line = readline("minishell> ");
		if (line)
		{
			if (line[0] == '\0')
				continue ;
			add_history(line);
			if (check_quote(line) == FALSE)	//따옴표 체크
			{
				print_err(&info, line, 0); //syntax error 258
				continue ; // 다시 명령줄 출력해야하므로
			}
			tokenize(&(info.list), line);	// 토큰화
			print_token(info.list);
			//구문 분석 및 파싱 과정 (AST TREE)
			if (syntax(&info) == FALSE)
			{
				print_err(&info, line, 0); //syntax error 258
				continue ;
			}
			if (chk_syntax(info.tree->root) == FALSE)
			{
				print_err(&info, line, 0); //syntax error 258
				continue ;
			}
			// print_tree(info.tree->root);
			// printf("\n\n");
			//환경변수 치환
			replace_recur(&info, info.tree->root);
			print_tree(info.tree->root);
			//실행 과정 (이후에 tok list 비우기)
			read_tree(&info, info.tree->root);
			ft_clear(&info);
			// add_history(line); 위치 변경 73번째로 - 77번째때문에 add_history()가 안됨
			free(line);
			line = NULL;
			// system("leaks minishell");
		}
		else //ctrl + d
		{
			ft_putstr_fd("\x1b[1A", STDOUT);
			ft_putstr_fd("\033[12C", STDOUT);
			printf("exit\n");
			exit(0); //마지막 종료 상태 값 exit 인자로 넣기
		}
	}
	
	return (0);
}