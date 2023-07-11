/* cliTCPIt.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <SDL2/SDL_bgi.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

/* codul de eroare returnat de anumite apeluri */

/* portul de conectare la server*/

chrono::system_clock::time_point start;

int port;

int sd_global;
char buffer[2048];
char global_string[256];
char user[30];
char last_pass_name[30];
int how = 0;

struct drawnSpots
{
    int x1;
    int y1;
    int x2;
    int y2;
    char name_site[30];
    char username_site[30];
    char url_site[30];
    char category_site[30];
    char notes_site[30];
    char password_site[30];
};

int UI_main();
int logat = 0;

bool mycomp(string a, string b)
{
    return a < b;
}

const char *convert(const std::string &s)
{
    return s.c_str();
}

int main(int argc, char *argv[])
{
    int sd;                    // descriptorul de socket
    struct sockaddr_in server; // structura folosita pentru conectare
        // mesajul trimis
    int nr = 0;
    char bufferCopy[2048];
    char to_server[256];
    char args[20][256];
    char *p;
    int i = 0;
    strcpy(to_server, "");

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi(argv[2]);

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return -1;
    }

    sd_global = sd;

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons(port);

    /* ne conectam la server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return -1;
    }

    /* citirea mesajului */
    /*while (1)
    {
        if (write(sd, &buffer, sizeof(buffer)) <= 0)
        {
            perror("[client]Eroare la write() spre server.\n");
            return -1;
        }

        
        if (read(sd, &buffer, sizeof(buffer)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return -1;
        }


        if (strncmp(buffer, "exit", strlen(buffer)) == 0)
            break;

        printf("[server] %s\n", buffer);
    }*/

    UI_main();

    /* inchidem conexiunea, am terminat */
    close(sd);
}

void loginScreen();
void registerScreen_1();
void registerScreen_2(char *username);
void registerScreen_3();
void dashboardScreen();
void passwordScreen(char *name_site, char *username_site, char *password_site, char *url_site, char *category_site, char *notes_site);
void userInfoScreen(int passNum, char *last_pass_name);
void addScreen();
char createSpecialCharacters(SDL_Keycode key)
{
    if (key == SDLK_COMMA)
        return '<';
    if (key == SDLK_PERIOD)
        return '>';
    if (key == SDLK_SLASH)
        return '?';
    if (key == SDLK_SEMICOLON)
        return ':';
    if (key == SDLK_QUOTE)
        return '"';
    if (key == SDLK_LEFTBRACKET)
        return '{';
    if (key == SDLK_RIGHTBRACKET)
        return '}';
    if (key == SDLK_BACKSLASH)
        return '|';
    if (key == SDLK_MINUS)
        return '_';
    if (key == SDLK_EQUALS)
        return '+';
    if (key == SDLK_1)
        return '!';
    if (key == SDLK_2)
        return '@';
    if (key == SDLK_3)
        return '#';
    if (key == SDLK_4)
        return '$';
    if (key == SDLK_5)
        return '%';
    if (key == SDLK_6)
        return '^';
    if (key == SDLK_7)
        return '&';
    if (key == SDLK_8)
        return '*';
    if (key == SDLK_9)
        return '(';
    if (key == SDLK_0)
        return ')';
    if ((char)key >= 97 && (char)key <= 192)
        return toupper((char)key);
}

void setFont()
{
    settextstyle(0, HORIZ_DIR, 2);
    setfillstyle(SOLID_FILL, WHITE);
}

void send_args_and_get_server_response_in_buffer(char *command)
{
    strcpy(buffer, command);
    if (write(sd_global, &buffer, sizeof(buffer)) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
    }

    if (read(sd_global, &buffer, sizeof(buffer)) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
    }
}

void send_args_and_get_server_response_in_buffer_const(const char *command)
{
    strcpy(buffer, command);
    if (write(sd_global, &buffer, sizeof(buffer)) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
    }

    if (read(sd_global, &buffer, sizeof(buffer)) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
    }
}

int strncmp_quick(char *string1, char *string2)
{
    return strncmp(string1, string2, max(strlen(string1), strlen(string2)));
}

int strncmp_quick_const(char *string1, const char *string2)
{
    return strncmp(string1, string2, max(strlen(string1), strlen(string2)));
}
void mainScreen()
{
    delay(50);
    int x, y;
    readimagefile("images/mainScreen.bmp", 0, 0, 1366, 768);
    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);
        if (x >= 558 && x <= 811 && y >= 351 && y <= 416) // loginscreen
        {
            if (!logat)
                loginScreen();
            else
                dashboardScreen();
        }

        if (x >= 557 && x <= 813 && y >= 454 && y <= 516) //registerscreen
            registerScreen_1();

        if (x >= 557 && x <= 810 && y >= 547 && y <= 612) // exit
        {
            send_args_and_get_server_response_in_buffer_const("exit");
            if (!strncmp_quick_const(buffer, "exit"))
            {
                close(sd_global);
                exit(0);
            }
        }
    }
}

void refreshTop()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, 1366, 200);
}

void refresh_xy(int x1, int y1, int x2, int y2)
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(x1, y1, x2, y2);
}

void loginScreen()
{
    int x, y;
    char c;
    char username[20];
    char password[30];
    char stars[30];
    char starsBackup[30];
    char serverMessage[30];
    SDL_Keycode key;
    strcpy(username, "");
    strcpy(password, "");
    strcpy(stars, "");
    strcpy(serverMessage, "");
    readimagefile("images/loginScreen.bmp", 0, 0, 1366, 768);
    while (true)
    {
        cout << logat << endl;
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);
        if (x >= 585 && x <= 909 && y >= 277 && y <= 309) //username
        {
            setFont();
            bar(923, 276, 940, 310); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 200, "PRESS ENTER AFTER TYPING");
            while (true)
            {
                key = getch();
                c = (char)key;
                if ((c >= 48 && c >= 57 || c >= 97 && c <= 122) && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // shift + letters or numbers
                {
                    sprintf(username, "%s%c", username, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(username, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    //readimagefile("images/loginScreen.bmp", 0, 0, 1366, 768);
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 276, 940, 310); // unselect select bar
                    bar(483, 191, 881, 219); // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    username[strlen(username) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 48 && c <= 57) //check ascii code. c needs to be a letter
                {
                    sprintf(username, "%s%c", username, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(585, 277, 906, 309);
                outtextxy(585 - textwidth(username) / 2 + (906 - 585) / 2, 277 - 6 + (309 - 277) / 2, username); //username output
            }
        }
        if (x >= 587 && x <= 905 && y >= 351 && y <= 380) //password
        {
            setFont();
            bar(923 + 39, 276 + 71, 940 + 39, 310 + 71); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 200, "PRESS ENTER AFTER TYPING");
            while (true)
            {
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(password, "%s%c", password, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(password, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923 + 39, 276 + 71, 940 + 39, 310 + 71); // unselect select bar
                    bar(483, 191, 881, 219);                     // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    starsBackup[strlen(starsBackup) - 1] = '\0';
                    password[strlen(password) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57) //check ascii code. c can be anything almost
                {
                    sprintf(password, "%s%c", password, c);
                }

                if (c != 8)
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 351, 905, 380);
                    for (int i = 0; i < strlen(password); i++)
                        strcat(stars, "*");
                    outtextxy(587 - textwidth(stars) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, stars); //password output
                    strcpy(starsBackup, stars);
                    strcpy(stars, "");
                }
                else
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 351, 905, 380);
                    outtextxy(587 - textwidth(starsBackup) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, starsBackup); //password output delete
                }
            }
        }

        if (x >= 930 && x <= 949 && y >= 354 && y <= 374) //copy password button
        {
            if (SDL_GetClipboardText() != NULL && strlen(SDL_GetClipboardText()) < 20)
            {
                setFont();
                strcpy(password, SDL_GetClipboardText());
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(587, 351, 905, 380);
                for (int i = 0; i < strlen(password); i++)
                    strcat(stars, "*");
                outtextxy(587 - textwidth(stars) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, stars); //password output
                strcpy(starsBackup, stars);
                strcpy(stars, "");
            }
        }

        if (x >= 555 && x <= 813 && y >= 451 && y <= 497) //login
        {
            char to_server[256];
            strcpy(to_server, "");
            strcat(to_server, "login ");
            strcat(to_server, username);
            strcat(to_server, " ");
            strcat(to_server, password);

            cout << to_server << endl;

            send_args_and_get_server_response_in_buffer(to_server);

            if (strncmp_quick(buffer, "login successful") == 0)
            {
                logat = 1;
                refreshTop();
                setFont();
                start = chrono::system_clock::now();
                outtextxy(1366 / 2 - textwidth("Login successful. Please wait...") / 2, 100, "Login successful. Please wait...");
                strcpy(user, username);
                delay(1000);
                dashboardScreen();
            }
            else
            {
                refreshTop();
                setFont();
                outtextxy(1366 / 2 - textwidth(buffer) / 2, 100, buffer);
            }
        }

        if (x >= 601 && x <= 767 && y >= 617 && y <= 687) // back button
            mainScreen();

        if (x <= 100 && y <= 100) //emergency exit
            exit(0);
    }
}

void registerScreen_1()
{
    int x, y;
    readimagefile("images/register1.bmp", 0, 0, 1366, 768);
    SDL_Keycode key;
    char username[20];
    strcpy(username, "");
    while (true)
    {
        settextstyle(0, HORIZ_DIR, 2);
        setcolor(YELLOW);
        outtextxy(140, 100, "Enter a username. This will be used for the authentification process.");
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);
        setcolor(WHITE);

        if (x >= 587 && x <= 905 && y >= 351 && y <= 380) //username
        {
            //bar(585,277,909,309);
            cout << "Am intrat in partea de inregistrat." << endl;
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 276 + 71, 940, 310 + 71); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 200, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");

            char c;
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // shift + letters or numbers
                {
                    sprintf(username, "%s%c", username, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(username, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(483, 191, 881, 219);           // cover "PRESS ENTER AFTER TYPING" text
                    bar(923, 276 + 71, 940, 310 + 71); // unselect select bar
                    break;
                }
                else if (c == 8) //backspace
                {
                    username[strlen(username) - 1] = '\0';
                }
                else if (c >= 48 && c <= 57 || c >= 97 && c <= 122) //check ascii code. c can be anything almost
                {
                    sprintf(username, "%s%c", username, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(587, 351, 905, 380);
                outtextxy(587 - textwidth(username) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, username); //password output
            }
        }

        if (x >= 556 && x <= 813 && y >= 451 && y <= 494) //continue
        {
            if (strlen(username) > 0)
                registerScreen_2(username);
            else
                outtextxy((1366-textwidth("username can't be empty")) / 2, 300, "username can't be empty");
        }
        if (x >= 602 && x <= 766 && y >= 618 && y <= 687)
        {
            mainScreen();
        }
    }
}

void registerScreen_2(char *username)
{
    int x, y;
    char c;
    char password[30];
    char stars[30];
    char starsBackup[30];
    char stars_x[30];
    char starsBackup_x[30];
    char password_x[30];
    SDL_Keycode key;
    strcpy(password, "");
    strcpy(stars, "");
    strcpy(stars_x, "");
    strcpy(password_x, "");
    readimagefile("images/registerscreen_2_fixed.bmp", 0, 0, 1366, 768);
    settextstyle(0, HORIZ_DIR, 2);
    setcolor(YELLOW);
    outtextxy(1366 / 2 - textwidth("This password will be used for authentification.") / 2, 100, "This password will be used for authentification.");
    setcolor(RED);
    outtextxy(1366 / 2 - textwidth("DO NOT LOSE THIS PASSWORD! It cannot be recovered.") / 2, 150, "DO NOT LOSE THIS PASSWORD! It cannot be recovered.");
    setcolor(WHITE);
    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);
        if (x >= 585 && x <= 909 && y >= 277 && y <= 309)
        {
            //bar(585,277,909,309);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 276, 940, 310); // select bar
            settextstyle(0, HORIZ_DIR, 2);
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 200, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // shift + letters or numbers
                {
                    sprintf(password_x, "%s%c", password_x, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(password_x, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    //readimagefile("images/loginScreen.bmp", 0, 0, 1366, 768);
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 276, 940, 310); // unselect select bar
                    bar(483, 191, 881, 219); // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    starsBackup_x[strlen(starsBackup_x) - 1] = '\0';
                    password_x[strlen(password_x) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 48 && c <= 57) //check ascii code. c needs to be a letter
                {
                    sprintf(password_x, "%s%c", password_x, c);
                }

                //render settings
                if (c != 8)
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(585, 277, 906, 309);
                    for (int i = 0; i < strlen(password_x); i++)
                        strcat(stars_x, "*");
                    outtextxy(585 - textwidth(stars_x) / 2 + (906 - 585) / 2, 277 - 6 + (309 - 277) / 2, stars_x); //username output
                    strcpy(starsBackup_x, stars_x);
                    strcpy(stars_x, "");
                }
                else
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(585, 277, 906, 309);
                    outtextxy(585 - textwidth(starsBackup_x) / 2 + (906 - 585) / 2, 277 - 6 + (309 - 277) / 2, starsBackup_x); //username output delete
                }
            }
        }
        if (x >= 587 && x <= 905 && y >= 351 && y <= 380)
        {
            //bar(585,277,909,309);
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923 + 39, 276 + 71, 940 + 39, 310 + 71); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 200, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(password, "%s%c", password, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(password, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923 + 39, 276 + 71, 940 + 39, 310 + 71); // unselect select bar
                    bar(483, 191, 881, 219);                     // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    starsBackup[strlen(starsBackup) - 1] = '\0';
                    password[strlen(password) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57) //check ascii code. c can be anything almost
                {
                    sprintf(password, "%s%c", password, c);
                }

                if (c != 8)
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 351, 905, 380);
                    for (int i = 0; i < strlen(password); i++)
                        strcat(stars, "*");
                    outtextxy(587 - textwidth(stars) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, stars); //password output
                    strcpy(starsBackup, stars);
                    strcpy(stars, "");
                }
                else
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 351, 905, 380);
                    outtextxy(587 - textwidth(starsBackup) / 2 + (906 - 587) / 2, 351 - 6 + (380 - 351) / 2, starsBackup); //password output delete
                }
            }
        }

        if (x >= 555 && x <= 813 && y >= 451 && y <= 495)
        {
            if (strncmp(password, password_x, max(strlen(password), strlen(password_x))) == 0)
            {
                char to_server[256];
                strcpy(to_server, "");
                strcat(to_server, "register ");
                strcat(to_server, username);
                strcat(to_server, " ");
                strcat(to_server, password);
                send_args_and_get_server_response_in_buffer(to_server);

                if (strncmp_quick_const(buffer, "register complete") == 0)
                {
                    refreshTop();
                    setFont();
                    outtextxy(1366 / 2 - textwidth(buffer) / 2, 100, buffer);
                    delay(1500);
                    registerScreen_3();
                }
                else
                {
                    refreshTop();
                    setFont();
                    outtextxy(1366 / 2 - textwidth(buffer) / 2, 100, buffer);
                }
            }
            else
            {
                refreshTop();
                setFont();
                outtextxy(1366 / 2 - textwidth("Passwords do not match") / 2, 100, "Passwords do not match");
            }
        }

        if (x >= 601 && x <= 765 && y >= 618 && y <= 688)
        {
            registerScreen_1();
        }
    }
}

void registerScreen_3()
{
    int x, y;
    readimagefile("images/register3.bmp", 0, 0, 1366, 768);
    while (true)
    {
        setcolor(YELLOW);
        outtextxy(1366 / 2 - textwidth("Proceed to login.") / 2, 100, "Proceed to login.");
        setcolor(WHITE);
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);

        if (x >= 604 && x <= 766 && y >= 316 && y <= 379)
        {
            loginScreen();
        }

        if (x >= 602 && x <= 767 && y >= 619 && y <= 686)
        {
            mainScreen();
        }
    }
}

void dashboardScreen()
{
    int x, y;
    readimagefile("images/dashboardScreen.bmp", 0, 0, 1366, 768);

    outtextxy(480, 67, user);

    send_args_and_get_server_response_in_buffer_const("display all");

    char passwordLine[256][256];
    char passwordLine_args[50][256];
    char *p;
    int k = 0;

    char *curLine = buffer;
    while (curLine)
    {
        char *nextLine = strchr(curLine, '\n');
        if (nextLine)
            *nextLine = '\0'; // temporarily terminate the current line
        strcpy(passwordLine[k++], curLine);
        if (nextLine)
            *nextLine = '\n'; // then restore newline-char, just to be tidy
        curLine = nextLine ? (nextLine + 1) : NULL;
    }

    k--;

    drawnSpots spots[k];

    /*for (int i = 0; i < k; i++)
    {
        cout << passwordLine[i] << endl;
    }*/
    //cout << endl;

    if (how == -1 || how == 1)
    {
        char new_passwordLine[256][256];
        char new_passwordLine_args[256][256];
        vector<string> sort_argument_SortedList;
        char sort_list[30][256];

        for (int i = 0; i < k; i++)
        {
            strcpy(new_passwordLine[i], passwordLine[i]);
        }
        for (int i = 0; i < k; i++)
        {
            char *p;
            int j = 0;
            p = strtok(new_passwordLine[i], "__");
            while (p)
            {
                strcpy(new_passwordLine_args[j++], p);
                p = strtok(NULL, "__");
            }

            if (how == -1)
            {
                sort_argument_SortedList.push_back(new_passwordLine_args[4]);
            }
            else
                sort_argument_SortedList.push_back(new_passwordLine_args[0]);
        }

        sort(sort_argument_SortedList.begin(), sort_argument_SortedList.end(), mycomp);

        for (int i = 0; i < k; i++)
        {
            strcpy(sort_list[i], convert(sort_argument_SortedList.at(i)));
        }

        int count = 0;

        for (int i = 0; i < k; i++)
        {
            strcpy(new_passwordLine[i], passwordLine[i]);
        }

        for (int i = 0; i < 10; i++)
        {
            strcpy(passwordLine_args[i], "");
        }

        char third_passwordLine[256][256];
        for (int x = 0; x < k; x++)
        {

            for (int i = 0; i < k; i++)
            {
                char *p;
                int j = 0;
                p = strtok(new_passwordLine[i], "__");
                while (p)
                {
                    strcpy(new_passwordLine_args[j++], p);
                    p = strtok(NULL, "__");
                }

                if (how == -1)
                {
                    if (strncmp_quick(sort_list[x], new_passwordLine_args[4]) == 0)
                    {
                        strcpy(third_passwordLine[count++], passwordLine[i]);
                        i = k;
                    }
                }
                else
                {
                    if (strncmp_quick(sort_list[x], new_passwordLine_args[0]) == 0)
                    {
                        strcpy(third_passwordLine[count++], passwordLine[i]);
                        i = k;
                    }
                }
            }

            for (int j = 0; j < k; j++)
            {
                strcpy(new_passwordLine[j], passwordLine[j]);
            }
        }

        for (int i = 0; i < k; i++)
        {
            strcpy(passwordLine[i], third_passwordLine[i]);
            cout << passwordLine[i] << endl;
        }
    }

    int x_orig = 20;
    int x_coord = 20, y_coord = 140;
    int offset_x = 50;
    int offset_y = 80;
    int offset_y_rect = 50;
    int offset_x_rect = 20;
    int offset_x_displayText = 100;
    for (int i = 0; i < k; i++)
    {
        int j = 0;
        p = strtok(passwordLine[i], "__");
        while (p)
        {
            strcpy(passwordLine_args[j++], p);
            p = strtok(NULL, "__");
        }

        char notes[256];
        strcpy(notes, "");
        p = strtok(passwordLine_args[5], "###");
        while (p)
        {
            strcat(notes, p);
            strcat(notes, " ");
            p = strtok(NULL, "###");
        }

        char name[256];
        strcpy(name, "");
        p = strtok(passwordLine_args[0], "###");
        while (p)
        {
            strcat(name, p);
            strcat(name, " ");
            p = strtok(NULL, "###");
        }

        char category[256];
        strcpy(category, "");
        p = strtok(passwordLine_args[4], "###");
        while (p)
        {
            strcat(category, p);
            strcat(category, " ");
            p = strtok(NULL, "###");
        }

        strcpy(passwordLine_args[4], category);
        strcpy(passwordLine_args[0], name);
        strcpy(passwordLine_args[5], notes);

        if (max(strlen(passwordLine_args[0]), strlen(passwordLine_args[5])) + x_coord < 800)
        {
            if (strlen(passwordLine_args[0]) > 0 && strlen(passwordLine_args[4]) > 0)
            {
                setfillstyle(SOLID_FILL, BLUE);
                bar(x_coord, y_coord, x_coord + max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4])) + offset_x_rect, y_coord + offset_y_rect);
                spots[i].x1 = x_coord;
                spots[i].y1 = y_coord;
                spots[i].x2 = x_coord + max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4])) + offset_x_rect;
                spots[i].y2 = y_coord + offset_y_rect;
                strcpy(spots[i].name_site, passwordLine_args[0]);
                strcpy(spots[i].username_site, passwordLine_args[1]);
                strcpy(spots[i].notes_site, passwordLine_args[5]);
                strcpy(spots[i].password_site, passwordLine_args[2]);
                strcpy(spots[i].category_site, passwordLine_args[4]);
                strcpy(spots[i].url_site, passwordLine_args[3]);
                setFont();
                outtextxy(x_coord + offset_x_rect / 2, y_coord + offset_y_rect / 10, passwordLine_args[0]);
                outtextxy(x_coord + offset_x_rect / 2, y_coord + 2 * offset_y_rect / 4, passwordLine_args[4]);
                x_coord += max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4]));
                x_coord += offset_x;
            }
        }
        else
        {
            if (strlen(passwordLine_args[0]) > 0 && strlen(passwordLine_args[4]) > 0)
            {
                x_coord = x_orig;
                y_coord += offset_y;
                setfillstyle(SOLID_FILL, BLUE);
                bar(x_coord, y_coord, x_coord + max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4])) + offset_x_rect, y_coord + offset_y_rect);
                spots[i].x1 = x_coord;
                spots[i].y1 = y_coord;
                spots[i].x2 = x_coord + offset_x_rect + max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4]));
                spots[i].y2 = y_coord + offset_y_rect;
                strcpy(spots[i].name_site, passwordLine_args[0]);
                strcpy(spots[i].username_site, passwordLine_args[1]);
                strcpy(spots[i].notes_site, passwordLine_args[5]);
                strcpy(spots[i].password_site, passwordLine_args[2]);
                strcpy(spots[i].category_site, passwordLine_args[4]);
                strcpy(spots[i].url_site, passwordLine_args[3]);
                setFont();
                outtextxy(x_coord + offset_x_rect / 2, y_coord + offset_y_rect / 10, passwordLine_args[0]);
                outtextxy(x_coord + offset_x_rect / 2, y_coord + 2 * offset_y_rect / 4, passwordLine_args[4]);
                x_coord += max(textwidth(passwordLine_args[0]), textwidth(passwordLine_args[4]));
                x_coord += offset_x;
            }
        }

        for (int x = 0; x < j; x++)
            strcpy(passwordLine_args[x], "");
    }

    if (how == 0)
    {
        cout << how << endl;
        strcpy(last_pass_name, spots[k - 1].name_site);
    }

    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);

        for (int i = 0; i < k; i++)
        {
            if (x >= spots[i].x1 && x <= spots[i].x2 && y >= spots[i].y1 && y <= spots[i].y2)
            {
                cout << "Clicked password " << i << endl;
            }
        }

        if (x >= 1124 && x <= 1342 && y >= 575 && y <= 621) //backtomenu
        {
            mainScreen();
        }

        if (x >= 1124 && x <= 1341 && y >= 173 && y <= 218)
        {
            addScreen();
        }

        if (x >= 1125 && x <= 1341 && y >= 339 && y <= 384) //userinfo
        {
            userInfoScreen(k, last_pass_name);
        }

        if (x >= 1123 && x <= 1342 && y >= 257 && y <= 303)
        {
            setFont();
            refresh_xy(296, 9, 1302, 60);
            outtextxy((1366 - textwidth("Click which password to remove")) / 2 + offset_x_displayText, 30, "Click which password to remove");
            while (!ismouseclick(WM_LBUTTONDOWN))
            {
            }
            getmouseclick(WM_LBUTTONDOWN, &x, &y);
            if (x >= 1123 && x <= 1342 && y >= 257 && y <= 303)
            {
                //do nothing and quit removing
                dashboardScreen();
            }
            else
            {
                for (int i = 0; i < k; i++)
                {
                    if (x >= spots[i].x1 && x <= spots[i].x2 && y >= spots[i].y1 && y <= spots[i].y2)
                    {
                        char to_server[256];
                        strcpy(to_server, "");
                        strcat(to_server, "remove ");
                        spots[i].name_site[strlen(spots[i].name_site) - 1] = '\0';
                        char *p;
                        char name_site_clone[30];
                        char actual_site_name[30];
                        strcpy(actual_site_name, "");
                        strcpy(name_site_clone, spots[i].name_site);
                        p = strtok(name_site_clone, " ");
                        while (p)
                        {
                            strcat(actual_site_name, p);
                            strcat(actual_site_name, "###");
                            p = strtok(NULL, " ");
                        }
                        strcat(to_server, actual_site_name);
                        strcat(to_server, " ");
                        strcat(to_server, spots[i].username_site);
                        cout << to_server << endl;
                        send_args_and_get_server_response_in_buffer(to_server);
                    }
                }
            }
            setFont();
            refresh_xy(296, 9, 1302, 60);
            outtextxy((1366 - textwidth(buffer)) / 2 + 50, 30, buffer);
            delay(500);
            dashboardScreen();
        }

        if (x >= 1125 && x <= 1340 && y >= 493 && y <= 539)
        {
            cout << "LOGOUT" << endl;
            send_args_and_get_server_response_in_buffer_const("logout");
            setFont();
            refresh_xy(296, 9, 1302, 41);
            outtextxy((1366 - textwidth("Logging out...")) / 2 + offset_x_displayText, 50, "Logging out...");
            logat = 0;
            delay(500);
            loginScreen();
        }

        if (x >= 1125 && x <= 1341 && y >= 416 && y <= 461)
        {
            //how = -1 sort alphabetically by category
            //how = 0 default
            //how = 1 sort alphabetically by name
            if (how == -1)
                how = 0;
            else if (how == 0)
                how = 1;
            else
                how = -1;

            if (how == 0)
            {
                refresh_xy(296, 9, 1302, 41);
                outtextxy((1366 - textwidth("Sorting by date added...")) / 2 + offset_x_displayText, 50, "Sorting by date added...");
            }

            if (how == -1)
            {
                refresh_xy(296, 9, 1302, 41);
                outtextxy((1366 - textwidth("Sorting by date added...")) / 2 + offset_x_displayText, 50, "Sorting by category...");
            }

            if (how == 1)
            {
                refresh_xy(296, 9, 1302, 41);
                outtextxy((1366 - textwidth("Sorting by date added...")) / 2 + offset_x_displayText, 50, "Sorting by site name...");
            }

            delay(1000);
            dashboardScreen();
        }

        for (int i = 0; i < k; i++)
        {
            if (x >= spots[i].x1 && x <= spots[i].x2 && y >= spots[i].y1 && y <= spots[i].y2)
            {
                passwordScreen(spots[i].name_site, spots[i].username_site, spots[i].password_site, spots[i].url_site, spots[i].category_site, spots[i].notes_site);
            }
        }
    }
}

void passwordScreen(char *name_site, char *username_site, char *password_site, char *url_site, char *category_site, char *notes_site)
{
    int x, y;
    readimagefile("images/passwordscreen_final_final_final.bmp", 0, 0, 1366, 768);

    int count = 0;
    char to_display[6][256];
    strcpy(to_display[count++], name_site);
    strcpy(to_display[count++], username_site);
    strcpy(to_display[count++], password_site);
    strcpy(to_display[count++], url_site);
    strcpy(to_display[count++], category_site);
    strcpy(to_display[count++], notes_site);
    int x_coord = 248;
    int y_coord = 177;
    int y_coord_offset = 50;
    int select_square_size = 20;
    drawnSpots spots[6];
    for (int i = 0; i < 6; i++)
    {
        setFont();
        outtextxy(x_coord, y_coord, to_display[i]);
        bar(x_coord + textwidth(to_display[i]) + 10, y_coord, x_coord + textwidth(to_display[i]) + 10 + select_square_size, y_coord + select_square_size);
        spots[i].x1 = x_coord + textwidth(to_display[i]) + 10;
        spots[i].y1 = y_coord;
        spots[i].x2 = x_coord + textwidth(to_display[i]) + 10 + select_square_size;
        spots[i].y2 = y_coord + select_square_size;
        y_coord += y_coord_offset;
    }

    for (int i = 0; i < 6; i++)
    {
        cout << spots[i].x1 << " " << spots[i].y1 << " " << spots[i].x2 << " " << spots[i].y2 << endl;
    }
    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);

        if (x >= 66 && x <= 355 && y >= 513 && y <= 580)
        {
            refresh_xy(382, 0, 1188, 79);
            outtextxy(1366 / 2, 50, "Select which camp to modify");
            while (!ismouseclick(WM_LBUTTONDOWN))
            {
            }
            getmouseclick(WM_LBUTTONDOWN, &x, &y);
            for (int i = 0; i < 6; i++)
            {
                //Syntax: modify <url> <site_username> (if there are more than 2 entries with same url) <what_to_update: url,username,name...> <new entry>
                if (x >= spots[i].x1 && x <= spots[i].x2 && y >= spots[i].y1 && y <= spots[i].y2)
                {
                    refresh_xy(382, 0, 1188, 79);
                    outtextxy(1366 / 2, 50, "Type new entry");
                    SDL_Keycode key;
                    char c;
                    char entry[30];
                    strcpy(entry, "");

                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(398, 515, 1315, 585);

                    while (true)
                    {
                        key = getch();
                        c = (char)key;
                        if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                        {
                            sprintf(entry, "%s%c", entry, createSpecialCharacters(key));
                        }
                        else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                        {
                            strcpy(entry, SDL_GetClipboardText());
                        }
                        else if (c == 13) //enter
                        {
                            break;
                        }
                        else if (c == 8) //backspace
                        {
                            entry[strlen(entry) - 1] = '\0';
                        }
                        else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57 || c == 32) //check ascii code. c can be anything almost
                        {
                            sprintf(entry, "%s%c", entry, c);
                        }

                        cout << entry << endl;

                        setFont();
                        setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                        bar(398, 515, 1315, 585);
                        outtextxy(398 + 20, 515 + 20, entry);
                    }

                    if (i == 5 || i == 0 || i == 4)
                    {
                        char *p;
                        char entry_fix[30];
                        strcpy(entry_fix, "");
                        p = strtok(entry, " ");
                        while (p)
                        {
                            strcat(entry_fix, p);
                            strcat(entry_fix, "###");
                            p = strtok(NULL, " ");
                        }
                        strcpy(entry, entry_fix);
                    }

                    char to_server[256];
                    strcpy(to_server, "modify ");
                    strcat(to_server, url_site);
                    strcat(to_server, " ");
                    strcat(to_server, username_site);
                    switch (i)
                    {
                    case 0:
                        strcat(to_server, " name ");
                        break;
                    case 1:
                        strcat(to_server, " username ");
                        break;
                    case 2:
                        strcat(to_server, " password ");
                        break;
                    case 3:
                        strcat(to_server, " url ");
                        break;
                    case 4:
                        strcat(to_server, " category ");
                        break;
                    case 5:
                        strcat(to_server, " notes ");
                        break;
                    }
                    strcat(to_server, entry);

                    cout << to_server << endl;

                    send_args_and_get_server_response_in_buffer(to_server);
                    if (strncmp_quick_const(buffer, "successfully modified") == 0)
                    {
                        refresh_xy(382, 0, 1188, 79);
                        setFont();
                        outtextxy(1366 / 2, 50, "Modified with success");
                        delay(1000);
                        dashboardScreen();
                    }
                    else
                    {
                        setFont();
                        refresh_xy(382, 0, 1188, 79);
                        outtextxy(1366 / 2, 50, buffer);
                    }
                }
            }
        }

        if (x >= 66 && x <= 349 && y >= 626 && y <= 693)
        {
            SDL_SetClipboardText(password_site);
            refresh_xy(382, 0, 1188, 79);
            outtextxy(1366 / 2, 50, "Copied password to clipboard");
        }

        if (x >= 401 && x <= 682 && y >= 626 && y <= 694)
        {
            SDL_SetClipboardText(url_site);
            refresh_xy(382, 0, 1188, 79);
            outtextxy(1366 / 2, 50, "Copied URL to clipboard");
        }

        if (x >= 1212 && x <= 1308 && y >= 22 && y <= 70)
        {
            dashboardScreen();
        }
    }
}

void userInfoScreen(int passNum, char *last_pass_name)
{
    int x, y;
    readimagefile("images/userinfo_fixed.bmp", 0, 0, 1366, 768);
    auto end = chrono::system_clock::now();
    chrono::duration<double> value = end - start;
    int duration = value.count();
    int minutes = duration / 60;
    int hours = minutes / 60;
    int seconds = duration % 60;
    char time[30];
    char minibuffer[30];
    strcpy(time, "");
    sprintf(minibuffer, "%d", hours);
    if (hours > 9)
        strcat(time, minibuffer);
    else
    {
        strcat(time, "0");
        strcat(time, minibuffer);
    }
    strcat(time, ":");
    sprintf(minibuffer, "%d", minutes);
    if (minutes > 9)
        strcat(time, minibuffer);
    else
    {
        strcat(time, "0");
        strcat(time, minibuffer);
    }
    strcat(time, ":");
    sprintf(minibuffer, "%d", seconds);
    if (seconds > 9)
        strcat(time, minibuffer);
    else
    {
        strcat(time, "0");
        strcat(time, minibuffer);
    }
    outtextxy(286, 320, time);
    outtextxy(136, 37, user);
    sprintf(minibuffer, "%d", passNum);
    outtextxy(325, 369, minibuffer);
    outtextxy(358, 408, last_pass_name);
    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);

        if (x >= 1213 && x <= 1307 && y >= 21 && y <= 72)
        {
            dashboardScreen();
        }
    }
}

char *generatePassword()
{
    static char password[20];
    srand(time(NULL));
    char c;
    for (int i = 0; i < 10; i++)
    {
        //temp = rand() % ( high - low + 1 ) + low
        c = rand() % (126 - 33 + 1) + 33;
        while (c == 95 || c == 35)
        {
            c = rand() % (126 - 33 + 1) + 33;
        }

        sprintf(password, "%s%c", password, c);
    }
    cout << password << endl;
    return password;
}
void addScreen()
{
    int x, y;
    char username[20];
    char url[40];
    char notes[20];
    char name[20];
    char category[20];
    char password[20];
    char starsBackup[20];
    char stars[20];
    strcpy(password, "");
    strcpy(starsBackup, "");
    strcpy(stars, "");
    strcpy(username, "");
    strcpy(url, "");
    strcpy(category, "");
    strcpy(notes, "");
    strcpy(name, "");
    char c;
    SDL_Keycode key;
    readimagefile("images/addscreen_final.bmp", 0, 0, 1366, 768);
    setfillstyle(SOLID_FILL, RED);
    bar(963, 139, 963 + 10 + textwidth("generate password"), 139 + 10 + textheight("generate password"));
    outtextxy(963 + 7, 139 + 4, "generate password");
    while (true)
    {
        while (!ismouseclick(WM_LBUTTONDOWN))
        {
        }
        getmouseclick(WM_LBUTTONDOWN, &x, &y);
        cout << "The mouse was clicked at: ";
        cout << "x=" << x;
        cout << " y=" << y << endl;
        delay(100);

        if (x >= 963 && x <= 929 + 50 + textwidth("generate password") && y >= 139 && y <= 139 + 10 + textheight("generate password"))
        {
            strcpy(password, generatePassword());
            //sprintf(password,"%s",generatePassword());
            setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
            bar(587, 142, 905, 171);
            for (int i = 0; i < strlen(password); i++)
                strcat(stars, "*");
            outtextxy(587 - textwidth(stars) / 2 + (906 - 587) / 2, 142 + textheight(stars) - 6, stars); //password output
            strcpy(starsBackup, stars);
            strcpy(stars, "");
        }

        if (x >= 587 && x <= 906 && y >= 142 && y <= 171) //pasword
        {
            cout << "ENTER PASSWORD" << endl;
            setFont();
            bar(923 + 10, 142, 940 + 10, 171); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            while (true)
            {
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(password, "%s%c", password, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(password, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923 + 10, 142, 940 + 10, 171); // unselect select bar
                    bar(483, 91, 881, 119);            // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    starsBackup[strlen(starsBackup) - 1] = '\0';
                    password[strlen(password) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57) //check ascii code. c can be anything almost
                {
                    sprintf(password, "%s%c", password, c);
                }

                if (c != 8)
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 142, 905, 171);
                    for (int i = 0; i < strlen(password); i++)
                        strcat(stars, "*");
                    outtextxy(587 - textwidth(stars) / 2 + (906 - 587) / 2, 142 + textheight(stars) - 6, stars); //password output
                    strcpy(starsBackup, stars);
                    strcpy(stars, "");
                }
                else
                {
                    setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                    bar(587, 142, 905, 171);
                    outtextxy(587 - textwidth(starsBackup) / 2 + (906 - 587) / 2, 142 + textheight(starsBackup) - 6, starsBackup); //password output delete
                }
            }
        }

        if (x >= 585 && x <= 909 && y >= 277 && y <= 309) //username
        {
            cout << "ENTER USERNAME" << endl;
            //bar(585,277,909,309);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 276, 940, 310); // select bar
            settextstyle(0, HORIZ_DIR, 2);
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(username, "%s%c", username, createSpecialCharacters(key));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(username, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    //readimagefile("images/loginScreen.bmp", 0, 0, 1366, 768);
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 276, 940, 310); // unselect select bar
                    bar(483, 91, 881, 119);  // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    username[strlen(username) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57)
                {
                    sprintf(username, "%s%c", username, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(585, 277, 906, 309);
                outtextxy(585 - textwidth(username) / 2 + (906 - 585) / 2, 277 - 6 + (309 - 277) / 2, username);
            }
            continue;
        }
        if (x >= 587 && x <= 1127 && y >= 351 && y <= 380) //url
        {
            cout << "ENTER URL" << endl;
            //bar(585,277,909,309);
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923 + 39 + 185, 276 + 71, 940 + 39 + 185, 310 + 71); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(url, "%s%c", url, createSpecialCharacters(c));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 40)
                {
                    strcpy(url, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923 + 39 + 185, 276 + 71, 940 + 39 + 185, 310 + 71); // unselect select bar
                    bar(483, 91, 881, 119);                                  // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    url[strlen(url) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57)
                {
                    sprintf(url, "%s%c", url, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(585, 351, 1127, 380);
                outtextxy(585 - textwidth(url) / 2 + (1127 - 585) / 2, 351 - 6 + (380 - 351) / 2, url);
            }
        }

        if (x >= 586 && x <= 906 && y >= 209 && y <= 238) //name
        {
            cout << "ENTER NAME" << endl;
            //bar(585,277,909,309);
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 276 - 72, 940, 310 - 72); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(name, "%s%c", name, createSpecialCharacters(c));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(name, SDL_GetClipboardText());
                }
                else if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 276 - 72, 940, 310 - 72); // unselect select bar
                    bar(483, 91, 881, 119);            // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    name[strlen(name) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57 || c == 32)
                {
                    sprintf(name, "%s%c", name, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(585, 209, 906, 235);
                outtextxy(585 - textwidth(name) / 2 + (906 - 586) / 2, 215, name); //name output delete
            }
            continue;
        }

        if (x >= 586 && x <= 906 && y >= 423 && y <= 450) //category
        {
            cout << "ENTER CATEGORY" << endl;
            //bar(585,277,909,309);
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 416, 940, 416 + 36); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                key = getch();
                c = (char)key;
                if (key != SDLK_MINUS && (SDL_GetModState() & KMOD_LSHIFT || SDL_GetModState() & KMOD_RSHIFT)) // special cahract shift + something
                {
                    sprintf(category, "%s%c", category, createSpecialCharacters(c));
                }
                else if (key == SDLK_v && SDL_GetModState() & KMOD_CTRL && strlen(SDL_GetClipboardText()) < 20)
                {
                    strcpy(category, SDL_GetClipboardText());
                }
                else if (c == 13)
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 416, 940, 416 + 36); // unselect select bar
                    bar(483, 91, 881, 119);       // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }
                else if (c == 8) //backspace
                {
                    category[strlen(category) - 1] = '\0';
                }
                else if (c >= 97 && c <= 122 || c >= 43 && c <= 47 || c == 59 || c == 61 || c >= 91 && c <= 93 || c == 96 || c == 39 || c >= 48 && c <= 57 || c == 32)
                {
                    sprintf(category, "%s%c", category, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(587, 423, 906, 450);
                outtextxy(587 - textwidth(category) / 2 + (906 - 587) / 2, 423 - 6 + (450 - 423) / 2, category); //category output
            }
            continue;
        }

        if (x >= 586 && x <= 906 && y >= 490 && y <= 517) //notes
        {
            cout << "ENTER NOTES" << endl;
            //bar(585,277,909,309);
            settextstyle(0, HORIZ_DIR, 2);
            setfillstyle(SOLID_FILL, WHITE);
            bar(923, 484, 940, 518); // select bar
            outtextxy(1366 / 2 - textwidth("PRESS ENTER AFTER TYPING") / 2, 100, "PRESS ENTER AFTER TYPING");
            //outtextxy(585,277,"work");
            while (true)
            {
                //bar(585,277,909,309);
                c = (char)getch();
                if (c == 13) //enter
                {
                    setfillstyle(SOLID_FILL, BLACK);
                    bar(923, 484, 940, 518); // unselect select bar
                    bar(483, 91, 881, 119);  // cover "PRESS ENTER AFTER TYPING" text
                    break;
                }

                if (c == 8) //backspace
                {
                    notes[strlen(notes) - 1] = '\0';
                }

                if (c >= 33 && c <= 126 || c == 32) //check ascii code. c can be anything almost
                {
                    sprintf(notes, "%s%c", notes, c);
                }
                setfillstyle(SOLID_FILL, COLOR(54, 57, 63));
                bar(587, 490, 906, 517);
                outtextxy(587 - textwidth(notes) / 2 + (906 - 587) / 2, 490 - 6 + (517 - 490) / 2, notes); //category output
            }
            continue;
        }

        if (x >= 582 && x <= 815 && y >= 589 && y <= 657)
        {
            if (strlen(password) > 0 && strlen(name) > 0 && strlen(username) > 0 && strlen(url) > 0 && strlen(category) > 0)
            { //"Syntax: add <password> <site_name> <username> <url> <category> <notes>"

                cout << password << " " << name << " " << username << " " << url << " " << category << " " << notes << endl;
                char to_server[256];
                strcpy(to_server, "");
                strcat(to_server, "add ");
                strcat(to_server, password);
                strcat(to_server, " ");

                char *p_1;
                char name_fixed[30];
                strcpy(name_fixed, "");
                p_1 = strtok(name, " ");
                while (p_1)
                {
                    strcat(name_fixed, p_1);
                    strcat(name_fixed, "###");
                    p_1 = strtok(NULL, " ");
                }
                strcat(to_server, name_fixed);

                strcat(to_server, " ");
                strcat(to_server, username);
                strcat(to_server, " ");
                strcat(to_server, url);
                strcat(to_server, " ");

                char *p_2;
                char category_fixed[30];
                strcpy(category_fixed, "");
                p_2 = strtok(category, " ");
                while (p_2)
                {
                    strcat(category_fixed, p_2);
                    strcat(category_fixed, "###");
                    p_2 = strtok(NULL, " ");
                }
                strcat(to_server, category_fixed);

                strcat(to_server, " ");

                char *p;
                char notes_fixed[30];
                strcpy(notes_fixed, "");
                p = strtok(notes, " ");
                while (p)
                {
                    strcat(notes_fixed, p);
                    strcat(notes_fixed, "###");
                    p = strtok(NULL, " ");
                }
                strcat(to_server, notes_fixed);

                cout << to_server << endl;

                send_args_and_get_server_response_in_buffer(to_server);

                refresh_xy(500, 96, 865, 123);
                setFont();

                if (strncmp_quick_const(buffer, "add complete") == 0)
                {
                    dashboardScreen();
                }
                else
                {
                    refresh_xy(0, 96, 865, 123);
                    outtextxy(1366 / 2 - textwidth(buffer) / 2, 100, buffer);
                }
            }
            else
            {
                refresh_xy(500, 96, 865, 123);
                setFont();
                outtextxy(1366 / 2 - textwidth("null argument detected") / 2, 100, "null argument detected");
            }
        }

        if (x >= 1213 && x <= 1307 && y >= 21 && y <= 72)
        {
            dashboardScreen();
        }
    }
}

void drawPasswords()
{
}
int UI_main()
{
    int gdriver = DETECT, gmode, errorcode;
    initgraph(&gdriver, &gmode, "");
    initwindow(1366, 768);
    mainScreen();
    closegraph();
}