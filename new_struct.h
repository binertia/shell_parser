typedef struct s_exec
{
	//int run_condition; // (|, ||, &&) if (run_condition == | ) do |; else if (run condition -compare current exit code) ? run : not run
	t_cmd *cmd;
	t_indirect *redirect;
	t_map_list *env;
	struct s_exec	*child; v
	struct s_exec	*next; ->
}	t_exec;

if (is_syntax_error)
	perror()
exucution(t_exec)

-- t_exec_set
// start recursive function with VVV
//if (child) > fork new and send ( s_exec child); its gonnabe recursive function




// send with head of s_exec >> int		is_syntax_error; // default value == 0 ; if ( @ == 1 )  this whole t_exec will be throw syntax error (parser error)


//    ls | ls
/*
*child == NULL
t_cmd *char = "ls""

t_exec -> next;

t_exec.run_condition "|"
s_exec *child == NULL;
*/
(ls || ls)
t_cmd->cmd ==

((ls) || (ls | (ls)) || (ls && (ls | ls)) ) | ls
//
//
t_exec cmd. NULL 
t_exec = ->

function(){
	condition (== && , == ||, |)
	
	if (child != NULL)
	{
		child = child->next;
		fork(child)
	}
	if (cmd->char * != NULL)
	exesev()
}

node tree

1 ->2 -> 3
	|
	v
	a -> b -> c -> d
		 |		   |
		 v		   v
		 $ -> %    jfkd
