/* servTCPPreTh.c - Exemplu de server TCP concurent care deserveste clientii
   printr-un mecanism de prethread-ing; cu blocarea mutex de protectie a lui accept(); 
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

/* portul folosit */
#define PORT 2909

int clients;

vector<string> logged_clients;
int current_logged_clients;
string convertToString(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++)
    {
        s = s + a[i];
    }
    return s;
}
/* codul de eroare returnat de anumite apeluri */

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
//void raspunde(void *);

typedef struct
{
    pthread_t idThread; //id-ul thread-ului
    int thCount;        //nr de conexiuni servite
} Thread;

Thread *threadsPool; //un array de structuri Thread

int sd;                                            //descriptorul de socket de ascultare
int nthreads;                                      //numarul de threaduri
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER; // variabila mutex ce va fi partajata de threaduri

void raspunde(int cl, int idThread);

int main(int argc, char *argv[])
{
    struct sockaddr_in server; // structura folosita de server
    void threadCreate(int);

    if (argc < 2)
    {
        fprintf(stderr, "Eroare: Primul argument este numarul de fire de executie...");
        exit(1);
    }
    nthreads = atoi(argv[1]);
    if (nthreads <= 0)
    {
        fprintf(stderr, "Eroare: Numar de fire invalid...");
        exit(1);
    }
    threadsPool = (Thread *)calloc(sizeof(Thread), nthreads);

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return -1;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return -1;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 10) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return -1;
    }

    printf("Nr threaduri %d \n", nthreads);
    fflush(stdout);
    int i;
    for (i = 0; i < nthreads; i++)
        threadCreate(i);

    /* servim in mod concurent clientii...folosind thread-uri */
    for (;;)
    {
        printf("[server]Asteptam la portul %d...\n", PORT);
        pause();
    }
};

void threadCreate(int i)
{
    void *treat(void *);

    pthread_create(&threadsPool[i].idThread, NULL, &treat, (void *)i);
    return; /* threadul principal returneaza */
}

void *treat(void *arg)
{
    int client;

    struct sockaddr_in from;
    bzero(&from, sizeof(from));
    cout << (int)arg << endl;
    fflush(stdout);

    for (;;)
    {
        socklen_t length;
        pthread_mutex_lock(&mlock);
        printf("Thread %d trezit\n", (int)arg);
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[thread]Eroare la accept().\n");
        }
        clients++;
        if (clients == nthreads)
        {
            for (int i = nthreads; i < 2 * nthreads; i++)
            {
                threadCreate(i);
            }
            nthreads = 2 * nthreads;
        }
        pthread_mutex_unlock(&mlock);
        threadsPool[(int)arg].thCount++;

        raspunde(client, (int)arg); //procesarea cererii
        /* am terminat cu acest client, inchidem conexiunea */
        clients--;
        close(client);
    }
}

void raspunde(int cl, int idThread)
{
    int logged = 0;
    char user[20];
    while (1)
    {
        int nr; //mesajul primit de trimis la client
        char buffer[2048];
        char bufferCopy[2048];
        char to_client[2048];
        char data[256];
        char args[20][256];
        char fromFile[256];
        char *p;
        int i = 0;
        strcpy(to_client, "");
        for (int j = 0; j < 20; j++)
            strcpy(args[j], "");

        if (read(cl, &buffer, sizeof(buffer)) <= 0)
        {
            printf("[Thread %d]\n", idThread);
            perror("Eroare la read() de la client.\n");
        }

        printf("[Thread %d]Mesajul a fost receptionat...%s\n", idThread, buffer);

        strcpy(bufferCopy, buffer);
        p = strtok(bufferCopy, " ");
        while (p)
        {
            strcpy(args[i++], p);
            p = strtok(NULL, " ");
        }
        cout << endl;

        if (!logged)
        {
            if (strncmp(args[0], "exit", max(strlen(args[0]), strlen("exit"))) == 0)
            {
                vector<string>::iterator result = find(logged_clients.begin(), logged_clients.end(), convertToString(user, sizeof(user) / sizeof(char)));
                logged_clients.erase(result);
                logged = 0;
                break;
            }
            else if (strncmp(args[0], "register", max(strlen(args[0]), strlen("register"))) == 0 && strlen(args[1]) > 0 && strlen(args[2]) > 0)
            {
                fstream fClient;
                fClient.open("database/clients.txt", ios::in);
                int found = 0;
                while (!found)
                {
                    fClient.getline(fromFile, 256, '\n');
                    if (fClient.eof())
                    {
                        fClient.close();
                        break;
                    }
                    int i_file = 0;
                    char args_file[20][256];
                    p = strtok(fromFile, "__");
                    while (p)
                    {
                        strcpy(args_file[i_file++], p);
                        p = strtok(NULL, "__");
                    }

                    if (strncmp(args[1], args_file[0], max(strlen(args[1]), strlen(args_file[0]))) == 0)
                    {
                        strcpy(to_client, "already exists");
                        found = 1;
                    }
                }

                if (!found)
                {
                    fClient.open("database/clients.txt", std::ios_base::app);
                    fClient << args[1] << "__" << args[2] << '\n';
                    strcpy(to_client, "register complete");
                    fClient.close();
                }
            }
            else if (strncmp(args[0], "login", max(strlen(args[0]), strlen("login"))) == 0 && strlen(args[1]) > 0)
            {
                fstream fClient;
                fClient.open("database/clients.txt", ios::in);
                int found = 0;
                int wrong_pas = 0;
                int already_logged = 0;
                while (!found)
                {
                    fClient.getline(fromFile, 256, '\n');
                    if (fClient.eof())
                    {
                        fClient.close();
                        break;
                    }
                    int i_file = 0;
                    char args_file[20][256];
                    p = strtok(fromFile, "__");
                    while (p)
                    {
                        strcpy(args_file[i_file++], p);
                        p = strtok(NULL, "__");
                    }

                    if (strncmp(args[1], args_file[0], max(strlen(args[1]), strlen(args_file[0]))) == 0)
                    {
                        if (strncmp(args[2], args_file[1], max(strlen(args[2]), strlen(args_file[1]))) == 0)
                        {
                            for (string client : logged_clients)
                            {
                                if (client == convertToString(args[1], sizeof(args[1]) / sizeof(char)))
                                {
                                    cout << client << " " << args[1] << endl;
                                    already_logged = 1;
                                }
                            }
                            if (!already_logged)
                            {
                                strcpy(to_client, "login successful");
                                logged_clients.push_back(convertToString(args[1], sizeof(args[1]) / sizeof(char)));
                                strcpy(user, args[1]);
                                logged = 1;
                                found = 1;
                            }
                        }
                        else
                        {
                            wrong_pas = 1;
                        }
                    }
                }

                if (!found && wrong_pas)
                {
                    strcpy(to_client, "wrong password");
                }
                else if (found == 1)
                {
                    strcpy(to_client, "login successful");
                }
                else if (already_logged)
                {
                    strcpy(to_client, "user already logged");
                }
                else
                    strcpy(to_client, "user not found");
            }
            else
                strcpy(to_client, "???");
        }
        else if (logged)
        {
            if (strncmp(args[0], "exit", strlen("exit")) == 0) // exit
                break;
            else if (strncmp(args[0], "logout", strlen("logout")) == 0) //logout
            {
                vector<string>::iterator result = find(logged_clients.begin(), logged_clients.end(), convertToString(user, sizeof(user) / sizeof(char)));
                logged_clients.erase(result);
                strcpy(to_client, "logged out");
                logged = 0;
            }
            else if (strncmp(args[0], "add", strlen("add")) == 0)
            {
                if (strlen(args[1]) == 0 || strlen(args[2]) == 0 // add but not all arguments
                    || strlen(args[3]) == 0 || strlen(args[4]) == 0 || strlen(args[5]) == 0)
                {
                    cout << args[1] << " " << args[2] << " " << args[3] << " " << args[4] << " " << args[5] << " " << args[6] << endl;
                    strcpy(to_client, "Syntax: add <password> <site_name> <username> <url> <category> <notes>");
                }
                else if (strlen(args[1]) > 0 && strlen(args[2]) > 0 // add but not all arguments
                         && strlen(args[3]) > 0 && strlen(args[4]) > 0 && strlen(args[5]) > 0 && strlen(args[6]) >= 0)
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    int canBeAdded = 0;
                    while (!found)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            if (strncmp(args[4], args_file[4], max(strlen(args_file[4]), strlen(args[4]))) == 0 && strncmp(args[3], args_file[3],
                                                                                                                           max(strlen(args_file[3]), strlen(args[3]))) == 0)
                            {
                                strcpy(to_client, "username/site are already added.");
                                found = 1;
                            }
                        }
                    }

                    if (!found)
                    {
                        fPasswords.open("database/passwords.txt", std::ios_base::app);
                        fPasswords << user << "__" << args[1] << "__" << args[2] << "__" << args[3] << "__" << args[4] << "__" << args[5] << "__" << args[6] << '\n';
                        strcpy(to_client, "add complete");
                        strcpy(args[6], " ");
                        fPasswords.close();
                    }
                }
            }
            else if (strncmp(args[0], "remove", max(strlen(args[0]), strlen("remove"))) == 0)
            {
                int remove = 0;
                if (strlen(args[1]) == 0)
                {
                    strcpy(to_client, "Syntax: remove <site name> <username>");
                }
                else if (strlen(args[2]) == 0)
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    while (found < 2)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            if (strncmp(args[1], args_file[2], max(strlen(args[1]), strlen(args_file[2]))) == 0)
                            {
                                found++;
                            }
                        }
                    }
                    if (found == 2)
                    {
                        strcpy(to_client, "found multiple instances of the same site. specify site username to delete");
                    }
                    else
                    {
                        fstream fPasswords;
                        fPasswords.open("database/passwords.txt", ios::in);
                        int found = 0;
                        char passwordsFile[1024];
                        strcpy(passwordsFile, "");
                        char fromFileBackup[256];
                        while (1)
                        {
                            fPasswords.getline(fromFile, 256, '\n');
                            if (fPasswords.eof())
                            {
                                fPasswords.close();
                                break;
                            }
                            int i_file = 0;
                            char args_file[20][256];
                            strcpy(fromFileBackup, fromFile);
                            p = strtok(fromFile, "__");
                            while (p)
                            {
                                strcpy(args_file[i_file++], p);
                                p = strtok(NULL, "__");
                            }

                            if (strncmp(args[1], args_file[2], max(strlen(args[1]), strlen(args_file[2]))) == 0)
                            {
                                remove = 1;
                            }
                            else
                            {
                                strcat(passwordsFile, fromFileBackup);
                                strcat(passwordsFile, "\n");
                            }
                        }
                        if (remove)
                            strcpy(to_client, "successfully removed");
                        else
                            strcpy(to_client, "didn't remove anything. maybe the name of the site is wrong?");
                        fPasswords.open("database/passwords.txt", std::ios_base::out);
                        fPasswords << passwordsFile;
                        fPasswords.close();
                    }
                }
                else
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    char passwordsFile[1024];
                    strcpy(passwordsFile, "");
                    char fromFileBackup[256];
                    while (1)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        strcpy(fromFileBackup, fromFile);
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            if (strncmp(args[1], args_file[2], max(strlen(args[1]), strlen(args_file[2]))) == 0 && strncmp(args[2], args_file[3], max(strlen(args[2]), strlen(args_file[3]))) == 0)
                            {
                                //do nothing
                                remove = 1;
                            }
                            else
                            {
                                strcat(passwordsFile, fromFileBackup);
                                strcat(passwordsFile, "\n");
                            }
                        }
                        else
                        {
                            strcat(passwordsFile, fromFileBackup);
                            strcat(passwordsFile, "\n");
                        }
                    }
                    if (remove)
                        strcpy(to_client, "successfully removed");
                    else
                        strcpy(to_client, "didn't remove anything. maybe the name of the site/the username is wrong?");
                    fPasswords.open("database/passwords.txt", std::ios_base::out);
                    fPasswords << passwordsFile;
                    fPasswords.close();
                }
            }
            else if (strncmp(args[0], "modify", max(strlen(args[0]), strlen("modify"))) == 0)
            {
                int modified = 0;
                if (strlen(args[1]) == 0)
                    strcpy(to_client, "Syntax: modify <url> <site_username> (if there are more than 2 entries with same url) <what_to_update: url,username,name...> <new entry>");
                else if (strlen(args[2]) > 0 && strlen(args[3]) > 0 && strlen(args[4]) == 0)
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    while (found < 2)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            if (strncmp(args[1], args_file[4], max(strlen(args[1]), strlen(args_file[4]))) == 0)
                            {
                                found++;
                            }
                        }
                    }
                    if (found == 2)
                    {
                        strcpy(to_client, "found multiple instances of the same site. specify site username to modify");
                    }
                    else
                    {
                        fstream fPasswords;
                        fPasswords.open("database/passwords.txt", ios::in);
                        int found = 0;
                        char passwordsFile[1024];
                        char newLine[256];
                        char oldLine[256];
                        strcpy(passwordsFile, "");
                        char fromFileBackup[256];
                        while (1)
                        {
                            fPasswords.getline(fromFile, 256, '\n');
                            if (fPasswords.eof())
                            {
                                fPasswords.close();
                                break;
                            }
                            int i_file = 0;
                            char args_file[20][256];
                            strcpy(fromFileBackup, fromFile);
                            p = strtok(fromFile, "__");
                            while (p)
                            {
                                strcpy(args_file[i_file++], p);
                                p = strtok(NULL, "__");
                            }

                            if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                            {
                                if (strncmp(args[1], args_file[4], max(strlen(args[1]), strlen(args_file[4]))) == 0)
                                {
                                    strcpy(oldLine, fromFileBackup);
                                    strcpy(newLine, "");
                                    for (int j = 0; j < i_file; j++)
                                    {
                                        if (strncmp(args[2], "username", max(strlen(args[2]), strlen("username"))) == 0 && j == 3)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (strncmp(args[2], "name", max(strlen(args[2]), strlen("name"))) == 0 && j == 2)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (strncmp(args[2], "password", max(strlen(args[2]), strlen("password"))) == 0 && j == 1)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (strncmp(args[2], "category", max(strlen(args[2]), strlen("category"))) == 0 && j == 5)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (strncmp(args[2], "url", max(strlen(args[2]), strlen("url"))) == 0 && j == 4)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (strncmp(args[2], "notes", max(strlen(args[2]), strlen("category"))) == 0 && j == 6)
                                        {
                                            strcat(newLine, args[3]);
                                            strcat(newLine, "__");
                                        }
                                        else if (!(strlen(args_file[6]) > 0))
                                        {
                                            strcat(newLine, "__");
                                        }
                                        else
                                        {
                                            strcat(newLine, args_file[j]);
                                            strcat(newLine, "__");
                                        }
                                    }
                                    strcat(passwordsFile, newLine);
                                    strcat(passwordsFile, "\n");
                                }
                                else
                                {
                                    strcat(passwordsFile, fromFileBackup);
                                    strcat(passwordsFile, "\n");
                                }
                            }
                            else
                            {
                                strcat(passwordsFile, fromFileBackup);
                                strcat(passwordsFile, "\n");
                            }
                        }

                        if (strncmp(oldLine, newLine, max(strlen(oldLine), strlen(newLine))) && strlen(oldLine) > 0)
                            modified = 1;
                        if (modified)
                            strcpy(to_client, "successfully modified");
                        else
                            strcpy(to_client, "didn't modify anything");
                        fPasswords.open("database/passwords.txt", std::ios_base::out);
                        fPasswords << passwordsFile;
                        fPasswords.close();
                    }
                }
                else
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    char passwordsFile[1024];
                    strcpy(passwordsFile, "");
                    char fromFileBackup[256];
                    while (1)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        strcpy(fromFileBackup, fromFile);
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            if (strncmp(args[1], args_file[4], max(strlen(args[1]), strlen(args_file[4]))) == 0 &&
                                strncmp(args[2], args_file[3], max(strlen(args[2]), strlen(args_file[3]))) == 0)
                            {
                                char newLine[256];
                                strcpy(newLine, "");
                                for (int j = 0; j < i_file; j++)
                                {
                                    if (strncmp(args[3], "username", max(strlen(args[3]), strlen("username"))) == 0 && j == 3)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (strncmp(args[3], "name", max(strlen(args[3]), strlen("name"))) == 0 && j == 2)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (strncmp(args[3], "password", max(strlen(args[3]), strlen("password"))) == 0 && j == 1)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (strncmp(args[3], "category", max(strlen(args[3]), strlen("category"))) == 0 && j == 5)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (strncmp(args[3], "url", max(strlen(args[3]), strlen("url"))) == 0 && j == 4)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (strncmp(args[3], "notes", max(strlen(args[3]), strlen("category"))) == 0 && j == 6)
                                    {
                                        strcat(newLine, args[4]);
                                        strcat(newLine, "__");
                                    }
                                    else if (!(strlen(args_file[6]) > 0))
                                    {
                                        strcat(newLine, "__");
                                    }
                                    else
                                    {
                                        strcat(newLine, args_file[j]);
                                        strcat(newLine, "__");
                                    }
                                }
                                strcat(passwordsFile, newLine);
                                strcat(passwordsFile, "\n");
                                modified = 1;
                            }
                            else
                            {
                                strcat(passwordsFile, fromFileBackup);
                                strcat(passwordsFile, "\n");
                            }
                        }
                        else
                        {
                            strcat(passwordsFile, fromFileBackup);
                            strcat(passwordsFile, "\n");
                        }
                    }
                    if (modified)
                        strcpy(to_client, "successfully modified");
                    else
                        strcpy(to_client, "didn't modify anything.");
                    fPasswords.open("database/passwords.txt", std::ios_base::out);
                    fPasswords << passwordsFile;
                    fPasswords.close();
                }
            }
            else if (strncmp(args[0], "display", max(strlen(args[0]), strlen("display"))) == 0)
            {
                if (strlen(args[1]) == 0)
                    strcpy(to_client, "Syntax: display <parameter: all, site name or certain category>");
                else if (strncmp(args[1], "all", max(strlen(args[1]), strlen("all"))) == 0)
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    char passwordsFile[1024];
                    strcpy(passwordsFile, "");
                    char fromFileBackup[256];
                    while (1)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        strcpy(fromFileBackup, fromFile);
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0)
                        {
                            strcat(to_client, args_file[2]); //site name
                            strcat(to_client, "__");
                            strcat(to_client, args_file[3]); //username site
                            strcat(to_client, "__");
                            strcat(to_client, args_file[1]); //password
                            strcat(to_client, "__");
                            strcat(to_client, args_file[4]); //url
                            strcat(to_client, "__");
                            strcat(to_client, args_file[5]); // category
                            strcat(to_client, "__");
                            strcat(to_client, args_file[6]); //notes
                            strcat(to_client, "\n");
                        }
                        for (int j = 0; j < 20; j++)
                            strcpy(args_file[j], "");
                    }
                }
                else
                {
                    fstream fPasswords;
                    fPasswords.open("database/passwords.txt", ios::in);
                    int found = 0;
                    char passwordsFile[1024];
                    strcpy(passwordsFile, "");
                    char fromFileBackup[256];
                    while (1)
                    {
                        fPasswords.getline(fromFile, 256, '\n');
                        if (fPasswords.eof())
                        {
                            fPasswords.close();
                            break;
                        }
                        int i_file = 0;
                        char args_file[20][256];
                        strcpy(fromFileBackup, fromFile);
                        p = strtok(fromFile, "__");
                        while (p)
                        {
                            strcpy(args_file[i_file++], p);
                            p = strtok(NULL, "__");
                        }

                        if (strncmp(user, args_file[0], max(strlen(user), strlen(args_file[0]))) == 0 &&
                            (strncmp(args[1], args_file[5], max(strlen(args[1]), strlen(args_file[5]))) == 0 ||
                             strncmp(args[1], args_file[2], max(strlen(args[1]), strlen(args_file[2]))) == 0))
                        {
                            found = 1;
                            strcat(to_client, args_file[2]);
                            strcat(to_client, "__");

                            strcat(to_client, args_file[3]);
                            strcat(to_client, "__");

                            strcat(to_client, args_file[1]);
                            strcat(to_client, "__");

                            strcat(to_client, args_file[4]);
                            strcat(to_client, "__");

                            strcat(to_client, args_file[5]);
                            strcat(to_client, "__");

                            strcat(to_client, args_file[6]);
                            strcat(to_client, "\n");
                        }

                        for (int j = 0; j < 20; j++)
                            strcpy(args_file[j], "");
                    }

                    if (!found)
                        strcpy(to_client, "site name not found");
                }
            }
            else
                strcpy(to_client, "???");
        }

        /* returnam mesajul clientului */
        if (write(cl, &to_client, sizeof(buffer)) <= 0)
        {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
}