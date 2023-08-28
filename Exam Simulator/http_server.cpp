#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <unistd.h>

using namespace std;

char* index_data;
string path = "index.html";

char* exam_data;
string path_A = "index2a.html";
string path_B = "index2b.html";

vector<string> DB_user;
vector<string> DB_pass;

struct vault {
    string u;
    string p;
};

vault* getCRED(string data) {
    string user;
    int a = 11;
    int b = 0;

    string pass;
    int p = 0;
    int q = 0;

    int index = 11;
    
    while(1) {
        if(data[index]=='+') {
            user = user + " ";
        }
        else {
            user = user + data[index];
        }
        index++;

        if(data[index]=='&') {
            index+=10;
            break;
        }
    }

    while(1) {
        pass = pass + data[index];
        index++;

        if(data[index]=='&') {
            break;
        }
    }

    vault* V = new vault;
    V->u = user;
    V->p = pass;

    return V;
}

string getDATA(char* response) {
    string data;
    int i = 0;
    while(1) {
        data = data + response[i];
        if(response[i]=='\n') {
            break;
        }
        i++;
    }
    return data;
}

string getREQ(string data) {
    string request;

    for(int i=4; i<data.length(); i++) {
        if(data[i]==' ') {
            break;
        }
        request = request + data[i];
    }
    request = request + "&";

    return request;
}

int getSIZE(char* cons_data) {
    int i = 0;
    int size = 0;
    while(1) {
        if(cons_data[i]=='|') {
            break;
        }

        i++;
        size++;
    }
    
    return size-1;
}

char* getHTML(string path) {
    string temp;
    string data;
    string head = "HTTP/1.1 200 OK\r\n\n";

    ifstream html (path);

    while(html) {
        data += temp + " ";
        html >> temp;
    }

    string str_data = head + data + "|";

    int size = str_data.length();
    
    char* cons_data = new char[size + 1];

    strcpy(cons_data, str_data.c_str());
    return cons_data;
}

char* getHTML2(string path_A, string path_B, string user) {
    string temp_A;
    string temp_B;

    string data_A;
    string data_B;

    string head = "HTTP/1.1 200 OK\r\n\n";

    ifstream html_A (path_A);
    ifstream html_B (path_B);

    while(html_A) {
        data_A += temp_A + " ";
        html_A >> temp_A;
    }

    while(html_B) {
        data_B += temp_B + " ";
        html_B >> temp_B;
    }
    
    string str_data = head + data_A + user + data_B + "|";

    int size = str_data.length();
    
    char* cons_data = new char[size + 1];

    strcpy(cons_data, str_data.c_str());
    return cons_data;
}

int getTRIG(char* response) {
    int trig = 1;
    
    string data;
    data = getDATA(response);
    
    string request;
    request = getREQ(data);


    if(request=="/&") {
        trig = 1;
        index_data = getHTML(path);
        
        cout << "T1: index.html" << endl;
    }
    else {
        vault* V = getCRED(request);

        string user = V->u;
        string pass = V->p;


        for(int i=0; i<DB_user.size(); i++) {
            if(DB_user[i]==user) {
                if(DB_pass[i]==pass) {
                    trig = 2;
                    exam_data = getHTML2(path_A, path_B, user);
                }
            }
        }

        cout << "T2: index2.html" << endl;
        cout << "user: " << user << endl;
        cout << "pass: " << pass << endl;
    }

    return trig;
}

void getVEC(string path) {
    string temp;
    string data;

    ifstream dbms (path);

    while(dbms) {
        dbms >> temp;
        if(temp=="&") {
            data.pop_back();
            DB_user.push_back(data);
            data = "";
        }
        else if(temp=="|") {
            data.pop_back();
            DB_pass.push_back(data);
            data = "";
        }
        else if(temp=="?") {
            data.pop_back();
            DB_pass.push_back(data);
            data = "";
            break;
        }
        else {
            data += temp + " ";
        }
    }
}

int main() {
    string DB_path = "dbms.txt";
    getVEC(DB_path);

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_socket;
    char response[4096];

    int trig;

    while(1) {
        client_socket = accept(server_socket, NULL, NULL);

        recv(client_socket, &response, sizeof(response), 0);
        trig = getTRIG(response);

        if(trig == 1) {
            send(client_socket, index_data, getSIZE(index_data), 0);
        }
        else if(trig == 2) {
            send(client_socket, exam_data, getSIZE(exam_data), 0);
        }
        

        close(client_socket);
    }

    return 0;
}