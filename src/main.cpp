#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <string.h>
#include <vector>
#include <wordexp.h>
#include <sys/wait.h>
#include <list>
#include <csignal>
#include <tuple>

#include "termcolors.hpp"
#include "getinput.hpp"
#include "signalhandler.hpp"

using namespace std;

int main(int argc, char **argv){
    char buf[1000];
    list<tuple<pid_t, wordexp_t *, int *>> bgProcs;


    signal(SIGINT, signalHandler);
    // signal(SIGTSTP, signalHandler);

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
                    int *retVal;
                    retVal = (int *)malloc(sizeof(int));
                    *retVal = 256;
                    bgProcs.push_back(make_tuple(pid, p, retVal));
                    waitpid(pid, retVal, WNOWAIT);
                }
            }
            
            else if(!strcmp(p->we_wordv[0], "bglist")){
                int procCount = 0;
                int returnVal = 255;
                for(auto i:bgProcs){
                    waitpid(get<0>(i), &returnVal, WSTOPPED | WNOHANG);
                    if(WIFSTOPPED(returnVal))
                        cout << "[" << procCount++ << "]+ Stopped\t\t";

                    else
                        cout << "[" << procCount++ << "]+ Running\t\t";

                    for(int j = 1; j < get<1>(i)->we_wordc; j++)
                        cout << get<1>(i)->we_wordv[j] << " ";
                    cout << endl;
                }
            }

            else if(!strcmp(p->we_wordv[0], "bgkill")){
                int procCounter = 0;
                int n = atoi(p->we_wordv[1]);
                for(auto i = bgProcs.begin(); i != bgProcs.end(); i++){
                    if(procCounter == n){
                        kill(get<0>(*i), SIGTERM);
                        bgProcs.erase(i);
                        cout << "[" << n << "]+ Killed\t\t";
                        for(int j = 1; j < get<1>(*i)->we_wordc; j++)
                            cout << get<1>(*i)->we_wordv[j] << " ";
                        cout << endl; 
                        break;
                    }
                    procCounter++;
                }
            }

            else if(!strcmp(p->we_wordv[0], "bgstop")){
                int procCounter = 0;
                int n = atoi(p->we_wordv[1]);
                for(auto i = bgProcs.begin(); i != bgProcs.end(); i++){
                    if(procCounter == n){
                        kill(get<0>(*i), SIGSTOP);
                        // bgProcs.erase(i);
                        cout << "[" << n << "]+ Stopped\t\t";
                        for(int j = 1; j < get<1>(*i)->we_wordc; j++)
                            cout << get<1>(*i)->we_wordv[j] << " ";
                        cout << endl; 
                        break;
                    }
                    procCounter++;
                }
            }

            else if(!strcmp(p->we_wordv[0], "bgstart")){
                int procCounter = 0;
                int n = atoi(p->we_wordv[1]);
                for(auto i = bgProcs.begin(); i != bgProcs.end(); i++){
                    if(procCounter == n){
                        kill(get<0>(*i), SIGCONT);
                        // bgProcs.erase(i);
                        cout << "[" << n << "]+ Continued\t\t";
                        for(int j = 1; j < get<1>(*i)->we_wordc; j++)
                            cout << get<1>(*i)->we_wordv[j] << " ";
                        cout << endl; 
                        break;
                    }
                    procCounter++;
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
        
        //Check if any background process is finished
        for(auto i = bgProcs.begin(); i != bgProcs.end(); i++){
                int procCount = 0;
                int returnVal = 255;
                waitpid(get<0>(*i), &returnVal, WNOHANG);
                if(returnVal == 0){
                    cout << "[" << procCount++ << "]+ Done\t\t";
                    for(int j = 1; j < get<1>(*i)->we_wordc; j++)
                        cout << get<1>(*i)->we_wordv[j] << " ";
                    cout << endl;

                    wordfree(get<1>(*i));
                    free(get<2>(*i));
                    i = bgProcs.erase(i);
                }
                

            }
        
    }
    exit(0);
}