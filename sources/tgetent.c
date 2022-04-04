#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>
#include <curses.h>
#include <term.h>

int main(void)
{
	char *env = getenv("TERM");

	if (!env)
		printf("$TERM is null!\n");
	else
		printf("$TERM is %s\n", env);
	strcpy(env, "hello");
	
	tgetent(NULL, env);
	
	if (!env)
		printf("$TERM is null!\n");
	else
		printf("$TERM is %s\n", env);	
	return (0);
}