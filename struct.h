//=============== main list ================
//
//		seperated each list by pipe; |
//
//==========================================

// ls (asdfjkl asdfjkl asdfjkl j asdfjkl )

// prototype should be similar type

// int	execution(t_exec *cmd_list, int run_in_sub_shell_or_not , int *here_doc_fd_arrays)
typedef struct s_exec
{
	t_cmd			*cmd;
	t_indirect		*redirect;
	t_map			*env;	/* use if called minishell recursively ; // if have another use case please tell me
									it will be ready to send to new env;  if cmd is not "minishell" it will be null for now"*/
	struct s_exe	*next;	
}	t_exec    

//=============== cmd =======================
//			
//		cmd_args[0] == char *cmd_name;
//		cmd_args = {[cmd name], [args], [...].. [null]}; 
//		
//===========================================

#define BUILD_IN_CMD 1;
typedef	struct	s_cmd
{
	int				type;		// if (type == BUILD_IN) run in build_in();
	char			**cmd_args;      // cmd[0] == name
	struct s_cmd	*next;
}		t_cmd;

//==================  redirect_fd  =============================
//
//		will use same typedef for redir_input and redir_output;
//
//==============================================================

struct  s_redirect_fd
{
  int  from_fd;
  int  to_fd;
};

struct	s_redirect_file
{
	char	*file_name;
	int		redir_fd;
};	

#define	ERROR			0	// if type == ERROR  just throw error; () // will catch illegal error use (bad_expansion.. >*.c  || use & wrong place)
#define	REDIR_IN		1   // <
#define	REDIR_OUT		2	// >
#define APPEND			3	// >>
typedef struct s_redirect
{
	int	type;			// type define on top
	int	use_ampersand;  // ampersand = & :: check if it 1 & or 0 ; if 1 do dup(**, 1); dup(**, 2);
	int	use_filename;   // if yes it easy to check for free(file->filename); also easy to decide use redirect_file or redirect_fd; 
	struct s_redirect_file file;
	struct s_redirect_fd  fd;
	struct s_redirct  *next;
}  t_indirect;	// manual use is below

/*								

						logic ::	if (type == ERROR)
										throw error;
									 if (use_filename)
										open (file_name, ..... )



					condition logic ::  if (use_filename)
										{
											if (use_ampersand)
												dup(file_name, 1);
												dup(file_name, 2);
											el if (type == 1)
												dup(redir_fd, file_name);
											el if (type == 2)
												dup(file_name, redir_fd);
											el if (type == 3)
												do_append(file_name)
										}
										else
										{
											dup(to_fd, from_fd);
										}
*/



// -------------- version 2
//

#define BUILD_IN_CMD 1;

typedef struct s_cmd
{
	int type;		 // if (type == BUILD_IN) run in build_in();
	char **cmd_args; // cmd[0] == name
	char **token_args;	// dont use it. it is for parser part; // can print it for check is parsing part error?
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
	int		is_syntax_error; // default value == 0 ; if ( @ == 1 )  this whole t_exec will be throw syntax error (parser error)
	t_cmd *cmd;
	t_indirect *redirect;
	t_map_list
		*env; /* use if called minishell recursively ; // if have another use
				 case please tell me it will be ready to send to new env;  if
				 cmd is not "minishell" it will be null for now"*/
	struct s_exe *next;
} t_exec;

typedef struct s_exe_set
{
	int run_condition;
	t_exec *head;
	struct s_exec_set *next;
} t_exec_set;
