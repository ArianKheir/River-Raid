#include "header.h"

void show_menu(){
    // the board we output
    int cl[HEIGHT][WIDTH];
    char ch[HEIGHT][WIDTH];
    // the buttons
    char *s[] = {"play", "logs", "difficulty", "exit"};
    //initializing the board
    for (int i = 0 ; i < HEIGHT ; i ++){
        for (int j = 0 ; j < WIDTH ; j ++){
            cl[i][j] = BLACK_WHITE;
            ch[i][j] = ' ';
        }
        // placing the buttons
        if(HEIGHT/2 <= i && i < HEIGHT/2 + 4){
            sprintf(&ch[i][WIDTH / 2 - strlen(s[i - HEIGHT/2])/ 2], "%s", s[i - HEIGHT / 2]);
            *(&ch[i][WIDTH / 2 - strlen(s[i - HEIGHT/2])/ 2] + strlen(s[i - HEIGHT/2])) = ' ';
        }
    }

    int c;
    //now is the button we are on it now and before is the last button we were on before now
    int now = 0, before = 1, lvl = 0;
    //running the menu 
    while(true){
        //getting input
        c = getch();
        if (c != ERR) {
            //updating the button we are choosing
            if(c == 'w' || c == 'W' || c == KEY_UP){
                now --;
            }
            else if(c == 's' || c == 'S' || c == KEY_DOWN){
                now ++;
            }
            else if(c == '\n'|| c == ' ' || c == KEY_ENTER){
                //selecting the button
                if(now == 0){
                    //running the game and checking the times
                    time_t tme;
                    time(&tme);
                    struct tm *curtime = localtime(&tme);
                    time_t fr = time(NULL);
                    int sc = play_bokon(lvl);
                    time_t sec = time(NULL);
                    //saving the score and level in a file to read in logs
                    FILE *fp = fopen("scores.txt", "a");
                    char sakhti[][20] = {"easy", "medium", "hard", "nightmare"};
                    fprintf(fp, "score = %d diff = %s time = %ds date = %s", sc, sakhti[lvl], sec - fr,asctime(curtime));
                    fclose(fp);
                }
                else if(now == 1){
                    // reading the logs
                    logs();
                }
                else if(now == 2){
                    //changing the difficulity
                    lvl = diff();
                }
                else if(now == 3){
                    //exit button
                    return;
                }
            }
            //going to first after last button and vice versa 
            now = (now + 4) % 4;
        }
        //using the >button< for the button we are on it
        *(&ch[before + HEIGHT / 2][WIDTH / 2 - strlen(s[before])/ 2] + strlen(s[before])) = ' ';
        *(&ch[before + HEIGHT / 2][WIDTH / 2 - strlen(s[before])/ 2] - 1) = ' ';
        before = now;
        *(&ch[now + HEIGHT / 2][WIDTH / 2 - strlen(s[now])/ 2] + strlen(s[now])) = '<';
        *(&ch[now + HEIGHT / 2][WIDTH / 2 - strlen(s[now])/ 2] - 1) = '>';
        //updating the voard we are watching
        show(cl, ch);
    }
    return;
}