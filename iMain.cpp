#include "iGraphics.h"
#include <iostream>
#include "iSound.h"
using namespace std;

int bgSoundIdx = -1; // for back ground sound
int gameover = 0;    // game over
int about_us_pg = 0;
int menu = 1;
int exiting = 0;
Image bg, mario, aboutus, gamepad;
int fornewgame = 0;
#define max_width 1200
#define max_hight 600
#define row 10
#define column 40
int xball = max_width / 2, yball = 50, radius = 5;
int velocity = 10; // ball speed
int dxball = velocity * cos(rand()), dyball = velocity * sin(rand());
// board initialization
int dxboard = 100;
int dyboard = 5;
int xboard = max_width / 2 - dxboard / 2;
int yboard = 50 - radius - dyboard;
bool initialize = false;
int soundoff = 0;
int help = 0;
int game_life = 3;
bool win1;
bool game_completed = false;
int t;
int dx = 30;
int dy = 14;
bool brick[row][column];

int score = 0;
int options = 0;
// brick features
int bricks[row][column];
int brickRed[row][column];
int brickGreen[row][column];
int brickBlue[row][column];
#define MAX_LEVEL 5
int level = 1;
// functions
void ballchange();
void boardchange();
void restart();
void initializing_brick();
void creating_bricks();

void breakingbrick();
void creating_brick();
void initializing_brick();
void win();
void level_initialize();
void decision_of_winning();
void level_increase();
void start();
/*

function iDraw() is called again and again by the system.
*/
void loadResources()
{

    iLoadImage(&bg, "assets/images/homepg.jpeg");
    iResizeImage(&bg, max_width, max_hight);
    iLoadImage(&mario, "assets/images/gamebg.jpg");
    iResizeImage(&mario, max_width, max_hight);
    iLoadImage(&aboutus, "assets/images/background.png");
    iResizeImage(&aboutus, max_width, max_hight);
}
void iDraw()
{
    iClear();

    if (menu == 1)
    {

        iShowImage(0, 0, "assets/images/homepg.jpeg");
        iSetColor(52, 235, 88);
        iTextBold(550, 360, "NEW GAME", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 330, "LOAD GAME", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 300, "OPTIONS", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 270, "HIGH SCORES", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 240, "HELP", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 210, "ABOUT US", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 180, "EXIT", GLUT_BITMAP_HELVETICA_18);
    }

    if (about_us_pg)
    {

        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(510, 350, "THE GAME was made by:", GLUT_BITMAP_HELVETICA_18);
        iText(510, 300, "1)NAZMUL HASAN RAFI", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 270, "Roll-2405017", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 230, "2) Md Raihan Hossan", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 200, "Roll-2405020", GLUT_BITMAP_TIMES_ROMAN_24);

        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    if (help)
    {
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(510, 450, "DX-BALL", GLUT_BITMAP_HELVETICA_18);
        iText(250, 400, "Break all the bricks using the ball and paddle to win the level.", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 370, "Move the paddle left/right.The ball bounces off walls, paddle, and bricks.", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 340, "When the ball hits a brick, the brick breaks (or weakens).", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 310, "For each brick you break, your score increases by 10 points.", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 280, "Break ALL the bricks to complete the level.", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 250, "In each level there are 3 lifes. ", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(250, 220, "If you break all the bricks wihtin those life then you will be updated to a new level.", GLUT_BITMAP_TIMES_ROMAN_24);

        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    if (exiting)
    {
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(510, 350, "Do you want to quit this game ?", GLUT_BITMAP_HELVETICA_18);
         iTextBold(530, 320, "YES", GLUT_BITMAP_HELVETICA_18);
         iTextBold(590, 320, "NO", GLUT_BITMAP_HELVETICA_18);

    }

    if (fornewgame == 1)
    {
        iClear();

        
        iSetColor(255, 255, 0);
        iShowImage(0, 0, "assets/images/gamebg.jpg");

        iFilledCircle(xball, yball, radius);
        // iShowImage(xboard,yboard,"assets/images/gamepad.png");
        iFilledRectangle(xboard, yboard, dxboard, dyboard);
        creating_bricks();
        breakingbrick();
        iSetColor(34, 240, 212);
        iFilledRectangle(5, 570, 6, 20);
        iFilledRectangle(17, 570, 6, 20);
        iSetColor(0, 0, 0);
        win();
        iSetColor(255, 255, 255);

        char scoreText[50];
        sprintf(scoreText, "Score: %d", score);
        iText(60, max_hight - 20, scoreText, GLUT_BITMAP_HELVETICA_18);

        char lifeText[50];
        sprintf(lifeText, "Lifes: %d", game_life);
        iText(200, max_hight - 20, lifeText, GLUT_BITMAP_HELVETICA_18);

        char leveltext[50];
        sprintf(leveltext, "Level: %d", level);
        iText(340, max_hight - 20, leveltext, GLUT_BITMAP_HELVETICA_18);
    }
    if (options == 1)
    {
        iClear();
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(480, 350, "SOUND :", GLUT_BITMAP_HELVETICA_18);
        iTextBold(620, 350, "ON", GLUT_BITMAP_HELVETICA_18);
        iTextBold(660, 350, "OFF", GLUT_BITMAP_HELVETICA_18);

        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }

    if (gameover == 1)
    {

        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(255, 0, 0);
        iTextAdvanced(max_width / 2 - 200, max_hight / 2, "GAME OVER!", 0.5, 2.0);
        char finalScore[50];
        sprintf(finalScore, "Final Score: %d", score);
        iTextAdvanced(max_width / 2 - 200, max_hight / 2 - 100, finalScore, 0.5, 2.0);
        iSetColor(68, 240, 34);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
}

/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // place your codes here
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
        if ((mx >= 550 && mx <= 590) && (my >= 210 && my <= 225))
        {
            about_us_pg = 1;
            menu = 0;
        }
        if (about_us_pg == 1 && ((mx >= 30 && mx <= 60) && (my >= 30 && my <= 60)))
        {
            menu = 1;
            about_us_pg = 0;
        }
        
        if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 240 && my <= 260))
        {
            menu = 0;
            help = 1;
        }
        if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 180 && my <= 200))
        {
            menu = 0;
            exiting = 1;
        }
        if(exiting==1&&(mx >=530 && mx <=550)&&(my >=320 && my<=340))
        {
            exit(0);
        }
        if(exiting==1&&(mx >=590 && mx <=620)&&(my >=320 && my<=340))
        {
           exiting=0;
           menu=1;
        }

        if (help == 1 && ((mx >= 30 && mx <= 60) && (my >= 30 && my <= 60)))
        {
            menu = 1;
            help = 0;
        }


        if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 360 && my <= 380))
        {
            menu = 0;
            fornewgame = 1;
            score = 0;
            game_life = 3;
            initialize = false;
            start();
        }
        if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 300 && my <= 320))
        {
            menu = 0;
            options = 1;
        }
        if (options == 1 && (mx >= 660 && mx <= 680) && (my >= 350 && my <= 380))
        {

            iPauseSound(bgSoundIdx);
        }
        if (options == 1 && (mx >= 620 && mx <= 640) && (my >= 350 && my <= 380))
        {

            iResumeSound(bgSoundIdx);
        }
        if (options == 1 && ((mx >= 30 && mx <= 60) && (my >= 30 && my <= 60)))
        {
            menu = 1;
            options = 0;
        }
        if (gameover == 1 && ((mx >= 30 && mx <= 110) && (my >= 30 && my <= 70)))
        {
            menu = 1;
            gameover = 0;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            initializing_brick();

            xball = max_width / 2;
            yball = 50;
            dxball = 10;
            dyball = 5;
            initialize = false;

            xboard = max_width / 2 - dxboard / 2;
        }
        if (fornewgame == 1 && ((mx >= 0 && mx <= 20) && (my >= 570 && my <= 600)))
        {
            menu = 1;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            initializing_brick();
            xball = max_width / 2;
            yball = 50;
            dxball = 10;
            dyball = 5;
            initialize = false;
            xboard = max_width / 2 - dxboard / 2;
        }
        /*if (score == 10 && ((mx >= 30 && mx <= 110) && (my >= 30 && my <= 70)))
        {
            score = 0;
            game_life = 3;
            initializing_brick();

            xball = max_width / 2;
            yball = 50;
            dxball = 10;
            dyball = 5;
            initialize = false;

            xboard = max_width / 2 - dxboard / 2;
            homepage = 1;
            fornewgame = 0;
            gameover = 0;
        }*/

        // lse
        // ousearea=0;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    switch (key)
    {

    case ' ':
    {
        if (!initialize)
        {
            initialize = true;
        }
        break;
    }

    default:
        break;
    }
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_RIGHT)
    {
        if (xboard < max_width)
        {

            if (xboard + dxboard < max_width)
            {
                xboard += 50;
                if (!initialize)
                {
                    xball += 50;
                }
            }
        }
    }
    else if (key == GLUT_KEY_LEFT)
    {

        if (xboard > 0)
        {
            xboard -= 50;
            if (!initialize)
            {
                xball -= 50;
            }
        }
    }
}
void level_initialize(int level)
{
    char name[20];
    sprintf(name, "level%d.txt", level);
    FILE *f = fopen(name, "r");
    if (f == NULL)
    {
        printf("No level.\n");
        return;
    }
    // game_life=3;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            fscanf(f, "%d", &bricks[i][j]);
            brick[i][j] = bricks[i][j];
        }
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (brick[i][j])
            {
                brickRed[i][j] = rand() % 255;
                brickGreen[i][j] = rand() % 255;
                brickBlue[i][j] = rand() % 255;
            }
        }
    }

    fclose(f);
}
void start()
{

    level_initialize(level);
}

void ballchange()
{
    if (initialize)
    {
        xball += dxball;
        yball += dyball;
        if (xball + radius >= max_width || xball - radius <= 0)
        {
            dxball = -dxball;
        }
        if (yball + radius >= max_hight)
        {

            dyball = -dyball;
        }

        {
            if ((xball > xboard && xboard + dxboard > xball) && (yball - radius <= yboard + dyboard))
            {
                dyball = -dyball;
            }
            else
                dyball = dyball;
        }

        if (yball - radius < 0)
        {
            restart();
        }
        decision_of_winning(); // check if all bricks are broken
        level_increase();
    }
}
void initializing_brick()
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            brick[i][j] = true;
        }
    }
}
void restart()
{
    game_life--;
    if (game_life > 0)
    {

        xball = max_width / 2, yball = 50, radius = 5;
        dxball = 10, dyball = 5;
        dxboard = 100;
        dyboard = 5;
        xboard = max_width / 2 - dxboard / 2;
        yboard = 50 - radius - dyboard;
        initialize = false;
    }
    else
    {
        gameover = 1;
    }
}

void creating_bricks()
{
    for (int i = 0; i < row; i++)
    {

        for (int j = 0; j < column; j++)
        {

            if (brick[i][j])
            {
                int x = dx * (j + 1);
                int y = 440 + dy * (i);

                iSetColor(brickRed[i][j], brickGreen[i][j], brickBlue[i][j]);
                iFilledRectangle(x, y, dx, dy);
            }
        }
    }
}
void breakingbrick()
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {

            if (brick[i][j])
            {

                if (xball + radius >= dx * (j + 1) && xball - radius <= dx * (j + 1) + dx && yball + radius >= 450 + dy * i && yball - radius <= 450 + dy * i + dy)
                {

                    brick[i][j] = false;
                    dyball = -dyball;
                    // dxball=-dxball;
                    score += 10;
                }
            }
        }
    }
}
void decision_of_winning()
{

    win1 = true;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (brick[i][j] == true)
            {
                win1 = false;
                break;
            }
        }
    }
    if (win1 && level == MAX_LEVEL - 1)
        win();
}
void win()
{

    if (level == 5)
    {
        iPauseTimer(1);
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(51, 255, 255);
        iTextAdvanced(500, 300, "you win!", 0.5, 2.0);

        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);

        // iTextBold(550,300,"you win!",GLUT_BITMAP_8_BY_13);
    }
}
void level_increase()
{
    if (win1)
    {
        level++;
        //score = 0;
        game_life = 4;
        if (level < MAX_LEVEL)
        {
            level_initialize(level);
            restart();
        }
        else
        {
            iPauseTimer(t);
            game_completed = true;
        }
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    t = iSetTimer(20, ballchange);

    // place your own initialization codes here.

    loadResources();
    iInitializeSound();
    bgSoundIdx = iPlaySound("assets/sounds/background.wav", true, 100);

    iInitialize(1200, 600, "DXBALL");

    return 0;
}
