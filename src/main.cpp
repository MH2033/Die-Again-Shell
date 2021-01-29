#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <string.h>
#include <vector>
#include <wordexp.h>
#include <sys/wait.h>
#include <list>
#include <csignal>

#include "termcolors.hpp"
#include "getinput.hpp"
#include "signalhandler.hpp"

using namespace std;

int main(int argc, char **argv){
    char buf[1000];
    list<pair<pid_t, wordexp_t *>> bgProcs;


    signal(SIGINT, signalHandler);
    signal(SIGTSTP, signalHandler);

    system("setterm -cursor on");
    system("setterm -blink on");
    system("clear");
    while(true){
        // cout << "IN LOOP" << endl;
        cout << BOLDCYAN << "Dash:" << YELLOW << getcwd(buf, 1000) << MAGENTA << "~> " << RESET;
        wordexp_t *p;
        p = new wordexp_t;
        *p = get_input();

        if(p->we_wordc > 0){
            if(!strcmp(p->we_wordv[0], "clear"))
                system("clear");

            else if(!strcmp(p->we_wordv[0], "cd"))
                chdir(p->we_wordv[1]);
            
            else if(!strcmp(p->we_wordv[0], "pwd"))
                cout << getcwd(buf, 1000) << endl;

            else if(!strcmp(p->we_wordv[0], "exit"))
                exit(0);

            else if(!strcmp(p->we_wordv[0], "bg")){
                pid_t pid = fork();
                //Child process instructions
                if(pid == 0){
                    execvp(p->we_wordv[1], &p->we_wordv[1]);
                }

                //Parent process instructions
                else{
                    bgProcs.push_back(make_pair(pid, p));
                    waitpid(pid, NULL, WNOHANG);
                }
            }
            
            else if(!strcmp(p->we_wordv[0], "bglist")){
                int procCount = 0;
                for(auto i:bgProcs){
                    cout << "[" << procCount++ << "]+\tRunning\t\t";
                    for(int j = 1; j < i.second->we_wordc; j++)
                        cout << i.second->we_wordv[j] << " ";
                    cout << endl;
                }
            }

            else {
                pid_t pid = fork();
                //Child process instructions
                if(pid == 0){
                    execvp(p->we_wordv[0], p->we_wordv);
                }

                //Parent process instructions
                else{
                    waitpid(pid, NULL, 0);
                    wordfree(p);
                    free(p);
                }
            }
        }
        
        
    }
    exit(0);
}