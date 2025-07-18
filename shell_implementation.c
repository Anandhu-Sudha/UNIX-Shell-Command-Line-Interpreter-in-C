// own shell implementation using c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>

#define MAX_LINE 128
#define HISTORY_SIZE 10

char history[HISTORY_SIZE][MAX_LINE];
int history_index=1;


// function prototypes
int single_command_mode(int argc,char *argv[]);
int shell_mode();
char **parse_input(char *line,int *arg_count);
void execute_single_command(char **args,int argc);
void execute_pipeline(char ***cmds,int n);
void free_args(char **args);

// main function
int main(int argc,char *argv[])
{
	if(argc>1 && strcmp(argv[1],"-c")==0)
		return single_command_mode(argc,argv);
	else
		return shell_mode();
}

// -c mode : single command mode
int single_command_mode(int argc,char *argv[])
{
	printf("CUSTOM MADE SHELL RUNNING: \n");
	if(argc<3)
	{
		fprintf(stderr,"USAGE: %s -c command [args...]\n",argv[0]);
		return 1;
	}

	pid_t pid=fork();
	if(pid ==0)
	{
		execvp(argv[2],&argv[2]);
		perror("exec failed");
		exit(1);
	}
	else if(pid>0)
	{
		wait(NULL);
	}
	else
	{
		perror("fork failed");
		return 1;
	}
	return 0;
}

int shell_mode()
{

	char input[MAX_LINE];
	int command_count=1;
	char host_name[100];
	gethostname(host_name,sizeof(host_name));

printf(
"   _____           _                    _____  _          _ _ \n"
"  / ____|         | |                  / ____|| |        | | |\n"
"  | |    _   _ ___| |_ ___  _ __ ___   | (___ | |__   ___| | |\n"
"  | |   | | | / __| __/ _ \\| '_ ` _ \\   \\___ \\| '_ \\ / _ \\ | |\n"
"  | |___| |_| \\__ \\ || (_) | | | | | |  ____) | | | |  __/ | |\n"
"  \\_____|\\__,_|___/\\__\\___/|_| |_| |_| |_____/|_| |_|\\___|_|_|\n"
"\n"
"\t\tCUSTOM MADE SHELL RUNNING:\n"
);

	while(1)
	{
		//prompt: <command_number hostname>
		printf("<%d %s> ",command_count,host_name);
		fflush(stdout);

		//read user input
		if(fgets(input,MAX_LINE,stdin)==NULL)
			break; //

		//remove newline char from input
		// VERY IMPORTANT, if \n is not removed the commands wont work.
		input[strcspn(input,"\n")]=0;

		//ignore empty input
		if(strlen(input)==0)
			continue;

		// history count implementation
		strncpy(history[history_index%HISTORY_SIZE],input,MAX_LINE);
		history_index++;

		//split input by ';'
		char *saveptr;
		char *token = strtok_r(input,";",&saveptr);
		while(token!=NULL)
		{
			while(*token ==' ' || *token == '\t')
				token++;
			int argc;
			char **args = parse_input(token,&argc);
			if(argc>0)
			{
				execute_single_command(args,argc);
				free_args(args);
			}
			token=strtok_r(NULL,";",&saveptr);
		}
		command_count++;
	}
	return 0;

}

//parses a line into token(word by word, cli style)
char **parse_input(char *line, int *arg_count)
{
	static char *args[100];
	char *token;
	int count=0,i=0;


	while(line[i]!='\0')
	{
		//skip while spaces
		while(line[i]==' ' || line[i] == '\t')i++;

		if(line[i]=='\0')break;

		//special symbols as tokens
		if(line[i]=='<' || line[i]=='>' || line[i]=='|' ||line[i]==';')
		{
			args[count]=(char*)malloc(2);
			args[count][0]=line[i];
			args[count][1]='\0';
			count++;
			i++;
		}
		else
		{
			// normal word token
			int start =i;
			while(line[i]!='\0' &&line[i]!=' ' &&line[i]!='\t' &&line[i]!='<' &&line[i]!='>' &&line[i]!='|' &&line[i]!=';')
				i++;
			int length=i-start;
			args[count]=(char*)malloc(length+1);
			strncpy(args[count],&line[start],length);
			args[count][length]='\0';
			count++;
		}

	}
	args[count]=NULL;
	*arg_count =count;
	return args;

}

void execute_single_command(char **args,int argc)
{
	char *in_file =NULL,*out_file=NULL;
	// Built-in commands
	// history command -> hist
	if(strcmp(args[0],"hist")==0)
	{
		int start=(history_index>HISTORY_SIZE)?(history_index-HISTORY_SIZE):0;
		for(int i=start;i<history_index;++i)
		{
			printf("%d\t%s\n",i+2,history[i%HISTORY_SIZE]);
		}
		return;
	}

	//current process id -> curPid
	if(strcmp(args[0],"curPid")==0)
	{
		printf("Current PID : %d\n",getpid());
		return;
	}

	//parent process id -> pPid
	if(strcmp(args[0],"pPid")==0)
	{
		printf("Parent PID : %d\n",getppid());
		return;
	}

	//cd:
	if(strcmp(args[0],"cd")==0)
	{
		if(argc<2)
			fprintf(stderr,"cd: missing operand\n");
		else if(chdir(args[1])!=0)
			perror("cd failed");
		return;
	}

	//quit
	if(strcmp(args[0],"quit")==0)
	{
		exit(0);
	}

	//pipeline detection
	int num_cmds=1;
	for(int i=0;i<argc;i++)
		if(strcmp(args[i],"|")==0)
			num_cmds++;
	char *exec_args[MAX_LINE];
	int j=0;
	if(num_cmds==1)
	{
		// no pipeline
		int in_redir =-1, out_redir=-1;
		int j=0;

		for(int i=0;i<argc;i++)
		{
			if(strcmp(args[i],"<")==0)
			{
				if(i+1 <argc)
					in_file=args[++i];
			}
			else if(strcmp(args[i],">")==0)
			{
				if(i+1<argc)
					out_file=args[++i];
			}
			else
				exec_args[j++]=args[i];
		}
		exec_args[j]=NULL;

		pid_t pid=fork();
		if(pid==0)
		{
			if(in_file)
			{
				int fd=open(in_file,O_RDONLY);
				if(fd<0)
				{
					perror("open input");
					exit(1);
				}
				dup2(fd,STDIN_FILENO);
				close(fd);
			}


			if(out_file)
			{
				int fd=open(out_file,O_WRONLY|O_CREAT|O_TRUNC,0664);
				if(fd<0)
				{
					perror("open output");
					exit(1);
				}
				dup2(fd,STDOUT_FILENO);
				close(fd);
			}


			execvp(exec_args[0],exec_args);
			perror("exec failed");
			exit(1);
		}
		else if(pid>0)
		{
			wait(NULL);
			return;
		}
		else
		{
			perror("fork failed");
		}


	}
	else
	{
		// pipeline handling
		char ***cmds=malloc(sizeof(char**)*num_cmds);
		int start =0, cmd_idx=0;

		for(int i=0;i<=argc;i++)
		{
			if(i==argc||strcmp(args[i],"|")==0)
			{
				cmds[cmd_idx]=malloc(sizeof(char *)*(i-start+1));
				int k=0;
				for(int j=start;j<i;j++)
					cmds[cmd_idx][k++]=args[j];
				cmds[cmd_idx][k]=NULL;
				cmd_idx++;
				start=i+1;
			}
		}
		execute_pipeline(cmds,cmd_idx);
		for(int i=0; i<cmd_idx;i++)
			free(cmds[i]);
	}
}





	//executes a set of piped commands
	void execute_pipeline(char ***cmds,int n)
	{
		int pipefd[2], in_fd=0;

		for(int i=0;i<n;i++)
		{
			pipe(pipefd);
			pid_t pid=fork();
			if(pid==0)
			{
				dup2(in_fd,0);
				if(i!=n-1)
					dup2(pipefd[1],1);
				close(pipefd[0]);
				execvp(cmds[i][0],cmds[i]);
				perror("exec failed");
				exit(1);
			}
			else if(pid<0)
			{
				perror("fork failed");
			}
			wait(NULL);
			close(pipefd[1]);
			in_fd=pipefd[0];
		}
	}


	// free allocated memory by parse input()
	void free_args(char **args)
	{
		for(int i=0;args[i];i++)
			free(args[i]);
	}

