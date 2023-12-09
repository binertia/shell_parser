#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct s_map_list
{
	char *key;
	char *value;
	struct s_map_list *next;
} t_map_list;

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

// ---------------- struct input ------- part
typedef struct s_cmd
{
	int type;		 // if (type == BUILD_IN) run in build_in();
	char **cmd_args; // cmd[0] == name
	struct s_cmd *next;
} t_cmd;

//==================  redirect_fd  =============================
//
//		will use same typedef for redir_input and redir_output;
//
//==============================================================

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
} t_indirect; // manual use is below

typedef struct s_exec
{
	int	run_condition;
	struct s_exec *child;

	t_cmd *cmd;
	t_indirect *redirect;
	t_map_list
		*env; /* use if called minishell recursively ; // if have another use
				 case please tell me it will be ready to send to new env;  if
				 cmd is not "minishell" it will be null for now"*/
	struct s_exec *next;
} t_exec;

typedef struct s_full_exec
{
	t_exec *exec;
	int run_sub_shell;
	struct s_full_exec *new;
} t_full_exec;


// ---------------- get_ stuff --------- part
// if ls " >> "ls" "\"" if (close then )

int	ft_is_space(int c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (1);
	return (0);
}

typedef struct	s_input_list
{
	char	*str;
	char	*tok_str;
	struct	s_input_list	*child;
	struct s_input_list	*next;
}	t_input_list;


typedef struct s_list
{
	struct s_list	*next;
}	t_list;

void	*ft_list_last(void *list)
{
	t_list	*temp;

	temp = (t_list *)list;
	while (temp->next)
		temp = temp->next;
	return (temp);
}

void	add_buf_to_list(t_input_list **head,char *s)
{
	t_input_list	*new_list;

	new_list = (t_input_list *)malloc(sizeof(t_input_list));
	new_list->str = strdup(s);
	new_list->next = NULL;

	if (*head == NULL)
		*head = new_list;
	else
	((t_input_list *)ft_list_last(*head))->next = new_list;
}
//-------------------------- end utils ------------------------------

int	handle_quote(char *str, char token)
{
	while (*str)
	{
		if (*str == token)
			return (1);
		str++;
	}
	return 0;
}

void	get_redirect_file(char *str, size_t index, t_input_list **head)
{
	size_t	i;
	int	token;
	char	*res;

	token = 0;
	i = index + 1;
	while (str[i] && strchr("><", str[i]))
	{
		if (token && str[i] == token)
			break ;
		else if (strchr("\"\'", str[i]) && token == 0 \
			&& handle_quote(str + i + 1, str[i]))
			token = str[i];
		else if (ft_is_space(str[i]) && token == 0)
				break;
		i++;
	}
	if (str[i] && (strchr("><", str[i]) || ft_is_space(str[i])))
		i -= 1;
	res = strndup(str + index, i + 1 - index);
	add_buf_to_list(head, res);
	free(res);
	return;
}

void	manage_redirect(char *str, int index, t_input_list **head)
{
	char	*res;
	int		i;
	int		is_filename;

	is_filename = 0;
	i = 0;
	if (str[index + 1])
	{
		index++;
		if (str[index] != '&')
		{
			while (str[index] && ft_is_space(str[index]))
				index++;
			get_redirect_file(str, index, head);
		}
		else
		{
			is_filename = 1;
			while (ft_is_space(str[index]) == 0)
			{
				i++;
				res = strndup(str + index, i + 1 - index);
				add_buf_to_list(head, res);
				free(res);
			}
		}
	}
}


char	*get_strtoken(char *input)
{
	char	*token_str;
	char	token;
	size_t	i;
	int	count = 0;

	i = 0;
	token = 0;
	token_str = calloc(strlen(input) + 1, 1);
	while (input[i])
	{
		if (strchr("()|&<>$\"\'", input[i]))
		{
			token_str[i] = input[i];
			if (strchr("\"\'", input[i]))
			{
				if (handle_quote(input + i + 1, input[i]))
				{
					token = input[i];
					token_str[i] = input[i];
					i++;
					while (input[i])
					{
						if (input[i] == token)
						{
							token_str[i] = input[i];
							break ;
						}
						token_str[i] = token;
						i++;
					}
					if (input[i] == 0)
						i--;
				}
				else			// can add error for handle_quote syntax error here :else retunr (0);
					token_str[i] = 'a';
			}
			else if (strchr("()", input[i]))
			{
				if (input[i] == '(')
					count++;
				else
					count--;
				if (count < 0)
					return (0);
			}
			else if (input[i] == '&')
			{
				if (input[i + 1] && input[i + 1] == '&')
				{
					token_str[i] = '&';
					i++;
					token_str[i] = '&';
				}
				else
					token_str[i] = 'a';
			}
			else
				token_str[i] = input[i];
		}
		else if (ft_is_space(input[i]))
		{
			token_str[i] = ' ';
			input[i] = ' ';
		}
		else
			token_str[i] = 'a';
		i++;
	}
	if (count != 0)
		return (0); // check error on "()"
	return (token_str);
}

void	add_raw_input_list(t_input_list **head, char *src, char *tok)
{
	t_input_list *new;
	t_input_list *temp;

	new = (t_input_list *)malloc(sizeof(t_input_list));
	new->str = strdup(src);
	new->tok_str = strdup(tok);
	new->child = NULL;
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

void	ft_tok_split(char *src_str, char *token_str, t_input_list **head)
{
	char *buf ;
	char *tok_buf ;
	size_t	j;
	size_t	i;

	j = 0;
	i = 0;
	buf = (char *)calloc(strlen(src_str) + 1, 1);
	tok_buf = (char *)calloc(strlen(src_str) + 1, 1);
	j = 0;
	while (src_str[i])
	{
		if (ft_is_space(src_str[i]) && token_str[i] == ' ')
		{
			if (j > 0)
			{
				add_raw_input_list(head, buf, tok_buf);
				memset(buf, 0, strlen(buf));
				memset(tok_buf, 0, strlen(tok_buf));
				j = 0;
			}
		}
		else
		{
			buf[j] = src_str[i];
			tok_buf[j] = token_str[i];
			j++;
		}
		i++;
	}
	if (j > 0)
        add_raw_input_list(head, buf, tok_buf);
	free(buf);
	free(tok_buf);
}
//-------------------------------------------

char	*ft_comchr(char *s1, char *s2)
{
	size_t	i;

	while (*s1)
	{
		i = 0;
		while (s2[i])
		{
			if (*s1 == s2[i])
				return (s1);
			i++;
		}
		s1++;
	}
	return (NULL);
}

typedef struct s_tree
{
	int	run_condition;
	int error;

	t_input_list	*args;
	t_input_list	*redir;
	t_input_list	*parens;
	struct s_tree	*next;
}		t_tree;

size_t	ft_list_str_len(t_input_list *list)
{
	size_t	count;
	size_t	i;

	count = 0;
	while (list)
	{
		i = 0;
		while (list->tok_str && list->tok_str[i])
		{
			count++;
			i++;
		}
		list = list->next;
	}
	return (count);
}


t_tree	*ft_new_tree(void)
{
	t_tree *new;

	new = (t_tree *)malloc(sizeof(t_tree));
	new->run_condition = 0;
	new->args = NULL;
	new->redir = NULL;
	new->next = NULL;
	return (new);
}

t_input_list	*ft_new_input_list(char *str, char *tok_str)
{
	t_input_list	*new;

	new = malloc(sizeof(t_input_list));
	new->next = NULL;
	new->child = NULL;
	new->tok_str = strdup(tok_str);
	new->str = strdup(str);
	return (new);
}

void	ft_newtree_addlast(t_tree **head)
{
	t_tree	*new;
	t_tree	*temp;

	new = ft_new_tree();
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

void	parsing_args(t_tree **head, char *str, char *tok_str)
{
	t_input_list	*temp;

	temp = (*head)->args;
	if (temp == NULL)
		(*head)->args = ft_new_input_list(str, tok_str);
	else
	{
		while (temp->next)
			temp = temp->next;
		temp -> next = ft_new_input_list(str, tok_str);
	}
}

void	parsing_redir(t_tree **head, char *str, char *tok_str, char *child)
{
	t_input_list	*temp;

	temp = (*head)->redir;
	if (temp == NULL)
	{
		(*head)->redir = ft_new_input_list(str, tok_str);
		if (child)
			(*head)->redir = ft_new_input_list(child, 0);
	}
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = ft_new_input_list(str, tok_str);
		if (child)
			temp->next = ft_new_input_list(child, 0);
	}
}
void	parser(t_input_list *list, t_tree **current_head, int res_indicate, char *prev_stack)
{
	t_tree	*res;
	t_tree	*head;
	char	*buf;
	t_input_list	*stack;
	char	*ptr;
	char	*temp;

	char	*s_temp1;
	char	*s_temp2;
	if (*current_head == NULL)
		*current_head = ft_new_tree();
	head = *current_head;
	while (list)
	{
		if (prev_stack)
		{
			printf("hateeee it\n");
			s_temp1 = list->str;
			s_temp2 = list->tok_str;
			list->str = list->str + (prev_stack - list->tok_str);
			list->tok_str = prev_stack;
		}
		// if (ft_comchr(list->tok_str, "<>|&("))
		// {
		// }
		// else
		// {
		// &&123> >3
		//    123>&3      s32> 43
		if (ft_comchr(list->tok_str, "><"))
		{
			int	i = 0;
			int	split_first = 0;
			char	*temp;
			char	*temp1;
			char	*temp2;

			if (strstr(list->tok_str, ">>>") || strstr(list->tok_str, "<<<"))
			{
				head->error = 1;
				return;
			}
			while(list->tok_str + i != ft_comchr(list->tok_str, "><"))
				i++;
			if (i != 0)
			{
				int	j = 0;
				while (j < i)
				{
					if (list->tok_str[j] >= '0' && list->tok_str[j] <= '9')
						j++;
				}
				temp1 = strndup(list->str, i);
				temp2 = strndup(list->tok_str, i);
				if (j != i)
				{
					parsing_args(&head, temp1, temp2);
					split_first = 1;
				}
			}
			//
			if (list->tok_str[i + 1])
			{
				// case :: >> : <<
				if (list->tok_str[i + 1] == list->tok_str[i])
				{
					if (list->tok_str[i + 1])
					{

					}
				}
			}
			//
			else if (list->tok_str[i + 1] == 0)
			{
				parsing_redir(&head, list->str + i, list->tok_str + i, 0);
			}
			else if (list->next == NULL)
			{
				head->error = 1;
				return;
			}
		}
		//end case ><;
		//  case:: | ,|| , &&
		if (ft_comchr(list->tok_str, "|&"))
		{
			int	i = 0;
			while (list->tok_str[i] != '|' && list->tok_str[i] != '&')
				i++;
			//case ::&
			//if (ft_comchar)
			//end
			if (i != 0)
			{
				char *temp1 = strndup(list->str, i);
				char *temp2 = strndup(list->tok_str, i);
				parsing_args(&head, temp1, temp2);
			}
			//implement all stuff | 
			//
			// case:: || ; &&  ;

			if (strstr(list->str, "||" ) || strstr(list->str, "&&"))
			{
				if (strstr(list->str, "|||") || strstr(list->str, "&&&"))
				{
					head->error = 1;
					return ;
				}
				else if (*(ft_comchr(list->str, "|&")) == '|'){
					head->run_condition = 1; // case: || ;
					i++;
				}
				else if (*(ft_comchr(list->str, "|&")) == '&'){
					head->run_condition = 2; // case: || ;
					i++;
				}
			}
			else
			{
				head->run_condition = 0; // default; case: | ;
			}
			//----------------------
			// what seperate it; only case of 
			// (| || && 3 case ; do as below) >need to change to new_head //
			// (< > ; put stack to same tree(head_tree))
			// (parens ; put stack to same tree(head_tree)
			//if 
			// case:: |

			head->next = ft_new_tree();
			head = head->next;
			if (list->tok_str[i + 1])
			{
				char	*temp1 = strdup(list->str + i + 1);
				char	*temp2 = strdup(list->tok_str + i + 1);
				// need to bring it back to loop; already did;
				stack = ft_new_input_list(temp1, temp2);
				parser(list, &head, 1, list->tok_str + i + 1); // call recursively
				return ;
				//
				//parsing_args(&head, temp1, temp2);
				free(temp1);
				free(temp2);
			}
			//clean old stack
			if (prev_stack) 
			{
				list->tok_str = s_temp2;
				list->str = s_temp1;
				prev_stack = 0;
			}
			// end clean stack
			list = list->next;
		}
		else {
			parsing_args(&head, list->str, list->tok_str);
			list = list->next;
		}
		// end | case

		// case:: || 
	}
	if (res_indicate)
		*current_head = head;
}

int	main(int ac, char *av[], char *curr_env[])
{
	t_map_list *env;
	char	*s;
	char	*temp;
	t_input_list *raw_input;
	t_tree	*head;

	raw_input = NULL;
	env = get_env_list(curr_env);
	temp = strdup(av[1]);
	s = get_strtoken(temp);
	if (s == 0)
	{
		s="error parser";
		printf("%s\n", s);
		return (1);
	}
	ft_tok_split(temp, s, &raw_input);
	//----------
	
	parser(raw_input, &head, 0, 0);


	////////
	while (raw_input)
	{
		printf("%s \n", raw_input->str);
		printf("%s \n", raw_input->tok_str);
		raw_input = raw_input->next;
	}
	while (head)
	{
		while (head->redir)
		{
			printf(" redir: --%s--\n", head->redir->str);
			if (head->redir->child)
				printf(" child :--%s--\n", head->redir->child->str);
			head->redir = head->redir->next;
		}
		// head = head->next;
		while (head->args)
		{
			printf("--%s--\n", head->args->str);
			head->args = head->args->next;
		}
		printf("__change_head\n");
		head = head->next;
	}
	// ////
	//process_input(av[1]);
	// // ----------- test fn -----------
	// while (env)
	// {
	// 	printf("%s : %s\n", env->key, env->value);
	// 	env = env->next;
	// }
	// // ----------- test fn -----------
}
