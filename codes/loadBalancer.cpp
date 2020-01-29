#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <dirent.h>
#include <stdlib.h>

#include<sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 

using namespace std;


#define EXEC_OBJECT_DECIDER "decider"
#define EXEC_OBJECT_WORKER  "worker"
#define EXEC_WORKER_ADDRESS "/home/fatemeh/Documents/university/semester_6/OS/projects/course/p2/worker"
#define EXEC_DECIDER_ADDRESS "/home/fatemeh/Documents/university/semester_6/OS/projects/course/p2/decider"

void listdir(const char *name, int indent, vector<string> &file_path)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) 
    {
        char path[2048];
        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        if (entry->d_type == DT_DIR) 
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            listdir(path, indent + 2 , file_path);

        } 
        else 
        {
            file_path.push_back(path);
            // int p = fork();
            // if (p < 0)
            //     ////cout <<"problemmmmmmmmmm"<<endl;
        }

    }
    closedir(dir);
}

void concate_words(vector < string > words , string &sentence)
{
    for(int j =0 ; j < words.size(); j++)
    {
        sentence += words[j];
        sentence += "$";
    }

}


void seperate_file_line(string line , vector < string > &line_content)
{
    string temp = "";
    // ////cout<< "line in function : "<<line<<endl;
    for(int i =0; i < line.size(); i++)
    {
        if(line[i] == ' ' || line[i] =='\t')
        {
            if(temp != "")
            {
                line_content.push_back(temp);
                temp = "";
            }
        }
        else if (i == line.size() - 1)
        {
            temp += line[i];
            line_content.push_back(temp);
        }
        else
        {
            // ////cout<<"charachter in line :" <<line[i]<<endl;
            temp += line[i];
            // ////cout << "addet character to word : " << temp<<endl;
        }
    }

    // ////cout << "words seperated in function " <<endl;
    // for(int i =0 ; i < line_content.size(); i++)
    // {
        // if(line_content[0] == "32")
        // {
        //     ////cout << "&&&&&&&&&&&&&line size is :" << line.size()<<endl;
        //     ////cout << "value is == " << line_content[1] <<endl;
        // }


    // }
    // ////cout << "\n";
}



void split_msg_rec(char* msg , vector < string > &splited_msg)
{
    string temp = "";
    for(int l = 0; l<strlen(msg); l++)
    {
        if(msg[l] != '$')
            temp += msg[l];
        else 
        {
            splited_msg.push_back(temp);
            temp = "";
        }
    }
}
void sorting(vector <int > &results)
{
    int temp;
    for(int i = 0; i < results.size(); i++)
    {
        temp = 0;
        for(int j = i + 1; j < results.size(); j++)
        {
            if(results[i] > results[j])
            {
                temp = results[j];
                results[j] = results[i];
                results[i] = temp;
            }

        }
    }
}
int find_max_repeated(vector < int> numbers , vector <int > &indexes)
{
    int temp = 0;
    int counter = 0;
    temp = numbers[0];

    for(int i =0; i < numbers.size(); i++)
    {
        if(i == 0)
        {
            indexes.push_back(counter);
        }
        else if(numbers[i] == temp)
        {
            counter ++;
            indexes.push_back(counter);
        }
        else
        {
            counter = 0;
            temp = numbers[i];
            indexes.push_back(counter);
        }
    }
    temp = 0;
    int index = 0;
    for(int i =0; i < indexes.size(); i++)
    {
        if(indexes[i] > temp)
        {
            temp = indexes[i];
            index = i;
        }
    }
    return numbers[index];

}

int main() 
{
    string input;
    string sendMsg = "";
    int sensor_number = 0;
    vector < string >  file_path;
    vector < string > worker_id_string;
    vector < vector < int > > process_pipe;
    vector < int > worker_id;
    int decider_pid;
    int worker_pid;
    int file_num = 0;
    int a[2];
    int open_res;
    listdir("./databases", 0 , file_path);
    mkfifo("named_pipe",0666);
    file_num = file_path.size();

    // std::fstream myfile;

    // myfile.open ("named_pipe", std::fstream::in);
    // myfile.clear();

    std::ofstream ofs;
    ofs.open("named_pipe", std::ofstream::out | std::ofstream::trunc);
    ofs.close();


    while(true)
    {
        cout<< "please enter a sensor number"<<endl;
        cin>> input;
        if(input == "quit")
        {
            break;
        }
        else if(atoi(input.c_str()) < 0 || atoi(input.c_str()) > 49)
        {
            ////cout<<"invalid input,try again"<<endl;
            continue;
        }
        sensor_number = atoi(input.c_str());
        // decider_pid = fork();
        // // mkfifo("named_pipe",0666);
        // if(decider_pid < 0)
        // {
        //     //cout<<"could not generate decider process"<<endl;
        //     exit(1);
        // }
        // if(decider_pid > 0)
        {
            for(int i = 0; i < file_num; i++)
            {
                pipe(a);
                worker_pid = fork();
                // ////cout << worker_pid<<endl;
                // //////cout<<"worker pid is "<< worker_pid << endl;
                if(worker_pid < 0)
                {
                    ////cout<<"could not generate a process"<<endl;
                    break;
                }
                if(worker_pid > 0)
                {
                    worker_id.push_back(worker_pid);
                    sendMsg = "";
                    sendMsg += input + "#" + file_path[i] + "$";
                    write( a[1], sendMsg.c_str() , strlen(sendMsg.c_str()) + 1);
                    close(a[0]);
                    close(a[1]);
                    waitpid(worker_pid, NULL, 0);
                }
                if(worker_pid == 0)
                {
                    string sensor_number = "";
                    string file_address = "";
                    string line;
                    string send_data = "";
                    vector <string> line_words;
                    char read_value[2000];
                    memset(read_value, 0, 2000);
                    read(a[0], read_value, 2000);

                    for(int i =0; i < sizeof(read_value); i++)
                    {
                        if(read_value[i] != '#')
                        {
                            sensor_number += read_value[i];
                            read_value[i] = '#';
                        }
                        else
                            break;
                    }
                    for(int i =0; i < sizeof(read_value); i++)
                    {
                        if(read_value[i] != '#' && read_value[i] != '$')
                            file_address += read_value[i];
                        if(read_value[i] == '$')
                            break;
                    }
                    ifstream myfile(file_address.c_str());
                    send_data = "";
                    if (myfile.is_open())
                    {
                        while(getline(myfile,line))
                        {
                            // ////cout<< "line : " << line<<endl;
                            line_words.clear();
                            seperate_file_line(line , line_words);
                            // for(int j =0; j < line_words.size() ; j ++)
                            // ////cout << "size of line words : " << line_words.size()<<endl;
                            // {
                            //     ////cout<< "sensor in file : "<< line_words[0] <<endl;

                            // }
                            // ////cout << "sensor number is : " << sensor_number << endl;
                            if(line_words[0] == sensor_number)
                            {
                                // ////cout << "found sensor \n";
                                // ////cout << "line is : " << line << endl;

                                // ////cout << "data of sensore in line is " << line_words[1] <<endl;
                                send_data = "";
                                send_data = line_words[1];
                                break;
                            }
                        }
                        myfile.close();
                    }

                    send_data += '$';
                    send_data += '\n';
                    // ////cout << "data send to decieder : " << send_data << endl;
                    open_res = open("named_pipe", O_WRONLY | O_APPEND);
                    // open_res = open("named_pipe",O_WRONLY);
                    //cout << "@@@@@@@@data is writing in decieder pipe is " <<send_data << endl;

                    int byte = write(open_res , send_data.c_str() , strlen(send_data.c_str()));
                    
                    // //cout << "send data to decieder is : " << send_data <<endl;

                    send_data = "";
                    close(open_res);
                    exit(1);
                }

            }
            for(int i =0; i < file_num; i++)
                waitpid(worker_id[i], NULL, 0);
            // while(wait(NULL));
            // open_res = open("named_pipe",O_WRONLY);
            open_res = open("named_pipe", O_WRONLY | O_APPEND);
            write(open_res,"endOfWork\n",strlen("endOfWork\n") + 1);
            close(open_res);  
            // wait(NULL);
        }
        // else if(decider_pid == 0)


        decider_pid = fork();
        if (decider_pid == 0)
        {
            // for(int k =0; k < 100; k++)
            //     ////cout <<"";
            // ////cout << "in decider exec part"<< endl;
            vector < int > results;
            string word = "";

            char recv[2000];
            
            // int fd = open("named_pipe",O_RDONLY);
            ifstream input_file("named_pipe");
            
            string line = "";
            // while(true)
            while (getline(input_file, line))
            {
                // memset(recv,0 , 2000);
                // read(fd,recv,sizeof(recv));
                // //cout << "************recv value is : " << line << endl;

                if (line == "endOfWork")
                    break; 
                else
                {
                    // word = "";
                    // for(int i =0 ; i < sizeof(recv); i++)
                    // {
                    //     if(recv[i] != '$')
                    //     {
                    //         word += recv[i];
                    //     }
                    //     else
                    //         break;
                    // }
                    results.push_back(atoi((line.substr(0, line.size() - 1)).c_str()));
                } 
                line = "";
            }

            vector <int > indexes;
            // for(int k =0; k < results.size(); k++)
            //     ////cout <<results[k] << "^^^^^^^^^^^^";
            ////cout <<endl;
            sorting(results);
            // for(int i =0; i < results.size(); i++)
            // {
            //     ////cout << results[i] << endl;
            // }

            cout<<"value of sensor "<< sensor_number << " is "<<find_max_repeated(results , indexes)<<endl;

            results.clear();

            std::ofstream ofs;
            ofs.open("named_pipe", std::ofstream::out | std::ofstream::trunc);
            ofs.close();
            exit(1);

        }
        else 
        {
            wait(NULL);
            // while(wait(NULL));
        }
    }
    exit(1);
}