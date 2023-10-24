#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <vector>
#include <pwd.h>
#include <grp.h>
#include <filesystem>
#include <fstream>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utility>
#include <readline/history.h>


using namespace std;

char curr_dir[500];
string home;
string current_directory;
string prev_dir;
vector<string> history;
//pid_t pid = -1;


char* string_to_Cstyle(string &input);
string trim_both(string& command);
string trim(string &input);
void print(vector<string>);
bool cd(string &command);
bool pwd(string& command);
bool echo(const string& command);

bool list(string &command);
bool ls();
bool ls_a();
bool ls_l();
bool ls_curr();
bool ls_home();
bool ls_prev();
bool ls_a_l();
bool ls_with_dir(string& command);
bool ls_l_with_dir(string& command);
bool ls_a_with_dir(string& command);
bool ls_a_l_with_dir(string& command);

string file_permission(mode_t mode);

bool search(string &command);
bool searchit(const filesystem::path& start_dir,string& input);

bool process_info(string& command);
bool retrieve_pinfo(int pid);

char** auto_complete(const char* text, int start, int end);
char* generator_function(const char* text, int state);


void foreground(vector<string>& arguments);
void background(vector<string>& arguments);

void extract(vector<string>& arguments, string& separator, vector<string>& command, vector<string>& output);
string find_separator(vector<string>& arguments);
//void ctrlC(int);
//void ctrlZ(int);
void ctrlD(int);

bool output_redirection(vector<string>& com, string& separator, vector<string>& output);
bool input_redirection(vector<string>& command, vector<string>& output);
bool piping(vector<string>&);
//void handle_sigchld(int);
bool input_output_redirection(vector<string>&, string&, vector<string>&,vector<string>&);
void call_exec(vector<string>);
pair<vector<string>, vector<string>> extract_pipe(vector<string>&);
int  hist_save();
int hist_read();
bool hist_retrieve();
bool hist_retrieve_num(int);

void tokenize_string(vector<string>&, string&);
int main()
{   
    //extracting username
    char *username = getenv("USER");                        //getenv() function returns a pointer to the value  in  the  environment, or NULL if there is no match
    if(username == nullptr)
    {
        cerr <<"Unable to fetch username"<<endl;
        return 1;
    }

    //extracting hostname
    char hostname[500];                                  //gethostname() returns the null-terminated hostname in the character array hostname, return 0 on success, -1 on error
    if(gethostname(hostname, sizeof(hostname)) == -1)
    {
        cerr <<"Unable to fetch hostname"<<endl;
        return 1;
    }

    ///extracting current directory
    getcwd(curr_dir, sizeof(curr_dir));                  //getcwd() return a null-terminated string containing an absolute pathname that is the current working directory of the calling process.
    if(curr_dir == nullptr)
    {
        cerr <<"Unable to fetch current directory"<<endl;
        return 1;
    }
    //cout<<curr_dir<<endl;

    //checking if current directory is home directory
    char *h = getenv("HOME");                     
    if(h == nullptr)
    {
        cerr <<"Unable to fetch home"<<endl;
        return 1;
    }
    
    home = curr_dir;
    prev_dir = curr_dir;
    //cout<<prev_dir<<endl;
    current_directory = (curr_dir == home)? "~": curr_dir;
    
    rl_attempted_completion_function = auto_complete; // This is a pointer to a function provided by the readline library and we are paing address of our custom made function 
    signal(SIGINT, SIG_IGN);  
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, ctrlD);
    //signal(SIGCHLD, handle_sigchld);

    hist_read();
    
    while(true)
    {
        //cout<<username<<"@"<<hostname<<":"<<current_directory<<"> ";

        //string input;
        //getline(cin, input);

        char* line = readline((username + string("@") + hostname + string(":") + current_directory + string("> ")).c_str());
        // If the user presses Ctrl-D (EOF), readline returns a nullptr
       //cout<<"hyy22"<<endl;
        if (line == nullptr) 
        {  raise(SIGQUIT);
            break;
        }
        string input(line);
        //free(in);  // Free the allocated memory by readline

        // Add to history if not empty
        if (!input.empty()) 
        {
            add_history(input.c_str()); //add_history is a function from the readline library and accepts c style string (char*) as its arguments
        }
        char * a = string_to_Cstyle(input);

        //char *a = new char[input.length()+1];
        //strcpy(a, input.c_str());
        char * in = strtok(a, ";");
        while(in)
        {
            //cout<<in<<endl;
            string command(in);
            command = trim_both(command);
            history.push_back(command);

            //cout<<command<<endl;

            vector<string> arguments;
            tokenize_string(arguments, command);
            if(arguments.back() == "&")
            {
                arguments.pop_back();
                background(arguments);
                
            }
            else 
            {
                //string find = ">";
                string separator = find_separator(arguments);
                vector<string> com;
                vector<string> output;
                extract(arguments, separator, com, output);
                string separator2 = find_separator(output);

                if(separator == "|" || (separator == "<" && separator2 == "|"))
                {
                    //cout<<"entring pipe"<<endl;
                    piping(arguments);
                }
                else if(!separator.empty())
                {
                    //cout<<command<<endl;
                    vector<string> com;
                    vector<string> output;
                    
                    extract(arguments, separator, com, output);
                    // cout<<"printing command"<<endl;
                    // print(com);
                    // cout<<"printing arguments"<<endl;
                    // print(output);
                    
                    //checking if another separator exist
                    string separator2 = find_separator(output);
                    //cout<<"separator2: "<<separator2<<endl;
                    if(!separator2.empty())
                    {
                        //cout<<command<<endl;
                        vector<string> output1;
                        vector<string> output2;
                        
                        extract(output, separator2, output1, output2);
                        // cout<<"printing output1"<<endl;
                        // print(output1);
                        // cout<<"printing output2"<<endl;
                        // print(output2);

                        
                    
                        //cout << "command: " << com<< endl;
                        //cout << "file: " << output << endl;
                        //cout<<"hyy"<<endl;
                        
                        if(separator == "<" && (separator2 == ">"|| separator2 == ">>"))
                        {
                            //cout<<"input_output"<<endl;
                            input_output_redirection(com, separator2, output1, output2);
                        }
                    }
                    else
                    {
                    
                        if(separator == ">" || separator == ">>")
                        {
                            
                            output_redirection(com, separator, output);
                        }
                        
                        if(separator == "<")
                        {
                            input_redirection(com, output);
                        }
                        
                        // if(separator == "|")
                        // {
                        //     cout<<"entring pipe"<<endl;
                        //     piping(com, output);
                        // }
                    }
                }

                else if(command.substr(0,2) == "cd")
                {
                    int i = cd(command);
                    if(i==0)
                    {
                        cerr <<"unable to process cd command"<<endl;
                    }
                }

                else if(command.substr(0,2) == "ls")
                {
                    int i = list(command);
                if(i==0)
                    {
                        cerr <<"unable to process ls command"<<endl;
                    }
                }

                else if(command == "pwd")
                {
                    int i = pwd(command);
                    if(i==0)
                    {
                        cerr <<"unable to process pwd command"<<endl;
                    }
                }

                else if(command.substr(0,4) == "echo")
                {
                    int i = echo(command);
                    if(i==0)
                    {
                        cerr <<"unable to process echo command"<<endl;
                    }
                }
                
                else if(command.substr(0,6) == "search")
                {
                    //cout<<"hyyyy"<<endl;
                    int i = search(command);
                    if(i==0)
                    {
                        cerr <<"unable to process echo command"<<endl;
                    }
                }

                else if(command.substr(0,5) == "pinfo")
                {
                    //cout<<"hyyyy"<<endl;
                    int i = process_info(command);
                    if(i==0)
                    {
                        cerr <<"unable to process pinfo command"<<endl;
                    }
                }
                else if(command.substr(0,7) == "history")
                {
                    if(command.length() == 7)
                    {
                        int i = hist_retrieve();
                        if(i==0)
                        {
                            cerr <<"unable to process history command"<<endl;
                        }
                    }
                    else
                    {
                        command = command.substr(8);
                        int num = stoi(command);
                        //cout<<num<<endl;
                        int i = hist_retrieve_num(num);
                        if(i==0)
                        {
                            cerr <<"unable to process history command"<<endl;
                        }

                    }   
                }
                else 
                {
                    foreground(arguments);
                }
            }

           // cout<<"printing arguments"<<endl;
            //print(arguments);

            in = strtok(NULL, ";");
        }
    
    }
    hist_save(); 
}

// bool piping(vector<string>& command1, vector<string>& command2)
// {
//     cout<<"inside pipe"<<endl;
//     //int pip[2] is an array of two integers that will hold the file descriptors for the read end (pipefd[0]) and the write end (pipefd[1]) of the pipe
//     int pip[2];
//     pipe(pip);
//     pid_t pid1;
//     pid1 = fork();

//     if(pid1<0)
//     {
//         perror("unable to create 1st child process");
//         return false;
//     }

//     if(pid1==0)
//     {
//         close(pip[0]);
//         if(dup2(pip[1], STDOUT_FILENO) == -1)
//         {
//             perror("unable to redirect output to pipe");
//             exit(EXIT_FAILURE);
//         }
//         close(pip[1]);
//         //cout<<"after dup"<<endl;
//         call_exec(command1);
//         exit(0);
//     }

//     pid_t pid2;
//     pid2 = fork();

//     if(pid2<0)
//     {
//         perror("unable to create 2nd child process");
//         return false;
//     }

//     if(pid2==0)
//     {
//         close(pip[1]);
//         if(dup2(pip[0], STDIN_FILENO) == -1)
//         {
//             perror("unable to redirect input to pipe");
//             exit(EXIT_FAILURE);
//         }
//         close(pip[0]);
//         cout<<"entering 2nd command piping"<<endl;
//         call_exec(command2);

//         //exit(0);
//         cout<<"exiting piping"<<endl;
//     }
//     //closing file descriptors of pipe in parent process (not necessary but good practice)
//     close(pip[0]);
//     close(pip[1]);
//     //waiting for child process 1 and 2 to terminate
//     waitpid(pid1, NULL, 0);
//     waitpid(pid2, NULL, 0);
// }

bool piping(vector<string>& command)
{
    //cout<<"ebtering piping"<<endl;
    vector<pid_t> pid;
    int pip[2];
    int fd=0;
    //string sep = "|";
    string check = find_separator(command);
    if(check == "<")
    {
        pair<vector<string>, vector<string>> com = extract_pipe(command);
        //cout<<(com.second[0])<<endl;
        print(com.first);
        print(com.second);
        fd = open(com.second[0].c_str(), O_RDONLY);
        //cout<<fd<<endl;
        if(fd== -1)
        {
            perror("file do not exist");
            exit(EXIT_FAILURE);
        }
        command = com.second;
        command.erase(command.begin(), command.begin() + 2);
        print(command);

    }
    while(true)
    {
        string check = find_separator(command);
        pair<vector<string>, vector<string>> com = extract_pipe(command);

    //int pip[2] is an array of two integers that will hold the file descriptors for the read end (pipefd[0]) and the write end (pipefd[1]) of the pipe
        
        pipe(pip);           //A new pipe is created for communication between the current command and the next command in the chain.
        pid_t pid1;
        pid1 = fork();

        if(pid1<0)
        {
            perror("unable to create 1st child process");
            return false;
        }

        if(pid1==0)
        {
                close(pip[0]);
                if(dup2(fd, STDIN_FILENO) == -1)  //You can use STDIN_FILENO when you want to redirect input from a different source, such as a file or another process.
                {
                    perror("unable to redirect input from standard input e.i terminal screen");
                    exit(EXIT_FAILURE);
                    return false;
                }
                close(fd);
            if(!com.second.empty())
            {
                if(check == ">" || check == ">>")
                {
                    
                    int fd;
                    vector<string> temp = com.second;
                    //cout<<"hy1"<<endl;
                    //print(com.first);
                    //cout<<"hy2"<<endl;
                    
                    //print(temp);
                    //cout<<temp[0]<<endl;
                    if(check == ">") fd = open(temp[0].c_str(), O_CREAT | O_WRONLY  | O_TRUNC, 0644);
                    else fd = open(temp[0].c_str(), O_WRONLY  | O_APPEND);
                    //cout<<fd<<endl;
                    if(fd== -1)
                    {
                        perror("unable to open file");
                        exit(EXIT_FAILURE);
                    }
                    //int ofd;
                    //ofd = dup(STDOUT_FILENO);
                    if(dup2(fd, STDOUT_FILENO) == -1)
                    {
                        perror("unable to write on output file");
                        exit(EXIT_FAILURE);
                    }

                }
                else
                {
                    if(dup2(pip[1], STDOUT_FILENO) == -1) //You can use STDOUT_FILENO when you want to redirect output to a different destination, such as a file or a pipe.
                    {
                        perror("unable to redirect output to pipe");
                        exit(EXIT_FAILURE);
                        return false;
                    }
                }
                
                close(pip[1]);   //Close the write-end of the pipe after redirection to signal the next command that there's no more data when the current command finishes execution
            }
            
            //cout<<"after dup"<<endl;
            call_exec(com.first);
            exit(0);
        }
        else
        {
            if(pid1>0)
            {
                if(fd !=0) close(fd);

                close(pip[1]);
                fd = pip[0];


                pid.push_back(pid1);

                if(com.second.empty())
                {
                    //call_exec(com.first);
                    break;
                }
                
               command = com.second;
            }
           
        }
     }
        //closing file descriptors of pipe in parent process (not necessary but good practice)
        
        //waiting for child process 1 and 2 to terminate
        //waitpid(pid1, NULL, 0);
        //waitpid(pid2, NULL, 0);
        for(auto v: pid)
        {
            waitpid(v, NULL, 0);
        }
        return true;
    
}

pair<vector<string>, vector<string>> extract_pipe(vector<string>& arguments)
{
    //print(arguments);
    vector<string> command1;
    vector<string> command2;
    string sep1 = "|";
    string sep2 = ">";
    string sep3 = ">>";
    string sep4 = "<";
    // pair<vector<string>, vector<string> > = com_pair;
    // com_pair.first = command1;
    // com_pair.first = command2;
    bool find_sep = false;
    

    for(auto v : arguments)
    {
        if((v == sep1 || v == sep2 || v == sep3 || v == sep4) && find_sep == false) 
        {
            find_sep = true;
            continue;
        }
        if(find_sep == false)
        {
            //cout<<"1"<<endl;
            command1.push_back(v);
            //com += " ";
            //cout<<"2"<<endl;
        }
        else
        {
            command2.push_back(v);
            //output += " ";
        }
        
    }
    //print(command1);
    //print(command2);
    return make_pair(command1, command2);
}


bool output_redirection(vector<string>& command, string& sep, vector<string>& output)
{
    //cout<<"inside output redirection"<<endl;
    pid_t pid;
    pid = fork();
    //cout<<pid<<endl;

    if(pid<0)
    {
        perror("unable to create child process");
        return 0;
    }

    if(pid==0)
    {
        int fd;
        if(sep == ">") fd = open(output[0].c_str(), O_CREAT | O_WRONLY  | O_TRUNC, 0644);
        else fd = open(output[0].c_str(), O_WRONLY  | O_APPEND);
        //cout<<fd<<endl;
        if(fd== -1)
        {
            perror("unable to open file");
            exit(EXIT_FAILURE);
        }
        //int ofd;
        //ofd = dup(STDOUT_FILENO);
        if(dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("unable to write on output file");
            exit(EXIT_FAILURE);
        }
            //cout<<"after dup"<<endl;
        //system("ls -l");

        call_exec(command);
        
        //cout<<"after ls"<<endl;
        
        close(fd);
       // close(ofd);
        exit(0);
    }
    else
    {
        wait(NULL);
        //cout<<"hyhyhyh"<<endl;
    }
   
return true;
}

// void handle_sigchld(int sig) {
//     while (waitpid(-1, NULL, WNOHANG) > 0);
// }
bool input_redirection(vector<string>& command, vector<string>& output)
{
    //cout<<"inside input redirection"<<endl;
    //int pip[2];
    pid_t pid;
    pid = fork();

    if(pid<0)
    {
        perror("unable to create child process");
        return 0;
    }

    if(pid==0)
    {
        int fd;
        fd = open(output[0].c_str(), O_RDONLY);
    
        //cout<<fd<<endl;
        if(fd== -1)
        {
            perror("file do not exist");
            exit(EXIT_FAILURE);
        }
        //int ofd;
        //ofd = dup(STDOUT_FILENO);
        if(dup2(fd, STDIN_FILENO) == -1)
        {
            perror("unable to write on output file");
            exit(EXIT_FAILURE);
        }
            //cout<<"after dup"<<endl;
        //system("ls -l");

        call_exec(command);
        
        //cout<<"after ls"<<endl;
        
        close(fd);
       // close(ofd);
        exit(0);
    }
    else
    {
        wait(NULL);
        //cout<<"hyhyhyh"<<endl;
    }
   
return true;
}

bool input_output_redirection(vector<string>& command,string& sep, vector<string>& input,vector<string>& output)
{
    //cout<<"inside input output redirection"<<endl;
    pid_t pid;
    pid = fork();

    if(pid<0)
    {
        perror("unable to create child process");
        return 0;
    }

    if(pid==0)
    {
        //cout<<"inside input output redirection child process"<<endl;
        //taking input from input file to command
        int fd1;
        fd1 = open(input[0].c_str(), O_RDONLY);
    
        //cout<<fd1<<endl;
        if(fd1== -1)
        {
            perror("file do not exist");
            exit(EXIT_FAILURE);
        }
        //int ofd;
        //ofd = dup(STDOUT_FILENO);
        if(dup2(fd1, STDIN_FILENO) == -1)
        {
            perror("unable to write on output file");
            exit(EXIT_FAILURE);
        }
        //cout<<"after dup"<<endl;
        close(fd1);
        //cout<<"inside"<<endl;
        //writing output of command to output file
        int fd2;
        //cout<<"output: "<< output[0]<<endl;
        //cout<<sep<<endl;
        //cout<<"output: "<< output[0]<<endl;
        if(sep == ">") fd2 = open(output[0].c_str(), O_CREAT | O_WRONLY  | O_TRUNC, 0644);
        else fd2 = open(output[0].c_str(), O_WRONLY  | O_APPEND);
        //cout<<sep<<endl;
        //cout<<fd2<<endl;
        if(fd2 == -1)
        {
            perror("unable to open file");
            exit(EXIT_FAILURE);
        }
        //int ofd;
        //ofd = dup(STDOUT_FILENO);
        if(dup2(fd2, STDOUT_FILENO) == -1)
        {
            perror("unable to write on output file");
            exit(EXIT_FAILURE);
        }
        //cout<<"after dup"<<endl;
        close(fd2);
        //cout<<"outside"<<endl;
        call_exec(command);
    }
    else
    {
        wait(NULL);
        //cout<<"hyhyhyh"<<endl;
    }
   
return true;
}









bool process_info(string& command)
{
    if(command.length() == 5) 
    {
        int pid = getpid();
        retrieve_pinfo(pid);
        return true;
    }
    else 
    {
        command = command.substr(5);
        command = trim(command);
        retrieve_pinfo(stoi(command));
        return true;
    }
}


bool retrieve_pinfo(int pid)
{
    pid_t pgid = tcgetpgrp(STDIN_FILENO);
    //cout<<pgid<<endl;

    //getting exectutable path
    string exec_path = "/proc/" + to_string(pid) + "/exe";
    //cout<<exec_path<<endl;
    char real_path[500];
    if(realpath(exec_path.c_str(), real_path) == NULL)
    {
        perror("Unable to retrieve executable path");
        return false;
    }

    //getting process status
    string status_path = "/proc/" + to_string(pid) + "/status";
    //cout<<status_path<<endl;
    ifstream sp(status_path);             //input file stream 
    string status;
    while(getline(sp, status))
    {
        if(status.substr(0,6) == "State:")
        {
            
            status = status.substr(6);
            status = trim(status);
            if(pgid == pid)
            {
                status.insert(1, "+");
            }
            break;
        }
    }

    //getting memory information
    string mem_path = "/proc/" + to_string(pid) + "/statm";
    //cout<<mem_path<<endl;
    ifstream m(mem_path);             //input file stream 
    string memory;
    getline(m, memory, ' ');

    cout<<"pid -- "<<pid<<endl;
    cout<<"Process Status -- "<<status<<endl;
    cout<<"Memory -- "<<memory<<" {Virtual Memory}"<<endl;
    cout<<"Executable path -- "<<real_path<<endl;

    return true;
}

char* string_to_Cstyle(string &input)
{
    char *a = new char[input.length()+1];
    strcpy(a, input.c_str());
    return a;

}

bool list(string &command)
{
    if(command.length() == 2) 
    {
        ls();
        return true;
    }
    else 
    {
        command = command.substr(3);
        command = trim(command);
        vector<string> res;
        istringstream yo(command);
        string arg;
        string temp;

        while (yo >> arg) 
        {
            if (arg == "-a" || arg == "-l") 
            {
                if (!temp.empty()) 
                {
                    temp += " ";
                }
                temp += arg;
            } 
            else 
            {
                if (!temp.empty()) 
                {
                    res.push_back(temp);
                    temp.clear();
                }
                res.push_back(arg);
            }
        }

        // Handle any remaining flags
        if (!temp.empty()) 
        {
            res.push_back(temp);
        }

        //print(res);
        int size = res.size();
        //cout<<size<<endl;
        
        if(size == 1)
        {
            if(res[0] == "-a")  ls_a();

            else if(res[0] == "-l")  ls_l();
            
            else if(res[0] == ".")  ls_curr();

            else if(res[0] == "..")  ls_prev();

            else if(res[0] == "~")  ls_home();

            else if(res[0] == "-a -l")  ls_a_l();

            else if(res[0] == "-al" || res[0] == "-la" )  ls_a_l();

            else ls_with_dir(res[0]);
        }
        else if(size == 2)
        {
            if(res[0] == "-a")  ls_a_with_dir(res[1]);

            else if(res[0] == "-l")  ls_l_with_dir(res[1]);

            else if(res[0] == "-a -l" || res[0] == "-al" || res[0] == "-la" )  ls_a_l_with_dir(res[1]);

            else 
            {
                
                cout<<res[0]<<":"<<endl;
                cout<<endl;
                ls_with_dir(res[0]);
                cout<<endl;
                cout<<res[1]<<":"<<endl;
                cout<<endl;
                ls_with_dir(res[1]);
            }
        }
        else
        {
            cout<<"entering inside else"<<endl;
            int i=0;
            bool flag2 = false;
            string sym;
            while(i<size)
            {
                bool flag = false;
                
                

                if(res[i] == "-a")  
                { 
                    cout<<endl;
                    cout<<res[i+1]<<":"<<endl; 
                    cout<<endl;
                    ls_a_with_dir(res[i+1]); 
                    flag = true; 
                    flag2 = true;
                    sym = res[i];
                }

                else if(res[i] == "-l")  
                { 
                    cout<<endl;
                    cout<<res[i+1]<<":"<<endl;
                    cout<<endl;
                    ls_l_with_dir(res[i+1]); 
                    flag = true; 
                    flag2 = true;
                    sym = res[i];
                }
                
                else if(res[i] == "-a -l" || res[i] == "-al" || res[i] == "-la" ) 
                { 
                    cout<<endl;
                    cout<<res[i+1]<<":"<<endl; 
                    cout<<endl;
                    ls_a_l_with_dir(res[i+1]); 
                    flag = true; 
                    flag2 = true;
                    sym = res[i];
                }

                else
                {
                    if(flag2 == false)
                    {
                        cout<<endl;
                        cout<<res[i]<<":"<<endl;
                        cout<<endl;
                        ls_with_dir(res[i]);
                    }
                    else
                    {
                        if(sym == "-a")
                        {
                            cout<<endl;
                            cout<<res[i]<<":"<<endl; 
                            cout<<endl;
                            ls_a_with_dir(res[i]);
                        }
                        if(sym == "-l")
                        {
                            cout<<endl;
                            cout<<res[i]<<":"<<endl; 
                            cout<<endl;
                            ls_l_with_dir(res[i]);
                        }
                        if(sym == "-al" || sym == "-la" ||sym == "-a -l")
                        {
                            cout<<endl;
                            cout<<res[i]<<":"<<endl; 
                            cout<<endl;
                            ls_a_l_with_dir(res[i]);
                        }
                    }
                    
                    
                }
                
                if(flag == true) i = i+2;
                else i++;
            }
            
        }
        

        // command = command.substr(3);
        // command = trim(command);
     
        // if(command == "-a")  ls_a();

        // else if(command == "-l")  ls_l();
        
        // else if(command == ".")  ls_curr();

        // else if(command == "..")  ls_prev();

        // else if(command == "~")  ls_home();

        // else if(command == "-a -l")  ls_a_l();

        // else if(command == "-al" || command == "-la" )  ls_a_l();

        // else if(command.substr(0,5) == "-a -l" && command.length()>5)
        // {
        //     cout<<"1 "<<command<<endl;
        //     command = command.substr(6);
            
        //     command = trim(command);
        //     cout<<"2 "<<command<<endl;
        //     ls_a_l_with_dir(command);
        // }

        // else if(command.substr(0,2) == "-a" && command.substr(2,1) == " " && command.substr(3,2) != "-l" && command.length()>2)
        // {
            
        //     cout<<"bhai ya"<<endl;
        //     command = command.substr(3);
        //     command = trim(command);
        //     ls_a_with_dir(command);
        // }

        // else if(command.substr(0,2) == "-l" && command.substr(2,1) == " " && command.substr(3,2) != "-a" &&  command.length()>2)
        // {
        //     command = command.substr(3);
        //     command = trim(command);
        //     ls_l_with_dir(command);
        // }

         
        // else if((command.substr(0,3) == "-la" || command.substr(0,3) == "-al") && command.length()>3)
        // {
        //     cout<<"4 "<<command<<endl;
        //     command = command.substr(4);
        //     command = trim(command);
        //     cout<<"5 "<<command<<endl;
        //     ls_a_l_with_dir(command);
        // }
        
        // else
        // {
        //     ls_with_dir(command);
        // }
    }
    return true;
}

bool ls()
{
    DIR *d;                       //DIR represent directory stream
    struct dirent *dir;           //it represent a single entry in a directory and store various information about that entry (file)
    //cout<<current_directory<<endl;
    d = opendir(curr_dir);
    if( d == nullptr)                                  //The  opendir()  function  opens a directory stream corresponding to the directory name, and returns a pointer to  the  directory  stream. The stream is positioned at the first entry in the directory
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;
        cout<<dir->d_name<<endl;
    }
    return true;
}

bool ls_with_dir(string& path)
{
  
    struct stat check;

    int i= stat(path.c_str(), &check);
    if (i==0)
    {
        if(check.st_mode & S_IFDIR)
        {
            DIR *d;                       //DIR represent directory stream
            struct dirent *dir;           //it represent a single entry in a directory and store various information about that entry (file)
            //cout<<current_directory<<endl;
            d = opendir(path.c_str());
            if( d == nullptr)                                  //The  opendir()  function  opens a directory stream corresponding to the directory name, and returns a pointer to  the  directory  stream. The stream is positioned at the first entry in the directory
            {
                perror("directory cannot be opened: ERRORRR");
                return false;
            }
            while((dir = readdir(d)) != nullptr )
            {
                string name = dir->d_name;
                if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;
                cout<<dir->d_name<<endl;
            }
            return true;
        }
        else
        {
            int point = path.find_last_of("/");
            string file_name = path.substr(point+1);
            cout<<file_name<<endl;
            return true;
        }
        
    }
    else
    {
        perror("invalid path: path do not exist:");
        return false;
    }
    
    
}
bool ls_curr()
{
    DIR *d;                       //DIR represent directory stream
    struct dirent *dir;           //it represent a single entry in a directory and store various information about that entry (file)
    //cout<<current_directory<<endl;
    d = opendir(curr_dir);
    if( d == nullptr)                                  //The  opendir()  function  opens a directory stream corresponding to the directory name, and returns a pointer to  the  directory  stream. The stream is positioned at the first entry in the directory
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        //if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;
        cout<<dir->d_name<<endl;
    }
    return true;
}

bool ls_home()
{
    DIR *d;                       //DIR represent directory stream
    struct dirent *dir;           //it represent a single entry in a directory and store various information about that entry (file)
    //cout<<current_directory<<endl;
    d = opendir(home.c_str());
    if( d == nullptr)                                  //The  opendir()  function  opens a directory stream corresponding to the directory name, and returns a pointer to  the  directory  stream. The stream is positioned at the first entry in the directory
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        //if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;
        cout<<dir->d_name<<endl;
    }
    return true;
}

bool ls_prev()
{
    DIR *d;                       //DIR represent directory stream
    struct dirent *dir;           //it represent a single entry in a directory and store various information about that entry (file)
    //cout<<current_directory<<endl;


    string prev = curr_dir;
    //cout<<prev<<endl;
    int e = prev.find_last_of("/");
    prev = prev.substr(0, e);
    //cout<<s<<endl;
    //cout<<e<<endl;
    //int z = e-s+1;
    //cout<<prev<<endl;
    //cout<<z<<endl;

    d = opendir(prev.c_str());
    if( d == nullptr)                                  //The  opendir()  function  opens a directory stream corresponding to the directory name, and returns a pointer to  the  directory  stream. The stream is positioned at the first entry in the directory
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;
        cout<<dir->d_name<<endl;
    }
    return true;
}


bool ls_a()
{
    DIR *d;                       
    struct dirent *dir;           
    d = opendir(curr_dir);
    if( d == nullptr)                            
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        //if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        cout<<dir->d_name<<endl;
    }
    return true;
}

bool ls_a_with_dir(string &path)
{
    struct stat check;

    int i= stat(path.c_str(), &check);
    if (i==0)
    {
        if(check.st_mode & S_IFDIR)
        {
            DIR *d;                       
            struct dirent *dir;           
            d = opendir(path.c_str());
            if( d == nullptr)                            
            {
                perror("directory cannot be opened: ERRORRR");
                return false;
            }
            while((dir = readdir(d)) != nullptr )
            {
                //if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
                cout<<dir->d_name<<endl;
            }
            return true;
        }
        else
        {
            int point = path.find_last_of("/");
            string file_name = path.substr(point+1);
            cout<<file_name<<endl;
            return true;
        }
        
    }
    else
    {
        perror("invalid path: path do not exist:");
        return false;
    }

//     FILE* file = fopen(command.c_str(), "r");
//    //cout<<"hy"<<endl;
//     if(file == NULL)
//     {
//      perror("unable to open file due to some failure");
//      //return 0;
//     }

    
}

string file_permission(mode_t mode) 
{
    string permissions;
    
    // File type
    if (S_ISDIR(mode)) permissions += "d";
    else if (S_ISREG(mode)) permissions += "-";
    else if (S_ISLNK(mode)) permissions += "l";
    else if (S_ISCHR(mode)) permissions += "c";
    else if (S_ISBLK(mode)) permissions += "b";
    else if (S_ISFIFO(mode)) permissions += "p";
    else if (S_ISSOCK(mode)) permissions += "s";

    // User permissions
    permissions += (mode & S_IRUSR) ? "r" : "-";
    permissions += (mode & S_IWUSR) ? "w" : "-";
    permissions += (mode & S_IXUSR) ? "x" : "-";

    // Group permissions
    permissions += (mode & S_IRGRP) ? "r" : "-";
    permissions += (mode & S_IWGRP) ? "w" : "-";
    permissions += (mode & S_IXGRP) ? "x" : "-";

    // Others permissions
    permissions += (mode & S_IROTH) ? "r" : "-";
    permissions += (mode & S_IWOTH) ? "w" : "-";
    permissions += (mode & S_IXOTH) ? "x" : "-";

    return permissions;
}


bool ls_l()
{
    //cout<<"entering ls -l"<<endl;
    DIR *d;                       
    struct dirent *dir; 
    struct stat status;
    if(stat(curr_dir, &status) == -1)
    {
        perror("unable to retrieve stat information");
    }          
    d = opendir(curr_dir);
    if( d == nullptr)                            
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;

        string path;
        path = string(curr_dir) + "/" + name;

        if(stat(path.c_str(), &status) == -1)
    {
        perror("unable to retrieve stat information");
        continue;
    }   

        cout << file_permission(status.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
        cout << status.st_nlink << " ";
        cout << getpwuid(status.st_uid)->pw_name << " ";
        cout << getgrgid(status.st_uid)->gr_name << " ";
        cout << setw(8) << to_string(status.st_size) << " ";
        string time = ctime(&status.st_mtime);
        time = time.substr(4, 12);      //remove new line
        cout << time <<" ";
        cout<<dir->d_name<<endl;
    }
    return true;
}


bool ls_a_l()
{
    //<<"entering detail"<<endl;
    DIR *d;                       
    struct dirent *dir; 
    struct stat status;
        
    d = opendir(curr_dir);
    if( d == nullptr)                            
    {
        perror("directory cannot be opened: ERRORRR");
        return false;
    }
    while((dir = readdir(d)) != nullptr )
    {
        string name = dir->d_name;
        //if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;

        string path;
        path = string(curr_dir) + "/" + name;

        if(stat(path.c_str(), &status) == -1)
    {
        perror("unable to retrieve stat information");
        continue;
    }   

        cout << file_permission(status.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
        cout << setw(2) << status.st_nlink << " ";
        cout << getpwuid(status.st_uid)->pw_name << " ";
        cout << getgrgid(status.st_uid)->gr_name << " ";
        cout << setw(8) << to_string(status.st_size) << " ";
        string time = ctime(&status.st_mtime);
        time = time.substr(4, 12);        //remove new line
        cout << time <<" ";
        cout<<dir->d_name<<endl;
    }
    return true;
}


bool ls_a_l_with_dir(string& command)
{



    struct stat check;

    int i= stat(command.c_str(), &check);
    if (i==0)
    {
        if(check.st_mode & S_IFDIR)
        {
            DIR *d;                       
            struct dirent *dir; 
            struct stat status;
                
            d = opendir(command.c_str());
            if( d == nullptr)                            
            {
                perror("directory cannot be opened: ERRORRR");
                return false;
            }
            while((dir = readdir(d)) != nullptr )
            {
                string name = dir->d_name;
               // if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;

                string path;
                path = command + "/" + name;

                if(stat(path.c_str(), &status) == -1)
            {
                perror("unable to retrieve stat information");
                continue;
            }   

                cout << file_permission(status.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
                cout << setw(3) << status.st_nlink << " ";
                cout << getpwuid(status.st_uid)->pw_name << " ";
                cout << getgrgid(status.st_uid)->gr_name << " ";
                cout << setw(6) << to_string(status.st_size) << " ";
                string time = ctime(&status.st_mtime);
                time = time.substr(4, 12);        //remove new line
                cout << time <<" ";
                cout<<dir->d_name<<endl;
            }
            return true;
        }
        else if (check.st_mode & S_IFREG)
        {
            //int point = command.find_last_of("/");
            //string file_name = command.substr(point+1);

            cout << file_permission(check.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
            cout << setw(2) << check.st_nlink << " ";
            cout << getpwuid(check.st_uid)->pw_name << " ";
            cout << getgrgid(check.st_uid)->gr_name << " ";
            cout << setw(6) << to_string(check.st_size) << " ";
            string time = ctime(&check.st_mtime);
            time = time.substr(4, 12);        //remove new line
            cout << time <<" ";
            cout<<command<<endl;

            return true;
        } 
    }
    else
    {
        perror("invalid path: path do not exist:");
        return false;
    }
    return true;
}


bool ls_l_with_dir(string &command)
{
    //cout<<"3 "<<command<<endl;
    
    struct stat check;

    int i= stat(command.c_str(), &check);
    if (i==0)
    {
        if(check.st_mode & S_IFDIR)
        {
            DIR *d;                       
            struct dirent *dir; 
            struct stat status;
                
            d = opendir(command.c_str());
            if( d == nullptr)                            
            {
                perror("directory cannot be opened: ERRORRR");
                return false;
            }
            while((dir = readdir(d)) != nullptr )
            {
                string name = dir->d_name;
                if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || name.front() == '.') continue;

                string path;
                path = command + "/" + name;

                if(stat(path.c_str(), &status) == -1)
            {
                perror("unable to retrieve stat information");
                continue;
            }   

                cout << file_permission(status.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
                cout << setw(3) << status.st_nlink << " ";
                cout << getpwuid(status.st_uid)->pw_name << " ";
                cout << getgrgid(status.st_uid)->gr_name << " ";
                cout << setw(8) << to_string(status.st_size) << " ";
                string time = ctime(&status.st_mtime);
                time = time.substr(4, 12);        //remove new line
                cout << time <<" ";
                cout<<dir->d_name<<endl;
            }
            return true;
        }
        else if (check.st_mode & S_IFREG)
        {
            //int point = command.find_last_of("/");
            //string file_name = command.substr(point+1);

            cout << file_permission(check.st_mode)<< " ";   //st_mode contains the file's mode (permissions and file type information)
            cout << setw(2) << check.st_nlink << " ";
            cout << getpwuid(check.st_uid)->pw_name << " ";
            cout << getgrgid(check.st_uid)->gr_name << " ";
            cout << setw(8) << to_string(check.st_size) << " ";
            string time = ctime(&check.st_mtime);
            time = time.substr(4, 12);        //remove new line
            cout << time <<" ";
            cout<<command<<endl;

            return true;
        }
        
    }
    else
    {
        perror("invalid path: path do not exist:");
        return false;
    }
    return true;
}
















string trim(string &input)
{
    int s = input.find_first_not_of(" \t");
    input = input.substr(s);
    return input;
}

string trim_both(string& command)
{
    size_t s = command.find_first_not_of(" \t");
    size_t e = command.find_last_not_of(" \t");
    //cout<<s<<endl;
    //cout<<e<<endl;
    //int z = e-s+1;
    //cout<<z<<endl;

    if(s != string::npos && e != string::npos)
    {
        command = command.substr(s, e-s+1);
    }
    else
    {
        cout<<"not a valid commamd"<<endl;
    }
    return command;
}

bool cd(string &command)
{
    string path;
    if(command.size() == 2)
        {
            path = home;
        }
        else
        {
            path = command.substr(3);
            path = trim(path);
        }
        //cout<<"hi"<<endl;
        
        if(path == "~")
        {
            path = home;
        }

        if(path == "." )
        {
            path = curr_dir;
        }

        if(path == "-")
        {
            path = prev_dir;
        }
        prev_dir = curr_dir;

        int i = chdir(path.c_str());                                     //chdir() changes the current working directory of the calling process to the directory specified in path
        if(i!=0)
        {
            cerr<<"Unable to change current path"<<endl;
            return false;
        }
    
        getcwd(curr_dir, sizeof(curr_dir)); 
        //cout<<curr_dir<<endl;  
        //cout<<home<<endl;           
        if(curr_dir == nullptr)
        {
            cerr <<"Unable to fetch current directory"<<endl;
            return false;
        }

        if(strncmp(curr_dir, home.c_str(), home.size())==0)
        {
            current_directory = "~" + string(curr_dir+home.size());
            //cout<<"~ wali "<<current_directory<<endl;
        }
        else
        {
            //string home = curr_dir;
            //current_directory = curr_dir;
            current_directory = (strcmp(curr_dir, home.c_str())==0)? "~": curr_dir;
        }   
        //cout<<current_directory<<endl;
        //cout<<username<<"@"<<hostname<<":"<<current_directory<<"> ";
        return true;
}




bool pwd(string& command)
{
    getcwd(curr_dir, sizeof(curr_dir));
    if(curr_dir == nullptr)
    {
        cerr <<"Unable to fetch current directory"<<endl;
        return false;
    }

    cout<<curr_dir<<endl;
    return true;
}

bool echo(const string& command)
{
    string text;
    if(command.size() == 4)
    {
        cout<<endl;
        return true;
    }
    else if(command.substr(0, 5) == "echo ")
    {
        text = command.substr(5);
        text = trim(text);
        /*if (text.front() == '"' && text.back() == '"') 
        {
            text = text.substr(1, text.length() - 2);
        }
        else if (text.front() == '\'' && text.back() == '\'') 
        {
            text = text.substr(1, text.length() - 2);
        }*/
        //cout<<"echoooooo"<<endl;
        cout<<text<<endl;
        return true;
    }
    else return false;
}


bool search(string &command)
{
    command = command.substr(7);
    command = trim(command);
    //cout<<"search: "<<command<<endl;

    bool result = searchit(filesystem::current_path(), command);
    cout << (result ? "True" : "False") << endl;
    return true;

}



bool searchit(const filesystem::path& start_dir, string& input) 
{
    // checking if file or folder exist in current directory
    if (start_dir.filename() == input) {
        return true;
    }

    // if no then searching recursively inside current directory 
    if (filesystem::is_directory(start_dir)) 
    {
        for (const auto& inside : filesystem::directory_iterator(start_dir)) 
        {
            if (searchit(inside.path(), input)) return true;  // got it somewhere inside
        }
    }

    // not found 
    return false;
}


char** auto_complete(const char* input, int start, int end)  //The partial argument we’re completing and the positions where it starts and ends in the current line of input will be passed as arguments automatically when we press tab
{
    //rl_attempted_completion_over = 1;
    char **match_list = nullptr;

    // Autocomplete commands or file/directory names
    match_list = rl_completion_matches(input, generator_function);  //The generator function is called repeatedly from rl_completion_matches(), returning a string each time
    // This function, in turn, repeatedly calls the function pointer given as its second argument (in your case, command_generator) until that function returns nullptr, signaling there are no more completions.

    return match_list;
    //match_list is a point to a array where each entry points to a string
}

char* generator_function(const char* input, int state) 
{
    static vector<string> list;
    static size_t list_index = 0;

    if (state == 0) {
        // If this is a new word to complete, initialize now.
        list.clear();
        list_index = 0;

        // Collect a list of matches from current directory
        string text(input);
        for (auto v : filesystem::directory_iterator(".")) //. represent current directory
        {
            // v points to directory entry (not a string)
            if (v.path().filename().string().find(text) == 0) 
            {
                list.push_back(v.path().filename().string());
            }
        }
    }

    if (list_index >= list.size()) 
    {
        return nullptr;
    } else {
        return strdup(list[list_index++].c_str());
        //strdup is a function that duplicates a string by allocating memory for a new string and copying the contents of the original string into it. It takes a const char* as an argument and returns a pointer to the duplicated string
    }
}

void foreground(vector<string>& arguments)
{
    pid_t pid = fork();

    if(pid == -1)
    {
        perror("ERROR in creating child process");
    }
    else if(pid == 0)
    {
        signal(SIGINT, SIG_DFL);  
        signal(SIGTSTP, SIG_DFL);
        
        //cout<<"child: "<<pid<<endl;
        call_exec(arguments);
         
    
    }
    else 
    {
        
        int status;
        waitpid(pid, &status, WUNTRACED);   //Also return if a child has stopped
        
    }
     //signal(SIGINT, SIG_IGN); 
     //signal(SIGTSTP, SIG_IGN);
     
}

void call_exec(vector<string> arguments)
{
    int length = arguments.size();
        char ** args = new char*[length+1];
        //args[0] = string_to_Cstyle(command);
        for(int i=0; i<length; i++)
        {
            // string temp = arguments[i];
            // if(temp.front() == '\"' || temp.back() == '\"' || temp.front() == '\'' || temp.back() == '\'')
            // {
            //     //string temp = arguments[i];
            //     temp = temp.substr(1, temp.length()-2);
            //     //arguments[i] = temp;
            //     cout<<temp<<endl;
            // }
            args[i] = string_to_Cstyle(arguments[i]);
        }
        args[length] = NULL;
        //cout<<"before exec"<<endl;
        int i = execvp(args[0], args);
        //cout<<"after exec"<<endl;
        if(i==-1)
        {
            perror("exec cant be executed");
            exit(EXIT_FAILURE);
        }
       
      
        delete[] args;
}

void background(vector<string>& arguments)
{
    pid_t pid = fork();

    if(pid == -1)
    {
        perror("ERROR in creating child process");
    }
    else if(pid == 0)
    {
         
        call_exec(arguments);//cout<<"child: "<<pid<<endl;
        //setpgid(0, 0);
        
    }
    else 
    {
        cout<<"["<<pid<<"]"<<endl;
        //int status;
        //waitpid(pid, &status, WUNTRACED);   //Also return if a child has stopped
    }
}

// void childHandler(int signum) {
//     while (waitpid(-1, NULL, WNOHANG) > 0); // Reap child processes
// }
// void ctrlC(int signum)
// {
//     cout<<"pid: "<<pid<<endl;
//     cout<<"ctrlC "<<signum<<endl;
//     if(pid != -1)
//     {
//         kill(pid, SIGINT);
//     }
    
//     pid = -1;  
// }

/*void ctrlZ(int signum)
{
    cout<<"pid: "<<pid<<endl;
    cout<<"ctrlZ "<<signum<<endl;
    //pause();
    if(pid != -1)
    {
        kill(pid, SIGSTOP);
        pid = -1;
    }
    
    // else
    // {
    //     signal(SIGTSTP, SIG_DFL);
    //     kill(getpid(), SIGTSTP);
    
    // }
}*/

void ctrlD(int signum) 
{
    cout << "Khatam goodbye gya tata"<<endl;
}


void extract(vector<string>& arguments, string &separator, vector<string>& com, vector<string>& output)
{

    bool find_sep = false;

    for(auto v : arguments)
    {
        if(v == separator) find_sep = true;
        else
        {
            if(find_sep == false)
            {
                com.push_back(v);
                //com += " ";

            }
            else
            {
                output.push_back(v);
                //output += " ";
            }
        }
    }
}

string find_separator(vector<string>& arguments) 
{
    // Define a list of possible separators
    vector<string> sep = {">", "<", ">>", "|"};

    for (auto v : arguments) 
    {
        if (find(sep.begin(), sep.end(), v) != sep.end()) 
        {
            return v;  // Return the first separator found
        }
    }

    return "";  // Return an empty string if no separator is found
}


void print(vector<string> list)
{
    for (auto args : list)
    {
    cout << "Argument: " << args << endl;
    }
    cout<<endl;
}

int hist_save()
{
    while(history_length > 20)
    {
        remove_history(0);   // removes the oldes command from the list
        history_length--;
    }

    const char * path = "/home/zenvis/Desktop/history.txt";
//     FILE* file = fopen(path, "w");
   
//    if(file == NULL)
//    {
//     perror("unable to open file due to some failure");
//     return 0;
//    }
    // int fd;
    // fd = open(path, O_CREAT | O_RDWR  | O_APPEND, 0644);
    //const char * path = "/home/zenvis/Desktop/history.txt";
    int i = write_history(path);
    if(i != 0)
    {
        perror("unable to write history to file");
    }
    //fclose(file);
    return 0;
}

int hist_read()
{
     const char * path = "/home/zenvis/Desktop/history.txt";
//     FILE* file = fopen(path, "r");
   
//    if(file == NULL)
//    {
//     perror("unable to open file due to some failure");
//     return 0;
//    }
   int i = read_history(path);
   if(i != 0)
    {
        perror("unable to read history from file");
    }
    //fclose(file);
    return 0;
}

 bool hist_retrieve()
{
     //cout<<"entering history function"<<endl;

    HIST_ENTRY ** history = history_list();
    //cout<<history_length<<endl;

    if(history_length>10)
    {
        int temp = history_length - 10;
        for(int i=temp; i<history_length; i++)
        {
            //line is a member of the HIST_ENTRY structure that contains the actual command string of that history entry.
            cout<<history[i]->line<<endl;
        }
    }
    else
        {
            for(int i=0; i<history_length; i++)
            {
            
                cout<<history[i]->line<<endl;
            }

        }
    return true;
}

 bool hist_retrieve_num(int num)
{
     //cout<<"entering history function"<<endl;
     if(num>20)
     {
        perror("can't retrieve more than 20 commands");
        return false;
     }

    HIST_ENTRY ** history = history_list();
    //cout<<history_length<<endl;
    int temp = history_length;
    for(int i= temp-num; i<history_length; i++)
    {
    
        cout<<history[i]->line<<endl;
    }
    return true;
}

void tokenize_string(vector<string>& arguments, string& command)
{
    string arg;
                
    bool sq = false, dq= false;
    
    for(char v: command) 
    {
        if(v == '\'') 
        {
            sq = !sq; 
        } 
        else if(v == '\"') 
        {
            dq = !dq; 
        } 
        else if(v == ' ' && !sq && !dq) 
        {
            if(!arg.empty()) 
            {
                arguments.push_back(arg);
                arg.clear();
            }
        } 
        else 
        {
            arg += v;
        }
    }

    // Push the last command
    if(!arg.empty()) 
    {
        arguments.push_back(arg);
    }
    //print(arguments);
}

