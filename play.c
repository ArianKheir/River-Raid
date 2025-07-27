#include "header.h"
#include <SDL2/SDL.h>

//struct for the plabe itself containing the coordinates heath and score
typedef struct havapeyma{
    int x, y;
    int hl;//health
    int sc; //score
} plane;

//making a linked list for enemies to update
typedef struct enemy_Node{
    int type;
    int x, y;
    int hl,sc, dm;//health score damage
    struct enemy_Node *next;
}node;

//adding enemies to the list of enemies
void add(node **head){
    node *nd = (node *)(malloc(sizeof(node)));
    nd->next = (*head);
    *head = nd;
    return;
}

//finding the enemy with ind_th index
node *get_node(node *head, int ind){
    if(ind == 0){
		return head;
    }
    return get_node(head->next, --ind);
}

//deleting the enemy with ind-th index
void del(node **head, int ind){
    if (ind == 0){
        node *pos = (*head)->next;
        free(*head);
        *head = pos;
        return;
    }

    /*node *pos = *head;
    for(int i = 0 ; i < ind - 1 ; i ++)
        pos = pos->next;*/
    node *pos = get_node(*head, ind - 1);
    node *tmp = pos->next;
    pos->next = (pos->next)->next;
    free(tmp);
    return;
}

//making t=a linked list for the bullets we shoot
//containing the coordinates and the damage
typedef struct shelik_Node{
    int x, y;
    int dm; // damage
    struct shelik_Node *next;
}nodeb;


//adding bullets to the list of our bullets
void addb(nodeb **head){
    nodeb *nd = (nodeb *)(malloc(sizeof(nodeb)));
    nd->next = (*head);
    *head = nd;
    return;
}

//finding the bullt with ind_th index
nodeb *get_nodeb(nodeb *head, int ind){
    if(ind == 0){
		return head;
    }
    return get_nodeb(head->next, --ind);
}

//deleting the bullet with ind-th index
void delb(nodeb **head, int ind){
    if (ind == 0){
        nodeb *pos = (*head)->next;
        free(*head);
        *head = pos;
        return;
    }

    /*nodeb *pos = *head;
    for(int i = 0 ; i < ind - 1 ; i ++)
        pos = pos->next;*/
    nodeb *pos = get_nodeb(*head, ind - 1); 
    nodeb *tmp = pos->next;
    pos->next = (pos->next)->next;
    free(tmp);
    return;
}

//freeing up the space we used
void freeup(node *head, nodeb *headb){
	node *pos = head;
	node *tmp;
	while(pos != NULL){
		tmp = pos->next;
		free(pos);
		pos = tmp;
	}
	nodeb *posb = headb;
	nodeb *tmpb;
	while(posb != NULL){
		tmpb = posb->next;
		free(posb);
		posb = tmpb;
	}
    return;    
}

//the design and the colors of the plane
char plane_texture[][3] = {
    {' ', '*', ' '},
    {'*', '*', '*'},
    {' ', '*', ' '},
    {'*', '*', '*'}
};

int plane_cl[][3] = {
    {BLACK_WHITE, WHITE_BLACK, BLACK_WHITE},
    {WHITE_BLACK, WHITE_BLACK, WHITE_BLACK},
    {BLACK_WHITE, WHITE_BLACK, BLACK_WHITE},
    {WHITE_BLACK, WHITE_BLACK, WHITE_BLACK}
};

//the design and the colors of the meen
char meen_ch[][2] ={
    {'#', '#'},
    {'#', '#'}
};

int meen_cl[][2] ={
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK}
};

//the design and the colors of the safineh baari
char safine_ch[][2] ={
    {'-','-'},
    {'|', '|'},
    {'|', '|'},
    {'|', '|'},
    {'-','-'}
};

int safine_cl[][2] ={
    {BLACK_WHITE,BLACK_WHITE},
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK},
    {BLACK_WHITE,BLACK_WHITE}
};

//the design and the colors of the jangandeh
char jangandeh_ch[][2] ={
    {'|', '|'},
    {'|', '|'},
    {'\\', '/'},
};

int jangandeh_cl[][2] ={
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK},
    {RED_BLACK,RED_BLACK}
};

//the design and the colors of the bombafkan
char bombaf_ch[][2] ={
    {'/','\\'},
    {'|', '|'},
    {'|', '|'},
    {'\\', '/'},
};

int bombaf_cl[][2] ={
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK},
    {RED_BLACK, RED_BLACK},
    {RED_BLACK,RED_BLACK}
};

//the design and the colors of the pahpad
char pahpad_ch[][2] ={
    {'/','\\'},
    {'\\', '/'}
};

int pahpad_cl[][2] ={
    {RED_BLACK, RED_BLACK},
    {RED_BLACK,RED_BLACK}
};

//the design and the colors of the bullets our enemies shoot
//bullets that our enemies shoot are considered as a new type of enemy
char enemyshelik_ch[1][1]={'.'};
int enemyshelik_cl[1][1] = {BLACK_WHITE};

void chaap(int x, int y, int h, int w, char txt_ch[h][w], int txt_cl[h][w], char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]);
void enemy_besaz(node **head, int lvl, int cnt);
void upd_enemies(node **head, plane p, int cnt);
void chap_enemy(node **head, char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]);
void upd_hl(node **head, nodeb **headb, plane *p);
void upd_shelik(nodeb **head);
void chaap_shelik(nodeb **headb, char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]);

// the main function to play
int play_bokon(int lvl){
    int c;
    //the board we see on screen
    char ch[HEIGHT][WIDTH];
    int cl[HEIGHT][WIDTH];
  
    //initail info of our plane
    plane p;
    p.x = HEIGHT / 2;
    p.y = (WIDTH - 5) / 2;
    p.hl = 1000;
    p.sc = 0;
    //cnt is number of times we updated the board
    int cnt = 1;
    //making heads for our list of enemies and list of bullets we shoot
    node *head = NULL;
    nodeb *headb = NULL;
    while(true){
        //if we ran out of health we die
        if(p.hl <= 0){
            for(int i = 0 ; i < HEIGHT ; i ++){
                for(int j = 0 ; j < WIDTH ; j ++){
                    cl[i][j] = BLACK_WHITE;
                    ch[i][j] = ' ';
                }
            }
            //the you lost massege and the score
            char youlost[] = "You Lost!";
            int tmp = sprintf((&ch[HEIGHT/2][(WIDTH - strlen(youlost)) / 2]), "%s", youlost);
            *(&ch[HEIGHT/2][(WIDTH - strlen(youlost)) / 2] + tmp) = ' ';
            for(int i = 0 ; i < tmp ; i ++)
                *(&cl[HEIGHT/2][(WIDTH - strlen(youlost)) / 2] + i) = RED_BLACK;
            char scr[] = "score = ", scr2[10];
            int len = sprintf(scr2, "%d", p.sc);
            len += strlen(scr);
            tmp = sprintf((&ch[HEIGHT/2 - 1][(WIDTH - len) / 2]), "score = %d", p.sc);
            *(&ch[HEIGHT/2 - 1][(WIDTH - len) / 2] + tmp) = ' ';
            for(int i = 0 ; i < tmp ; i ++)
                *(&cl[HEIGHT/2 - 1][(WIDTH - len) / 2] + i) = RED_BLACK;   

            show(cl, ch);
            //freeup(head, headb);
            SDL_Delay(2000);
            return p.sc;
        }
        //getting input
        c = getch();
        //flushing the buffer from useless things
        //fflush(stdout);
        flushinp();
        //upadting the coordinates of our plane or shooting bullets
        if (c == 'w' || c == 'W' || c == KEY_UP)
            p.x--;
        else if (c == 's' || c == 'S' || c == KEY_DOWN)
            p.x++;
        else if (c == 'a' || c == 'A' || c == KEY_LEFT)
            p.y--;
        else if (c == 'd' || c == 'D' || c == KEY_RIGHT)
            p.y++;
        else if(c == '\n'|| c == ' ' || c == KEY_ENTER){
            //shooting bullets
            //adding to our shooting linked list
            addb(&headb);
            headb->x = p.x - 1;
            headb->y = p.y + 1;
            headb->dm = 75;
        }
        //for quiting
        else if (c == 'q' || c == 'Q'){
            //freeup(head, headb);
            return p.sc;
        }
        //if the plane gets out of the screen we lose
        if(p.x < -5 || p.x > HEIGHT - 4 || p.y < -3 || p.y > WIDTH){
            p.hl = -1;
            continue;
        }
        //initializing the board
        for(int i = 0 ; i < HEIGHT ; i ++){
            for(int j = 0 ; j < WIDTH ; j ++){
                cl[i][j] = BLACK_WHITE;
                ch[i][j] = ' ';
            }
        }
        //updating the two lists we have , applying damages, deleting enemies if they crash to our plane ,etc
        upd_hl(&head, &headb, &p);
        //printing the health at the top of the page
        int tmp = sprintf((&ch[1][2]), "Health = %d", p.hl);
        *(&ch[1][2] + tmp) = ' ';
        tmp = sprintf((&ch[1][WIDTH - 20]), "score = %d", p.sc);
        *(&ch[1][WIDTH - 20] + tmp) = ' ';
        //making new enemies by the random function and considering the difficulity
        enemy_besaz(&head, lvl, cnt);
        //updating the coordinates of the enemies and making the bullets that some of them shoot
        //and also deleting them if they get out of screen 
        upd_enemies(&head, p, cnt);
        //updating the coordinates of the bullets we shoot
        //and also deleting them if they get out of screen 
        upd_shelik(&headb);
        //updating the board for the bullets we shoot
        chaap_shelik(&headb, ch, cl);
        //upadting the board for the enemis that move
        chap_enemy(&head, ch, cl);
        //updating the place of our plane on the board
        chaap(p.x, p.y, sizeof(plane_texture) / sizeof(plane_texture[0]), sizeof(plane_texture[0]) / sizeof(plane_texture[0][0]), plane_texture, plane_cl,ch, cl);
        //printing the new updated board
        show(cl, ch);
        //number of times we ran this loop is plused one
        cnt ++;
        //upadting the score due to the time the player is playing
        if(cnt % 100 == 0)
            p.sc += 20; 
        //a delay before running the next loop
        SDL_Delay(100);
    }
}

//function to update the place of an object we want on the borad
void chaap(int x, int y, int h, int w, char txt_ch[h][w], int txt_cl[h][w], char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]){
    for(int i = 0 ; i < h ; i ++){
        for(int j = 0 ; j < w ; j ++){
            int i2 = x + i;
            int j2 = y + j;
            if (i2 < 0 || i2 >= HEIGHT || j2 < 0 || j2 >= WIDTH)
                continue;
            if (txt_ch[i][j] == ' ' && txt_cl[i][j] == BLACK_WHITE)
                continue;
            //updating the borad with the character and the color it should be
            ch[i2][j2] = txt_ch[i][j];
            cl[i2][j2] = txt_cl[i][j];
        }
    }
    return;
}

// maing enemies with random function and considering the difficulity
void enemy_besaz(node **head, int lvl, int cnt){
    //finding the difficulty due the player's choice and the time the person is playing
    int lv = 0;
    if(cnt > 1000 && cnt < 1500){
        lv = 1;
    }
    else if(cnt >= 1500 && cnt < 6000){
        lv = 2;
    }
    else{
        lv = 3;
    }
    lv = fmin(lv, lvl);

    //making more enemies for harder difficulities
    int eht[7] = {10, 10, 8, 7, 6, 5};
    for(int i = 0 ; i < lv ; i ++){
        for(int j = 0 ; j < 6 ; j ++){
            eht[j] += 5;
        }
    }

    if((rand() % 1000) < eht[0]){
        //meen
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = rand() % WIDTH;
        (*head)->type = 1;
        (*head)->hl = 150;
        (*head)->sc = 50;
        (*head)->dm = 150;
        (*head)->sc = 20;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }

    if((rand() % 1000) < eht[1]){
        //safine
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = rand() % WIDTH;
        (*head)->type = 2;     
        (*head)->hl = 250;
        (*head)->dm = 100;
        (*head)->sc = 40;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }

    if((rand() % 1000) < eht[2]){
        //jangandeh
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = rand() % WIDTH;
        (*head)->type = 3;     
        (*head)->hl = 270;
        (*head)->dm = 200;
        (*head)->sc = 50;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }

    if((rand() % 1000) < eht[3]){
        //bombafkan
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = WIDTH - (rand() % (WIDTH / 2));
        (*head)->type = 4;     
        (*head)->hl = 350;
        (*head)->dm = 220;
        (*head)->sc = 90;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }

    if((rand() % 1000) < eht[4]){
        //bombafkan from the other side
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = rand() % (WIDTH / 2);
        (*head)->type = 5;     
        (*head)->hl = 350;
        (*head)->dm = 220;
        (*head)->sc = 90;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }

    if((rand() % 1000) < eht[5]){
        //pahpad
        //adding to the enemies list and initializing the info of it
        add(head);
        (*head)->x = 0;
        (*head)->y = rand() % WIDTH;
        (*head)->type = 6;     
        (*head)->hl = 150;
        (*head)->dm = 290;
        (*head)->sc = 150;
        //make it harder to be killed and stronger for harder difficulities
        for(int i = 0 ; i < lv ; i ++){
            (*head)->hl += 5;
            (*head)->dm += 5;
        }
    }
    return;
}

//function to update the enemies place on the borad using chaap function
void chap_enemy(node **head, char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]){
    node *pos = *head;
    for( ; pos != NULL ; pos = pos->next){
        if(pos->type == 1){
            chaap(pos->x, pos->y, sizeof(meen_ch) / sizeof(meen_ch[0]), sizeof(meen_ch[0]) / sizeof(meen_ch[0][0]), meen_ch, meen_cl,ch, cl);
        }
        if(pos->type == 2){
            chaap(pos->x, pos->y, sizeof(safine_ch) / sizeof(safine_ch[0]), sizeof(safine_ch[0]) / sizeof(safine_ch[0][0]), safine_ch, safine_cl,ch, cl);
        }
        if(pos->type == 3){
            chaap(pos->x, pos->y, sizeof(jangandeh_ch) / sizeof(jangandeh_ch[0]), sizeof(jangandeh_ch[0]) / sizeof(jangandeh_ch[0][0]), jangandeh_ch, jangandeh_cl,ch, cl);
        }
        if(pos->type == 4 || pos->type == 5){
            chaap(pos->x, pos->y, sizeof(bombaf_ch) / sizeof(bombaf_ch[0]), sizeof(bombaf_ch[0]) / sizeof(bombaf_ch[0][0]), bombaf_ch, bombaf_cl,ch, cl);
        }
        if(pos->type == 6){
            chaap(pos->x, pos->y, sizeof(pahpad_ch) / sizeof(pahpad_ch[0]), sizeof(pahpad_ch[0]) / sizeof(pahpad_ch[0][0]), pahpad_ch, pahpad_cl,ch, cl);
        }
        if(pos->type == 7){
            chaap(pos->x, pos->y, sizeof(enemyshelik_ch) / sizeof(enemyshelik_ch[0]), sizeof(enemyshelik_ch[0]) / sizeof(enemyshelik_ch[0][0]), enemyshelik_ch, enemyshelik_cl,ch, cl);
        }
    }
    return;
}

//function to update the coordinates of the enemies and making the bullets some of them shoot
void upd_enemies(node **head, plane p, int cnt){
    node *pos = *head;
    for(int i = 0 ; pos != NULL ; i ++){
        if(pos->type == 1){
            //meen
            if (cnt % 10 == 0)
                pos->x ++;
        }
        else if(pos->type == 2){
            //safine bari
            if (cnt % 10 == 3)
                pos->x += 2;
        }
        else if(pos->type == 3){
            //jangandeh
            //making the bullets for it
            if(cnt % 5 == 0){
                add(head);
                (*head)->x = pos->x + sizeof(jangandeh_ch[0]) / sizeof(jangandeh_ch[0][0]);
                (*head)->y = pos->y + (sizeof(jangandeh_ch) / sizeof(jangandeh_ch[0])) / 2 - 1;
                (*head)->type = 7;     
                (*head)->hl = 0;
                (*head)->dm = 50;
                i ++;
            }
            if (cnt % 10 == 5)
                pos->x += 2;
        }
        else if(pos->type == 4){
            //bombafkan
            //making the bullets for it
            if(cnt % 5 == 0){
                add(head);
                (*head)->x = pos->x + sizeof(bombaf_ch[0]) / sizeof(bombaf_ch[0][0]);
                (*head)->y = pos->y + (sizeof(bombaf_ch) / sizeof(bombaf_ch[0])) / 2 - 1;
                (*head)->type = 7;     
                (*head)->hl = 0;
                (*head)->dm = 50;
                i ++;
            }
            if (cnt % 10 == 0){
                pos->x++;
                pos->y --;
            }
        }
        else if(pos->type == 5){
            //bombafkan from the other side
            //making the bullets for it
            if(cnt % 5 == 0){
                add(head);
                (*head)->x = pos->x + sizeof(bombaf_ch[0]) / sizeof(bombaf_ch[0][0]);
                (*head)->y = pos->y + (sizeof(bombaf_ch) / sizeof(bombaf_ch[0])) / 2 - 1;
                (*head)->type = 7;     
                (*head)->hl = 0;
                (*head)->dm = 50;
                i ++;
            }
            if (cnt % 10 == 0){
                pos->x++;
                pos->y ++;
            } 
        }
        else if(pos->type == 6){
            //pahpad
            if (cnt % 4 == 0){
                if (p.x < pos->x)
                    pos->x --;
                else if (p.x > pos->x)
                    pos->x ++;

                if (p.y < pos->y)
                    pos->y --;
                else if (p.y > pos->y)
                    pos->y ++;
            }
        }
        else if(pos->type == 7){
            //the bullets that enemies shoot
            pos->x ++;
        }
        //deleting the ones outside of the board
        if((pos->x) > HEIGHT || (pos->y) > WIDTH || (pos->y) < -5){
            pos = pos->next;
            del(head, i--);
        }
        else{
            pos = pos->next;
        }
    }
    return;  
}

//function for checking if two object crash into eachother
int covering(int x1, int y1, int h1, int w1, char txt1[h1][w1], int x2, int y2, int h2, int w2, char txt2[h2][w2]) {
    for(int i = 0 ; i < h1 ; i ++){
        for(int j = 0 ; j < w1 ; j ++){
            //we dont need to check
            if(txt1[i][j] == ' '){
                continue;
            }
            int i2 = i + x1 - x2;
            int j2 = j + y1 - y2;
            //we dont need to check
            if(i2 < 0 || i2 >= h2 || j2 < 0 || j2 >= w2){
                continue;
            }
            //the crash happens
            if(txt2[i2][j2] != ' '){
                return 1;
            }
        }
    }
    //crash doesn't happen
    return 0;
}

//updateing the enemies and our bulltes list cheking if there any crashing with our plane or if our bullets hit the enemies
void upd_hl(node **head, nodeb **headb, plane *p){
    node *pos = *head;
    for(int j = 0; pos!= NULL ; j ++){
        int h = 0;
        int w = 0;
        char **txt = NULL;
        //knowing the type of the enemy to give it to the covering function
        if(pos->type == 1){
            txt = meen_ch;
            h = sizeof(meen_ch) / sizeof(meen_ch[0]);
            w = sizeof(meen_ch[0]) / sizeof(meen_ch[0][0]);
        }
        else if(pos->type == 2){
            txt = safine_ch;
            h = sizeof(safine_ch) / sizeof(safine_ch[0]);
            w = sizeof(safine_ch[0]) / sizeof(safine_ch[0][0]);
        }
        else if(pos->type == 3){
            txt = jangandeh_ch;
            h = sizeof(jangandeh_ch) / sizeof(jangandeh_ch[0]);
            w = sizeof(jangandeh_ch[0]) / sizeof(jangandeh_ch[0][0]);
        }
        else if(pos->type == 4 || pos->type == 5){
            txt = bombaf_ch;
            h = sizeof(bombaf_ch) / sizeof(bombaf_ch[0]);
            w = sizeof(bombaf_ch[0]) / sizeof(bombaf_ch[0][0]);
        }
        else if(pos->type == 6){
            txt = pahpad_ch;
            h = sizeof(pahpad_ch) / sizeof(pahpad_ch[0]);
            w = sizeof(pahpad_ch[0]) / sizeof(pahpad_ch[0][0]);
        }
        else if(pos->type == 7){
            txt = enemyshelik_ch;
            h = sizeof(enemyshelik_ch) / sizeof(enemyshelik_ch[0]);
            w = sizeof(enemyshelik_ch[0]) / sizeof(enemyshelik_ch[0][0]);
        }      
        nodeb *posb = *headb;
        bool rm = 0;
        //check if our bullets hit the enemies
        for(int i = 0 ; posb != NULL  ; i ++){
            if(posb->x >= pos->x && posb->y >= pos->y && posb->x < pos->x + h && posb->y < pos->y + w){
                pos->hl -= posb->dm;
                if(pos->hl <= 0){
                    // if the health is under zero remove the enemy
                    rm = 1;
                    p->sc += pos->sc;
                    pos = pos->next;
                    del(head, j--);
                }
                posb = posb->next;
                delb(headb, i--);
                //dont need to check more of the enemy is dead
                if(rm){
                    break;
                }
            }
            else{
                posb = posb->next;
            }
        }
        //dont need to check the rest if the enemy is dead
        if(rm){
            continue;
        }
        //checking if the enemy crash into our plane
        if(covering(p->x, p->y, sizeof(plane_texture) / sizeof(plane_texture[0]), sizeof(plane_texture[0]) / sizeof(plane_texture[0][0]), plane_texture,
                    pos->x, pos->y, h, w, txt)){
            //updating the plane's health
            p->hl = (p->hl) - (pos->dm);
            //updating the score we've got till now
            p->sc += (pos->sc);
            //removing the enemy
            pos = pos->next;
            del(head, j--);
        }
        else{
            pos = pos->next;
        }
    }
    return;
}

//function to update the coordinates of our bullets and deleting them if they are out of screen
void upd_shelik(nodeb **headb){
    nodeb *pos = *headb;
    for(int i = 0 ; pos != NULL ; i ++){
        pos->x --;
        if((pos->x) < 0){
            pos = pos->next;
            delb(headb, i --);
        }
        else{
            pos = pos->next;
        }
    }
    return;
}

//functionto update the board for the place of our bullets
void chaap_shelik(nodeb **headb, char ch[HEIGHT][WIDTH], int cl[HEIGHT][WIDTH]){
    nodeb *pos = *headb;
    for( ; pos != NULL ; pos = pos->next){
        ch[pos->x][pos->y] = '.';
        cl[pos->x][pos->y] = BLACK_WHITE;
    }
    return;
}