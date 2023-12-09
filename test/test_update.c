#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef	struct s_list
{
	char	*s;
	struct s_list	*right;
	struct s_list	*down;
}	t_list;

void	add_anotherone(t_list **head, char *s)
{
	t_list	*new;
	int	i=0;

	new = malloc(sizeof(t_list));
	new->s = calloc(strlen(s) + 1, 1);
	new->right = 0;
	new->down = 0;
	while (s[i])
	{
		new->s[i] = s[i];
		i++;
	}
	*head = new;
}

void	add_one(t_list **head, char *s)
{
	t_list	*new;
	int	i=0;

	new = malloc(sizeof(t_list));
	new->s = calloc(strlen(s) + 1, 1);
	new->right = 0;
	new->down = 0;
	add_anotherone(&new->right, "child_right_and_right");
	while (s[i])
	{
		new->s[i] = s[i];
		i++;
	}
	*head = new;
}

void	add_down(t_list **head, char *s)
{
	t_list	*new;
	t_list	*temp;
	int	i;

	i = 0;
	new = malloc(sizeof(t_list));
	new->s = calloc(strlen(s) + 1, 1);
	while (s[i])
	{
		new->s[i] = s[i];
		i++;
	}
	new->right = 0;
	new->down = 0;
	add_one(&new->right, "child_right");
	if (*head == 0)
		*head = new;
	else
	{
		temp = *head;
		while (temp->down)
			temp = temp->down;
		temp->down = new;
	}
}

void	add_right(t_list **head, char *s)
{
	t_list	*new;
	t_list	*temp;
	int	i;

	i = 0;
	new = malloc(sizeof(t_list));
	new->s = calloc(strlen(s) + 1, 1);
	while (s[i])
	{
		new->s[i] = s[i];
		i++;
	}
	new->right = 0;
	new->down = 0;
	add_down(&new->down, "child");
	if (*head == 0)
		*head = new;
	else
	{
		temp = *head;
		while (temp->right)
			temp = temp->right;
		temp->right = new;
	}
}
void	init_struct(t_list **head)
{
	add_right(head, "right_1");
	add_right(head, "right_2");
	add_right(head, "right_3");
}

void recursive_fork(t_list *list) {
	pid_t	pid_down;
	pid_t	pid_right;
	char	c;

	if (list->down)
	{
		pid_down = fork();
		if (pid_down == 0)
		{
		dup2(2, 1);
		recursive_fork(list->down);
        exit(0);
		}
	}
	if (list->right)
	{
		pid_right = fork();
		if (pid_right == 0)
		{
		recursive_fork(list->right);
		exit(0);
		}
	}
	printf("%s : %d\n", list->s, getpid());
	if (list->down) {
        waitpid(pid_down, NULL, 0);
    }
    if (list->right) {
        waitpid(pid_right, NULL, 0);
    }
}


int main(int ac, char *av[]) {
	t_list	*new;

	new = NULL;
	init_struct(&new);
    recursive_fork(new);
    return 0;
}
