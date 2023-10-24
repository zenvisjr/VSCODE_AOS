* strtok is used to take multiple commands at once using ; as delimiter with many spaces as required
* pwd, echo and cd with all 4 flags are implemented
* all the cases of ls are handled with multiple direcrtory and multiple flags
* foreground and background process can be executed using fork and execvp system call
* pinfo is implemented and shows all the required information with + sign in status code if process is terminal itself
* search command is implemented to seearch under current directory recursively.
* all 4 case of I/O redirection [>, >>, <, < >] is handled
* pipelining is implemented that can handle multiple pipes and with input output redirection 
* all 3 signals are implemented where ctrl+D quits my custom terminal, ctrl+C quits the foreground process and ctrl_Z makes foreground process into background
autocomplete is implemented with TAB button under current directory 
* history is implemented that shows previous commands accross all sessions along with upper arrow to show previous history

points to notice:
* now tab do not work to give spaces in terminal 
* few process like nautilus (file explorer in ubuntu) and firefox for which terminal e.i parent process do not wait and print user@hostname again
* when using piping with output redirection, final output is written on output ile but still execvp shows perror
* typing a random process name and running it in background gives perror in execvp but terminal gets struck and do not move to next line



