#include <unistd.h>
#include <string>
#include <iostream>
using namespace std;

int main()
{
    int fd_p2c[2], fd_c2p[2], bytes_read;
    pid_t childpid;
    char readbuffer[83];
    string program_name = "/usr/bin/php-cgi";
    string gulp_command = "this is the command data sent to the child cat (kitten?)";
    string receive_output = "";

    if (pipe(fd_p2c) != 0 || pipe(fd_c2p) != 0)
    {
        cerr << "Failed to pipe\n";
        exit(1);
    }

    if ((childpid = fork()) < 0)
    {
        cout << "Fork failed" << endl;
        exit(-1);
    }
    if (childpid == 0)
    {
        if (dup2(fd_p2c[0], 0) != 0 ||
            close(fd_p2c[0]) != 0 ||
            close(fd_p2c[1]) != 0)
        {
            cerr << "Child: failed to set up standard input\n";
            exit(1);
        }
        if (dup2(fd_c2p[1], 1) != 1 ||
            close(fd_c2p[1]) != 0 ||
            close(fd_c2p[0]) != 0)
        {
            cerr << "Child: failed to set up standard output\n";
            exit(1);
        }

		char	*agv[3] = {(char*) program_name.c_str(), (char*) "-n", NULL};
		char	*env[3] = {(char*) "REDIRECt_STATUS=1", NULL};
        execve(program_name.c_str(), agv, NULL);
        cerr << "Failed to execute " << program_name << endl;
        exit(1);
    }
    else
    {
        close(fd_p2c[0]);
        close(fd_c2p[1]);

        cout << "Writing to child: <<" << gulp_command << ">>" << endl;
        int nbytes = gulp_command.length();
        if (write(fd_p2c[1], gulp_command.c_str(), nbytes) != nbytes)
        {
            cerr << "Parent: short write to child\n";
            exit(1);
        }
        close(fd_p2c[1]);

        while (1)
        {
            bytes_read = read(fd_c2p[0], readbuffer, sizeof(readbuffer)-1);

            if (bytes_read <= 0)
                break;

            readbuffer[bytes_read] = '\0';
            receive_output += readbuffer;
        }
        close(fd_c2p[0]);
        cout << "From child: <<" << receive_output << ">>" << endl;
    }
    return 0;
}
