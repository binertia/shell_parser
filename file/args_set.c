#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------ for env list only ---------
typedef struct s_map_list
{
	char *key;
	char *value;
	struct s_map_list *next;
} t_map_list;  
//----------------------------------------

typedef struct s_cmd
{
	int type;		 // if (type == BUILD_IN) run in build_in();
	char **cmd_args; // cmd[0] == name
} t_cmd;

struct s_redirect_fd
{
	int from_fd;
	int to_fd;
};

struct s_redirect_file
{
	char *file_name;
	int redir_fd;
};

#define ERROR                                                                  \
	0 // if type == ERROR  just throw error; () // will catch illegal error use
	  // (bad_expansion.. >*.c  || use & wrong place)
#define REDIR_IN 1	// <
#define REDIR_OUT 2 // >
#define APPEND 3	// >>
#define HEREDOC 4

typedef struct s_redirect
{
	int type;		   // type define on top
	int use_ampersand; // ampersand = & :: check if it 1 & or 0 ; if 1 do
					   // dup(**, 1); dup(**, 2);
	int use_filename;  // if yes it easy to check for free(file->filename); also
					   // easy to decide use redirect_file or redirect_fd;
	struct s_redirect_file file;
	struct s_redirect_fd fd;
	struct s_redirct *next;
} t_direct; // manual use is below

typedef struct s_exec
{
    int        run_condition; // (|, ||, &&) if (run_condition == | ) do |; else if (run condition -compare current exit code) ? run : not run
    struct s_exec    *child;

    t_cmd *cmd;
    t_direct *redirect;
    t_map_list *env;
    struct s_exec    *next;
}    t_exec;

// ft_split
static size_t num_genr(char const *s, char c, int indc, int line)
{
	size_t count;
	size_t i;
	size_t target;

	target = 0;
	count = 0;
	i = -1;
	while (s[++i])
	{
		if (i != 0 && s[i - 1] != c && s[i] == c)
			count++;
		else if (s[i] != c && s[i + 1] == 0)
			count++;
		if ((int)count == line && indc == 0)
			return (target);
		else if (indc == 1 && s[i] != c && s[i + 1] == 0)
			return (i + 1);
		else if ((int)count == line && indc == 1)
			return (i);
		if (s[i] == c)
			target = i + 1;
	}
	return (count);
}

static char *cup_noodle_malloc(const char *src, char c, size_t line)
{
	size_t start;
	size_t end;
	size_t i;
	char *dst;

	i = 0;
	start = num_genr(src, c, 0, line);
	end = num_genr(src, c, 1, line);
	dst = (char *)malloc((end - start) + 1);
	if (!dst)
		return (0);
	while (start != end)
		dst[i++] = *(src + (start++));
	dst[i] = 0;
	return (dst);
}

static int free_time(char **s, char const *root, char c, size_t line)
{
	size_t end_line;

	end_line = num_genr(root, c, -1, -1);
	while (line <= end_line && line++ != 0)
		free(s[line - 2]);
	free(s);
	return (1);
}

char **ft_split(char const *s, char c)
{
	size_t line;
	char **ptr_result;

	if (!s)
		return (0);
	line = num_genr(s, c, -1, -1);
	ptr_result = malloc(sizeof(char *) * (line + 1));
	if (!ptr_result)
		return (0);
	if (!line)
	{
		ptr_result[0] = 0;
		return (ptr_result);
	}
	ptr_result[line] = 0;
	while (line-- != 0)
	{
		ptr_result[line] = cup_noodle_malloc(s, c, (line + 1));
		if (!ptr_result[line] && free_time(ptr_result, s, c, line))
			return (0);
	}
	return (ptr_result);
}
//--------------------------------->

void ft_add_maplist(t_map_list **head, char **src)
{
	t_map_list *new;
	t_map_list *temp;
	size_t key_len;
	size_t value_len;

	new = malloc(sizeof(t_map_list));
	new->key = strdup(src[0]);
	if (src[1])
		new->value = strdup(src[1]);
	new->next = NULL;
	if (*head == NULL)
		*head = new;
	else
	{
		temp = *head;
		while (temp->next)
			temp = temp->next;
		temp->next = new;
	}
}

t_map_list *get_env_list(char **env)
{
	size_t i;
	char **temp;
	t_map_list *head;

	head = NULL;
	i = 0;
	while (env[i])
	{
		ft_add_maplist(&head, ft_split(env[i], '='));
		i++;
		// int	j = 0;
		//  temp = ft_split(env[i], '=');
		//  // test
		//  while (j < 2)
		//  {
		//  	printf("%s\n", temp[j]);
		//  	j++;
		//  }
	}
	return (head);
}
// ---------------- end env ------------ part
//
// ---------------- utils --------------

t_exec	*new_exec()
{
	t_exec	*new;

	new = malloc(sizeof(t_exec));
	new->child = NULL;
	new->redirect = NULL;
	new->env = NULL;
	new->cmd = NULL;
	new->run_condition = 0;  // 0 is pipe : 1 is || : 2 is &&
	new->next = NULL;
	return (new);
}

// ---------------- utils end ----------
void	case_one(t_exec **head, char **env)
{
	t_exec *res;
	t_exec *new = malloc(sizeof(t_exec));

	new = new_exec();
	res = new;
	new->env = get_env_list(env);
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->type = 0;
	new->cmd->cmd_args = ft_split("ls -la", ' ');
	new->next = new_exec();
	new = new->next;

	new->env = get_env_list(env);
	new->run_condition = 1; // case ||
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->type = 1; // build_in;
	new->cmd->cmd_args = ft_split("echo \"Hello\"", ' ');
	new->redirect = malloc(sizeof(t_direct));
	new->redirect->type = 2;
	new->redirect->use_filename = 0;
	new->redirect->fd.from_fd = 1;
	new->redirect->fd.to_fd = 2;
	new->next = new_exec();
	new = new->next;

	new->env = get_env_list(env);
	new->run_condition = 2; // case &&
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->type = 0;
	new->cmd->cmd_args = ft_split("asdf", ' ');
	new->next = new_exec();
	new = new->next;

	new->env = get_env_list(env);
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->type = 1;
	new->cmd->cmd_args = ft_split("echo \"World\"", ' ');
	new->redirect = malloc(sizeof(t_direct));
	new->redirect->type = 2;
	new->redirect->use_filename = 1;
	new->redirect->file.file_name = strdup("world.txt");
	new->redirect->file.redir_fd = 1;
	new->redirect->next = 0;

	*head = res;
}
// case 1 ::  ls -la | echo "Hello" >&2 && asdf || echo "World" > world.txt

void	case_two(t_exec **head, char **env)
{
	t_exec *res;
	t_exec *new = malloc(sizeof(t_exec));

	new = new_exec();
	res = new;
	new->run_condition = 2; // case && after (... "how are you") &&  <<
	new->env = get_env_list(env);
	new->child = new_exec();
	//child part
	new->child->run_condition = 2;
	new->child->env = get_env_list(env);
	new->child->cmd = malloc(sizeof(t_cmd));
	new->child->cmd->cmd_args = ft_split("ls", ' ');
	new->child->cmd->type = 0;
	new->child->next = new_exec();
	//
	new->child->next->env = get_env_list(env);
	new->child->next->cmd = malloc(sizeof(t_cmd));
	new->child->next->cmd->cmd_args = ft_split("echo \"how_are_you\"", ' ');
	new->child->next->cmd->type = 0;
	//
	new->next = new_exec();
	new = new->next;

	new->env= get_env_list(env);
	new->child = new_exec();
	// child part
	new->child->run_condition = 2;
	new->child->env = get_env_list(env);
	new->child->child = new_exec();
	new->child->child->env = get_env_list(env);
	new->child->child->cmd = malloc(sizeof(t_cmd));
	new->child->child->cmd->cmd_args = ft_split("ls", ' ');
	new->child->child->cmd->type = 0;
	//
	new->child->next = new_exec();
	new->child->next->env = get_env_list(env);
	new->child->next->child = new_exec();
	new->child->next->child->env = get_env_list(env);
	new->child->next->child->cmd = malloc(sizeof(t_cmd));
	new->child->next->child->cmd->cmd_args = ft_split("ls", ' ');
	new->child->next->child->cmd->type = 0;
	new->child->next->child->next = new_exec();
	new->child->next->child->next->env = get_env_list(env);
	new->child->next->child->next->cmd = malloc(sizeof(t_cmd));
	new->child->next->child->next->cmd->cmd_args = ft_split("ls", ' ');
	new->child->next->child->next->cmd->type = 0;
	
	*head = res;
}
// case 2 :: (ls && echo "how_are_you") && ((ls) && (ls | ls))

void	case_three(t_exec **head, char **env)
{
	t_exec	*new = new_exec();
	*head = new;

	// no env for this;
	new->run_condition = 2;
	new->child = new_exec();
	new->child->run_condition = 2;
	new->child->cmd = malloc(sizeof(t_cmd));
	new->child->cmd->cmd_args = ft_split("ls", ' ');
	new->child->cmd->type = 0;
	new->child->next = new_exec();
	new->child->next->run_condition = 2;
	new->child->next->child = new_exec();
	new->child->next->child->run_condition = 2;
	new->child->next->child->cmd = malloc(sizeof(t_cmd));
	new->child->next->child->cmd->cmd_args = ft_split("ls", ' ');
	new->child->next->child->cmd->type = 0;
	new->child->next->child->next = new_exec();
	new->child->next->child->next->cmd = malloc(sizeof(t_cmd));
	new->child->next->child->next->cmd->cmd_args = ft_split("ls", ' ');
	new->child->next->child->next->cmd->type = 0;
	
	new->child->next->next = new_exec();
	new->child->next->next->cmd = malloc(sizeof(t_cmd));
	new->child->next->next->cmd->cmd_args = ft_split("ls", ' ');
	new->child->next->next->cmd->type = 0;

	new->next = new_exec();
	new = new->next;
	new->run_condition = 1; // case ||
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->cmd_args = ft_split("ls", ' ');
	new->cmd->type = 0;
	new->next = new_exec();

	new = new->next;
	new->cmd = malloc(sizeof(t_cmd));
	new->cmd->cmd_args = ft_split("ls", ' ');
	new->cmd->type = 0;
}
// case 3 :: (ls && (ls && ls) && ls) && ls || ls

// ------------------------------------------
// case 1 ::  ls -la | echo "Hello" >&2 && asdf || echo "World" > world.txt
// case 2 :: (ls && echo "how_are_you") && ((ls) && (ls | ls))
// case 3 :: (ls && (ls && ls) && ls) && ls || ls


int	main(int ac, char *av[], char *curr_env[])
{
	t_map_list *env;
	char	*s;
	char	*temp;
	t_exec	*one;
	t_exec	*two;
	t_exec	*three;

	env = get_env_list(curr_env);
	// ------------test-----------
	// while (env)
	// {
	// 	printf("%s : ", env->value);
	// 	printf("%s\n", env->key);
	// 	env = env->next;
	// }
	case_one(&one, curr_env);
	case_two(&two, curr_env);
	case_three(&three, curr_env);
}
