
#include "iGraphics.h"
#include <iostream>
#include "iSound.h"
#include <ctime>
#include <math.h>
#include <time.h>
#include <stdlib.h>

using namespace std;
Image bg, mario, aboutus, gamepad, heart_img, game_win_img, ball_img, gaming_bg, gameplay1, gameplay2, gameplay3, game_over_img;
Image brick_img, heart2, unbreakable_brick, mouse;
// all defining...
#define max_width 1200
#define max_height 600 // Fixed typo: max_hight -> max_height
#define row 12
#define column 40
#define PI 3.14159265359
#define radius 5
#define FIRE_BALL_DURATION 15000
#define MAX_BALLS 3
#define MAX_LEVEL 5
float angle;
int bgSoundIdx = -1;
int breakin_sound = -1;
int paddle_sound = -1; // for background sound
int gameover = 0;      // game over
int about_us_pg = 0;
int menu = 1;
int exiting = 0;
int fornewgame = 0;
int mx1, my1;
typedef struct
{
    int x, y;
    int dx, dy;
    bool active;
} Ball;

Ball balls[MAX_BALLS];

int velocity = 10;
int dxball = velocity * cos(rand()) + 5, dyball = velocity * sin(rand()) + 5;

// board initialization
int dxboard = 100;
int dyboard = 5;
int xboard = max_width / 2 - dxboard / 2;
int yboard = 50 - radius - dyboard;

int xball = xboard + (dxboard / 2), yball = 50;

float speed;

bool initialize = false;
// sound ...
int soundoff = 0;
// help ...
int help = 0;
// game life will be 3 initially...
int game_life = 3;
//...
// resume page
int resum_pg = 0;
bool win1;
bool game_completed = false;
int t, t1;
int score = 0;
int options = 0;
// they are for brick's height and width...
int dx = 30;
int dy = 14;
// fireball...
bool fireBallActive = false;
int fireBallTimer = 0;
// paddle increasing and decreasing system...
bool paddle_increase_activate = false;
bool paddle_decrease_activate = false;
time_t paddle_power_start_time = 0;
int paddle_power_duration = 10;
// invisible bricks...
bool invisible_brick = true;
int _time = 0;

// brick features
int brick[row][column];
int bricks[row][column];
int brickRed[row][column];
int brickGreen[row][column];
int brickBlue[row][column];
// level...
int level = 1;

char playerName[30] = {'\0'};
int enteringName = 1; // 1 = name input mode
int nameCharIndex = 0;
int highscores = 0;
bool scoreSaved = false;
int flag2 = 0;

// velocity and speed...
time_t time_for_velocity = 0; // seconds...
int time_standered_for_velocity = 15;
bool speed_increase = false;
bool speed_decrease = false;
// functions
void ballchange();
void boardchange();
void restart();
void creating_bricks();

void breakingbrick();
void win();
void level_initialize(int level); // Updated declaration
void decision_of_winning();
void level_increase();
void start();
void loadHighScores();
void saveHighScores();
void addHighScore(int score, const char *name);
void showHighScores();
void initializing_brick();
void activateMultiball(int mainBallIndex);
void activate_fire_ball();

void paddle_increase();
void paddle_decrease();
void save_game();
void loadGame();
// msg
int rectangle = 0;
bool msg1 = false, msg2 = false, msg3 = false, msg4 = false;

int msg1_timer_count = 0, msg2_timer_count = 0, msg3_timer_count = 0, msg4_timer_count = 0;

#define MAX_HIGHSCORES 10

typedef struct
{
    char name[30];
    int score;
} HighScore;

HighScore highScores[MAX_HIGHSCORES];
int highScoreCount = 0;

void loadHighScores()
{
    FILE *fp = fopen("highscore.txt", "r");
    if (fp == NULL)
    {
        highScoreCount = MAX_HIGHSCORES;
        for (int i = 0; i < MAX_HIGHSCORES; i++)
        {
            strcpy(highScores[i].name, "rafi");
            highScores[i].score = 0;
        }
        saveHighScores();
        return;
    }

    highScoreCount = 0;
    while (fscanf(fp, "%s %d", highScores[highScoreCount].name, &highScores[highScoreCount].score) == 2)
    {
        highScoreCount++;
        if (highScoreCount >= MAX_HIGHSCORES)
            break;
    }

    fclose(fp);
}

void saveHighScores()
{
    FILE *fp = fopen("highscore.txt", "w");
    if (fp == NULL)
        return;

    for (int i = 0; i < highScoreCount; i++)
    {
        fprintf(fp, "%s %d\n", highScores[i].name, highScores[i].score);
    }

    fclose(fp);
}

void addHighScore(int score, const char *name)
{
    if (strlen(name) == 0)
        name = "PLAYER";

    if (highScoreCount < MAX_HIGHSCORES || score > highScores[highScoreCount - 1].score)
    {
        if (highScoreCount < MAX_HIGHSCORES)
            highScoreCount++;

        strcpy(highScores[highScoreCount - 1].name, name);
        highScores[highScoreCount - 1].score = score;

        for (int i = highScoreCount - 1; i > 0; i--)
        {
            if (highScores[i].score > highScores[i - 1].score)
            {
                HighScore temp = highScores[i];
                highScores[i] = highScores[i - 1];
                highScores[i - 1] = temp;
            }
            else
                break;
        }

        saveHighScores();
    }
}

void showHighScores()
{
    iShowImage(0, 0, "assets/images/background.png");
    iSetColor(34, 240, 212);
    iText(450, 470, " --Top 10 High Scores--", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(470, 435, "player name                  Score", GLUT_BITMAP_HELVETICA_18);

    FILE *high_sco = fopen("highscore.txt", "r");
    if (high_sco == NULL)
    {
        printf("Error: highscore.txt not found.\n");
        iText(510, 400, "No high scores available.", GLUT_BITMAP_HELVETICA_18);
        return;
    }

    char palyer[16];
    int sco;
    int i = 0;
    while (i < MAX_HIGHSCORES && fscanf(high_sco, "%s %d", palyer, &sco) == 2)
    {

        iText(490, 400 - i * 30, palyer, GLUT_BITMAP_HELVETICA_18);
        char str[100];
        sprintf(str, "%d", sco);
        iText(670, 400 - i * 30, str, GLUT_BITMAP_HELVETICA_18);
        sprintf(str, "%d. ", i + 1);
        iText(460, 400 - i * 30, str, GLUT_BITMAP_HELVETICA_18);

        i++;
    }

    fclose(high_sco);
}

void loadResources()
{
    iLoadImage(&bg, "assets/images/homepg.jpeg");
    iResizeImage(&bg, max_width, max_height);
    iLoadImage(&mario, "assets/images/gamebg.jpg");
    iResizeImage(&mario, max_width, max_height);
    iLoadImage(&aboutus, "assets/images/background.png");
    iResizeImage(&aboutus, max_width, max_height);
    iLoadImage(&heart_img, "assets/images/heart.png");
    iResizeImage(&heart_img, 4, 1);
    iLoadImage(&gamepad, "assets/images/game_paddle.png");
    iResizeImage(&gamepad, 70, 10);
    iLoadImage(&game_win_img, "assets/images/gameover.jpg");
    iResizeImage(&game_win_img, max_width, max_height);
    iLoadImage(&ball_img, "assets/images/ball.bmp");
    iResizeImage(&ball_img, 10, 10);
    iLoadImage(&gaming_bg, "assets/images/gameing_bg.bmp");
    iResizeImage(&gaming_bg, max_width, max_height);
    iLoadImage(&gameplay1, "assets/images/gameplay1.bmp");
    iResizeImage(&gameplay1, max_width, max_height);
    iLoadImage(&gameplay2, "assets/images/gameplay2.jpg");
    iResizeImage(&gameplay2, max_width, max_height);
    iLoadImage(&gameplay3, "assets/images/gameplay3.jpg");
    iResizeImage(&gameplay3, max_width, max_height);
    iLoadImage(&game_over_img, "assets/images/game_over_img.bmp");
    iResizeImage(&game_over_img, max_width, max_height);
    iLoadImage(&brick_img, "assets/images/bricks.bmp");
    iResizeImage(&brick_img, dx, dy);
    iLoadImage(&heart2, "assets/images/heart2.png");
    iLoadImage(&unbreakable_brick, "assets/images/unbreakable_brick.bmp");
    iResizeImage(&unbreakable_brick, dx, dy);
}

void iDraw()
{

    iClear();

    //  printf("%d %d\n",mx1,m y1);
    if (gameover == 1)
    {
        iShowImage(0, 0, "assets/images/game_over_img.bmp");
        iSetColor(34, 240, 212);
        iTextAdvanced(max_width / 2 - 250, max_height / 2, "GAME OVER!", 0.5, 2.0);
        char finalScore[50];
        sprintf(finalScore, "Final Score: %d", score);
        iTextAdvanced(max_width / 2 - 250, max_height / 2 - 100, finalScore, 0.5, 2.0);
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
        if (!scoreSaved)
        {
            addHighScore(score, playerName);
            scoreSaved = true;
        }
    }
    else if (game_completed)
    {
        win();
    }
    else if (menu == 1)
    {
        iShowImage(0, 0, "assets/images/homepg.jpeg");

        iSetColor(16, 40, 222);

        switch (rectangle)
        {
        case 1:
            iFilledRectangle(520, 360, 180, 35);
            break;
        case 2:
            iFilledRectangle(520, 320, 180, 35);
            break;
        case 3:
            iFilledRectangle(520, 280, 180, 35);
            break;
        case 4:
            iFilledRectangle(525, 240, 180, 35);
            break;
        case 5:
            iFilledRectangle(520, 200, 180, 35);
            break;
        case 6:
            iFilledRectangle(520, 160, 180, 35);
            break;
        case 7:
            iFilledRectangle(520, 120, 180, 35);
            break;
        }
        iSetColor(52, 235, 88);
        iTextBold(555, 370, "NEW GAME", GLUT_BITMAP_HELVETICA_18);
        iTextBold(552, 330, "LOAD GAME", GLUT_BITMAP_HELVETICA_18);
        iTextBold(560, 290, "OPTIONS", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 250, "HIGH SCORES", GLUT_BITMAP_HELVETICA_18);
        iTextBold(580, 210, "HELP", GLUT_BITMAP_HELVETICA_18);
        iTextBold(555, 170, "ABOUT US", GLUT_BITMAP_HELVETICA_18);
        iTextBold(580, 130, "EXIT", GLUT_BITMAP_HELVETICA_18);
    }
    else if (highscores == 1)
    {
        iShowImage(0, 0, "assets/images/background.png");
        showHighScores();
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    else if (about_us_pg)
    {
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(510, 350, "THE GAME was made by:", GLUT_BITMAP_HELVETICA_18);
        iText(510, 300, "1) NAZMUL HASAN RAFI", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 270, "Roll-2405017", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 230, "2) Md Raihan Hossan", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(510, 200, "Roll-2405020", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    else if (help)
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
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    else if (exiting)
    {
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(510, 350, "Do you want to quit this game?", GLUT_BITMAP_HELVETICA_18);
        iTextBold(530, 320, "YES", GLUT_BITMAP_HELVETICA_18);
        iTextBold(590, 320, "NO", GLUT_BITMAP_HELVETICA_18);
    }
    else if (fornewgame == 1)
    {

        iClear();
        if (enteringName)
        {
            iPauseTimer(t);
            invisible_brick = true;
            fireBallActive = false;
            iShowImage(0, 0, "assets/images/gameing_bg.bmp");
            iSetColor(68, 240, 34);
            iText(400, 400, "Enter Your Nick Name:", GLUT_BITMAP_TIMES_ROMAN_24);
            iRectangle(400, 350, 300, 40);
            iText(410, 360, playerName, GLUT_BITMAP_HELVETICA_18);

            return;
        }

        game_completed = 0;
        iSetColor(255, 255, 0);
        switch (level)
        {
        case 1:
            iShowImage(0, 0, "assets/images/gameplay1.bmp");
            break;
        case 2:
            iShowImage(0, 0, "assets/images/gameplay2.jpg");
            break;
        default:
            iShowImage(0, 0, "assets/images/gameplay3.jpg");
        }
        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (balls[i].active)
            {
                if (fireBallActive)
                {
                    iSetColor(255, 0, 0);
                }
                else
                    iSetColor(255, 255, 0);

                iFilledCircle(balls[i].x, balls[i].y, radius);
            }
        }

        iSetColor(255, 255, 0);
        iFilledRectangle(xboard, yboard, dxboard, dyboard);

        loadHighScores();
        creating_bricks();
        breakingbrick();
        iSetColor(34, 240, 212);
        iFilledRectangle(5, 570, 6, 20);
        iFilledRectangle(17, 570, 6, 20);
        iSetColor(0, 0, 0);
        win();
        iSetColor(68, 240, 34);

        char scoreText[50];
        sprintf(scoreText, "Score: %d", score);
        iText(60, max_height - 20, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

        char lifeText[50];
        sprintf(lifeText, "Lives: %d", game_life);
        iText(200, max_height - 20, lifeText, GLUT_BITMAP_TIMES_ROMAN_24);

        char leveltext[50];
        sprintf(leveltext, "Level: %d", level);
        iText(340, max_height - 20, leveltext, GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(255, 0, 0);
        for (int i = 0; i < game_life; i++)
        {
            iShowImage(i * 25 + 1025, max_height - 25, "assets/images/heart.png");
        }
        if (msg1 && msg1_timer_count < 30)
        {

            iSetColor(34, 240, 212);
            // iText(max_width / 2 - 100, max_height - 20, "you got an extra life", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if (msg2 && msg2_timer_count < 30 && msg1 == false && msg3 == false && msg4 == false)
        {

            iSetColor(34, 240, 212);
            iText(max_width / 2 - 100, max_height - 20, "you have lost a life", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if (msg3 && msg3_timer_count < 30 && msg1 == false && msg2 == false && msg4 == false)
        {

            iSetColor(34, 240, 212);
            iText(max_width / 2 - 100, max_height - 20, "The length of the paddle has increased", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if (msg4 && msg4_timer_count < 30 && msg1 == false && msg2 == false && msg3 == false)
        {

            iSetColor(34, 240, 212);
            iText(max_width / 2 - 100, max_height - 20, "The length of the paddle has decreased", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }
    else if (options == 1)
    {
        iClear();
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(68, 240, 34);
        iTextBold(480, 350, "SOUND:", GLUT_BITMAP_HELVETICA_18);
        iTextBold(620, 350, "ON", GLUT_BITMAP_HELVETICA_18);
        iTextBold(660, 350, "OFF", GLUT_BITMAP_HELVETICA_18);
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
    else if (resum_pg)
    {
        iClear();
        iShowImage(0, 0, "assets/images/gameing_bg.bmp");
        iSetColor(16, 40, 222);
        switch (rectangle)
        {

        case 2:
            iFilledRectangle(520, 320, 180, 35);
            break;
        case 3:
            iFilledRectangle(520, 280, 180, 35);
            break;
        case 4:
            iFilledRectangle(525, 240, 180, 35);
            break;
        }

        iSetColor(52, 235, 88);

        iTextBold(550, 330, "RESTART", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 290, "SAVE", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 250, "MAIN MENU", GLUT_BITMAP_HELVETICA_18);
    }
    if (paddle_decrease_activate || paddle_increase_activate)
    {
        if (difftime(time(NULL), paddle_power_start_time) >= paddle_power_duration)
        {
            paddle_increase_activate = false;
            paddle_decrease_activate = false;
            dxboard = 100;
        }
    }
}

void iMouseMove(int mx, int my)
{

    mx1 = mx;
    my1 = my;
    // assets/images/homepg.jpeg assets/images/mouse.png

    if (mx >= 520 && mx < 680 && my >= 360 && my <= 390)
    {
        rectangle = 1;
    }
    else if (mx >= 520 && mx < 700 && my >= 320 && my <= 355)
        rectangle = 2;
    else if (mx >= 520 && mx < 700 && my >= 280 && my <= 315)
        rectangle = 3;
    else if (mx >= 520 && mx < 700 && my >= 240 && my <= 275)
        rectangle = 4;
    else if (mx >= 520 && mx < 700 && my >= 200 && my <= 235)
        rectangle = 5;
    else if (mx >= 520 && mx < 700 && my >= 160 && my <= 195)
        rectangle = 6;
    else if (mx >= 520 && mx < 700 && my >= 120 && my <= 155)
        rectangle = 7;
    else
        rectangle = 0;
}

void iMouseDrag(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 170 && my <= 190))
        {
            about_us_pg = 1;
            menu = 0;
        }
        else if (about_us_pg == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            about_us_pg = 0;
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 200 && my <= 220))
        {
            menu = 0;
            help = 1;
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 230 && my <= 270))
        {
            menu = 0;
            highscores = 1;
        }
        else if (highscores == 1 && (mx >= 20 && mx <= 120) && (my >= 20 && my <= 60))
        {
            menu = 1;
            highscores = 0;
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 320 && my <= 350))
        {
            loadGame();
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 140 && my <= 160))
        {
            menu = 0;
            exiting = 1;
        }
        else if (exiting == 1 && (mx >= 530 && mx <= 550) && (my >= 320 && my <= 340))
        {
            exit(0);
        }
        else if (exiting == 1 && (mx >= 590 && mx <= 620) && (my >= 320 && my <= 340))
        {
            exiting = 0;
            menu = 1;
        }
        else if (help == 1 && (mx >= 20 && mx <= 120) && (my >= 20 && my <= 60))
        {
            menu = 1;
            help = 0;
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 360 && my <= 380))
        {
            menu = 0;
            fornewgame = 1;
            enteringName = 1;
            score = 0;
            game_life = 3;
            initialize = false;
            scoreSaved = false;
            playerName[0] = '\0';
            nameCharIndex = 0;
            // start();
        }
        else if (menu == 1 && (mx >= 520 && mx <= 700) && (my >= 280 && my <= 300))
        {
            menu = 0;
            options = 1;
        }
        else if (options == 1 && (mx >= 660 && mx <= 680) && (my >= 350 && my <= 380))
        {
            iPauseSound(bgSoundIdx);
        }
        else if (options == 1 && (mx >= 620 && mx <= 660) && (my >= 350 && my <= 380))
        {
            iResumeSound(bgSoundIdx);
        }
        else if (options == 1 && (mx >= 20 && mx <= 120) && (my >= 20 && my <= 60))
        {
            menu = 1;
            options = 0;
        }
        else if (gameover == 1 && (mx >= 20 && mx <= 120) && (my >= 20 && my <= 60))
        {
            menu = 1;
            gameover = 0;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            initializing_brick();

            dxboard = 100;
            dyboard = 5;
            xboard = max_width / 2 - dxboard / 2;
            yboard = 50 - radius - dyboard;
            balls[0].x = xboard + dxboard / 2;
            balls[0].y = yboard + radius;
            float angle = (rand() % 90 + 30) * 3.1416 / 180.0;
            balls[0].dx = velocity * cos(angle);
            balls[0].dy = velocity * sin(angle);
            balls[0].active = true;
            if (balls[0].dx == 0 && balls[0].dy == 0)
            {
                balls[0].dx = velocity * cos(PI / 4);
                balls[0].dy = velocity * sin(PI / 4);
            }
            for (int i = 1; i < MAX_BALLS; i++)
                balls[i].active = false;
            level = 1;
            level_initialize(level);
            playerName[0] = '\0'; // Reset name
            nameCharIndex = 0;
            initialize = false;
            scoreSaved = false;
            fireBallActive = false;
            iResumeTimer(t);
        }
        else if (fornewgame == 1 && (mx >= 0 && mx <= 30) && (my >= 570 && my <= 600))
        {

            resum_pg = 1;

            iPauseTimer(t);
            fornewgame = 0;
        }
        else if (resum_pg == 1 && (mx >= 550 && mx <= 730) && (my >= 250 && my <= 280))
        {
            resum_pg = 0;

            fornewgame = 0;
            menu = 1;
            game_completed = 0;
            dxboard = 100;
            dyboard = 5;
            score = 0;
            game_life = 3;
            initializing_brick();
            xboard = max_width / 2 - dxboard / 2;
            initialize = false;
            yboard = 50 - radius - dyboard;
            balls[0].x = xboard + (dxboard / 2);
            balls[0].y = yboard + radius;
            // float angle = (rand() % 90 + 30) * 3.1416 / 180.0;
            balls[0].dx = velocity * cos(rand());
            balls[0].dy = velocity * sin(rand());
            if (balls[0].dx == 0 && balls[0].dy == 0)
            {
                balls[0].dx = velocity * cos(PI / 4);
                balls[0].dy = velocity * sin(PI / 4);
            }
            balls[0].active = true;
            for (int i = 1; i < MAX_BALLS; i++)
                balls[i].active = false;
            initialize = false;
            level = 1;
            level_initialize(level);
            playerName[0] = '\0';
            nameCharIndex = 0;
        }
        else if (resum_pg == 1 && (mx >= 550 && mx <= 730) && (my >= 330 && my <= 370))
        {
            resum_pg = 0;
            // iResumeTimer(t);
            fornewgame = 1;
            menu = 0;

            game_completed = 0;
            fireBallActive = FALSE;
            dxboard = 100;
            dyboard = 5;
            score = 0;
            game_life = 3;

            initializing_brick();
            xboard = max_width / 2 - dxboard / 2;
            initialize = false;
            yboard = 50 - radius - dyboard;
            balls[0].x = xboard + (dxboard / 2);
            balls[0].y = yboard + radius;
            // float angle = (rand() % 90 + 30) * 3.1416 / 180.0;
            balls[0].dx = velocity * cos(rand());
            balls[0].dy = velocity * sin(rand());
            if (balls[0].dx == 0 && balls[0].dy == 0)
            {
                balls[0].dx = velocity * cos(PI / 4);
                balls[0].dy = velocity * sin(PI / 4);
            }
            balls[0].active = true;
            for (int i = 1; i < MAX_BALLS; i++)
                balls[i].active = false;
            initialize = false;

            level = 1;
            level_initialize(level);
            playerName[0] = '\0';
            nameCharIndex = 0;
            initialize = false;
            scoreSaved = false;
            playerName[0] = '\0';
            nameCharIndex = 0;
            msg1 = false;
            msg2 = false;
            msg3 = false;
            msg4 = false;
        }
        else if (resum_pg == 1 && (mx >= 550 && mx <= 730) && (my >= 290 && my <= 330))
        {
            save_game();
        }

        else if (game_completed == 1 && (mx >= 20 && mx <= 120) && (my >= 20 && my <= 60))
        {
            game_completed = 0;
            menu = 1;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            dxboard = 100;
            dyboard = 5;
            xboard = max_width / 2 - dxboard / 2;
            yboard = 50 - radius - dyboard;
            balls[0].x = xboard + (dxboard / 2);
            balls[0].y = yboard + radius;
            // float angle = (rand() % 90 + 30) * 3.1416 / 180.0;
            balls[0].dx = velocity * cos(rand());
            balls[0].dy = velocity * sin(rand());
            balls[0].active = true;
            if (balls[0].dx == 0 && balls[0].dy == 0)
            {
                balls[0].dx = velocity * cos(PI / 4);
                balls[0].dy = velocity * sin(PI / 4);
            }
            for (int i = 1; i < MAX_BALLS; i++)
                balls[i].active = false;
            initialize = false;
            level = 1;
            level_initialize(level);
            playerName[0] = '\0';
            nameCharIndex = 0;
        }
    }
}

void iMouseWheel(int dir, int mx, int my)
{
}

void iKeyboard(unsigned char key)
{
    if (enteringName)
    {
        if (key == '\r')
        {
            enteringName = 0;
            fornewgame = 1;

            score = 0;
            game_life = 3;
            initialize = false;
            start();
        }
        else if (key == '\b')
        {
            if (nameCharIndex > 0)
            {
                nameCharIndex--;
                playerName[nameCharIndex] = '\0';
            }
        }
        else if (nameCharIndex < 29 && key >= 32 && key <= 126)
        {
            playerName[nameCharIndex++] = key;
            playerName[nameCharIndex] = '\0';
        }
    }
    else if (fornewgame == 1 && key == ' ')
    {

        initialize = true;
        iResumeTimer(t);
    }
    else if (fornewgame == 1 && key == 's')
    {
        save_game();
    }
    if (menu == 1 && key == 's')
    {

        menu = 0;
        fornewgame = 1;
        enteringName = 1;
        score = 0;
        game_life = 3;
        initialize = false;
        scoreSaved = false;
        playerName[0] = '\0';
        nameCharIndex = 0;
    }
    if (key == 'p')
    {
        iPauseTimer(t);
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_RIGHT && xboard + dxboard < max_width)
    {
        xboard += 50;
        if (!initialize)
        {
            balls[0].x = xboard + dxboard / 2;
            balls[0].y = yboard + radius + dyboard;
        }
    }
    else if (key == GLUT_KEY_LEFT && xboard > 0)
    {
        xboard -= 50;
        if (!initialize)
        {
            balls[0].x = xboard + dxboard / 2;

            balls[0].y = yboard + radius + dyboard;
        }
    }
}

void level_initialize(int level)
{
    char name[20];
    sprintf(name, "level%d.txt", level);
    FILE *fp = fopen(name, "r");
    if (fp == NULL)
    {
        printf("No level.\n");
        return;
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            fscanf(fp, "%d", &bricks[i][j]);
            brick[i][j] = bricks[i][j];
        }
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (brick[i][j])
            {
                brickRed[i][j] = rand() % 256;

                brickGreen[i][j] = rand() % 256;
                brickBlue[i][j] = rand() % 256;
            }
        }
    }

    fclose(fp);
}
void initBalls()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].active = false;
    }
    balls[0].x = xball;
    balls[0].y = yball;
    balls[0].dx = dxball;
    balls[0].dy = dyball;
    balls[0].active = true;
}
void paddle_increase()
{
    paddle_increase_activate = true;
    dxboard = 150;
    paddle_power_start_time = time(NULL);
}
void paddle_decrease()
{
    paddle_decrease_activate = true;
    dxboard = 75;
    paddle_power_start_time = time(NULL);
}

void start()
{
    level_initialize(level);
    initBalls();

    for (int i = 1; i < MAX_BALLS; i++)
        balls[i].active = false;
    initialize = false;

    scoreSaved = false;
    iResumeTimer(t);
}
void timer()
{
    if (msg1)
    {
        msg1_timer_count++;

        if (msg1_timer_count > 30)
        {
            msg1_timer_count = 0;
            msg1 = false;
        }
    }
    if (msg2)
    {
        msg2_timer_count++;

        if (msg2_timer_count > 30)
        {
            msg2_timer_count = 0;
            msg2 = false;
        }
    }
    if (msg3)
    {
        msg3_timer_count++;

        if (msg3_timer_count > 30)
        {
            msg3_timer_count = 0;
            msg3 = false;
        }
    }
    if (msg4)
    {
        msg4_timer_count++;

        if (msg4_timer_count > 30)
        {
            msg4_timer_count = 0;
            msg4 = false;
        }
    }
}
void ballchange()
{
    if (initialize == false)
        return;

    for (int i = 0; i < MAX_BALLS; i++)
    {
        int k, j, brickX, brickY;
        for (j = 0; j < row; j++)
        {
            for (k = 0; k < column; k++)
            {
                if (brick[j][k])
                {
                    brickX = dx * (k);
                    brickY = 390 + dy * (j);
                }
            }
        }
        if (!balls[i].active)
            continue;

        balls[i].x += balls[i].dx;
        balls[i].y += balls[i].dy;
        if (fireBallActive)
        {
            fireBallTimer -= 10;
            if (fireBallTimer <= 0)
            {
                fireBallActive = false;
            }
        }

        // Wall collision
        if (balls[i].x + radius >= max_width - 2 || balls[i].x - radius <= 2)
            balls[i].dx = -balls[i].dx;
        if (balls[i].y + radius >= max_height)
            balls[i].dy = -balls[i].dy;

        // Paddle collision

        if ((balls[i].x >= xboard && balls[i].x <= xboard + dxboard) &&
            (balls[i].y - radius <= yboard + dyboard) && (balls[i].y - radius >= yboard))
        {

            iPauseTimer(bgSoundIdx);
            paddle_sound = iPlaySound("assets/sounds/paddle_colision.wav", false, 80);
            iPauseTimer(paddle_sound);
            iResumeTimer(bgSoundIdx);
            balls[i].dy = -balls[i].dy;
            int m = balls[i].x;
            balls[i].dx = (float)3 * (m - (xboard + dxboard / 2)) / dxboard * velocity + 5;
        }
        // brick collision...

        if (bricks[i][j] != 12 && bricks[i][j] != 0 && fireBallActive != true)
        {
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY)
            {
                balls[i].dy = -balls[i].dy;
            }
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dy = -balls[i].dy;
            }
            if (balls[i].x + radius >= brickX &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dx = -balls[i].dx;
            }
            if (balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dx = -balls[i].dx;
            }
        }
        else if (bricks[i][j] == 12 && invisible_brick == true && fireBallActive != true)
        {
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY)
            {
                balls[i].dy = -balls[i].dy;
                invisible_brick = false;
            }
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dy = -balls[i].dy;
                invisible_brick = false;
            }
            if (balls[i].x + radius >= brickX &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dx = -balls[i].dx;
                invisible_brick = false;
            }
            if (balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                balls[i].dx = -balls[i].dx;
                invisible_brick = false;
            }
        }
        else if (fireBallActive == true)
        {
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY)
            {
                continue;
            }
            if (balls[i].x + radius >= brickX && balls[i].x - radius <= brickX + dx && balls[i].y - radius <= brickY + dy)
            {
                continue;
            }
            if (balls[i].x + radius >= brickX &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                continue;
            }
            if (balls[i].x - radius <= brickX + dx &&
                balls[i].y + radius >= brickY && balls[i].y - radius <= brickY + dy)
            {
                continue;
            }
        }

        // Missed ball
        if (balls[i].y < 0)
        {

            balls[i].active = false;
            // check if all balls are inactive
            bool allBallsInactive = true;
            for (int j = 0; j < MAX_BALLS; j++)
            {
                if (balls[j].active)
                {
                    allBallsInactive = false;
                    break;
                }
            }

            if (allBallsInactive)
            {
                fireBallActive = false;
                restart();
            }
        }
    }
    decision_of_winning();
    level_increase();
}
void activate_fire_ball()
{
    fireBallActive = true;
    fireBallTimer = FIRE_BALL_DURATION;
}

void initializing_brick()
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            brick[i][j] = true;
            bricks[i][j] = 1; // set as normal brick

            // Assign random RGB color for drawing bricks
            brickRed[i][j] = rand() % 256;
            brickGreen[i][j] = rand() % 256;
            brickBlue[i][j] = rand() % 256;
        }
    }
}
void restart()
{
    game_life--;
    if (game_life > 0)
    {
        invisible_brick = true;
        dxboard = 100;
        dyboard = 5;
        xboard = max_width / 2 - dxboard / 2;
        yboard = 50 - radius - dyboard;
        initialize = false;
        // initializing the new ball's position ...
        balls[0].x = max_width / 2;
        balls[0].y = 50;
        float angle = (rand() % 90 + 30) * 3.1416 / 180.0;
        balls[0].dx = velocity * cos(angle) + 3;
        balls[0].dy = velocity * sin(angle) + 3;

        balls[0].active = true;
        for (int i = 1; i < MAX_BALLS; i++)
            balls[i].active = false;

        // initBalls();
        // velocity = 10;
    }
    if (game_life <= 0)
    {
        invisible_brick = true;
        gameover = 1;
        addHighScore(score, playerName);
        scoreSaved = true;
        iPauseTimer(t);
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
                int x = dx * (j);
                int y = 390 + dy * (i);

                if (bricks[i][j] == 2) // unbreakable
                {
                    iShowImage(x, y, "assets/images/unbreakable_brick.bmp");

                } // grey
                else if (bricks[i][j] == 3) // explosive
                {
                    iShowImage(x, y, "assets/images/bricks.bmp"); // red
                }
                else if (bricks[i][j] == 4) // multiball
                    iSetColor(0, 0, 255);   // blue
                else if (bricks[i][j] == 5) // fireball
                    iSetColor(255, 140, 0); // orange
                else if (bricks[i][j] == 8)
                {
                    iShowImage(x + 10, y, "assets/images/heart2.png");
                }
                else
                {
                    if (bricks[i][j] != 12)
                    {
                        iSetColor(brickRed[i][j], brickGreen[i][j], brickBlue[i][j]); // normal bricks
                    }
                    if (bricks[i][j] == 12 && invisible_brick == false)
                    {
                        iSetColor(brickRed[i][j], brickGreen[i][j], brickBlue[i][j]);
                    }
                    if (bricks[i][j] == 12 && invisible_brick == true)
                    {
                        continue;
                    }
                }

                if (bricks[i][j] != 8 && bricks[i][j] != 3 && bricks[i][j] != 2)
                    iFilledRectangle(x, y, dx, dy);
            }
        }
    }
}

void breakingbrick()
{
    for (int b = 0; b < MAX_BALLS; b++)
    {

        if (!balls[b].active)
            continue;

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                if (brick[i][j])
                {
                    int brickX = dx * (j);
                    int brickY = 390 + dy * i;

                    if (balls[b].x + radius >= brickX && balls[b].x - radius <= brickX + dx &&
                        balls[b].y + radius >= brickY && balls[b].y - radius <= brickY + dy)
                    {

                        // iPauseSound(bgSoundIdx);
                        breakin_sound = PlaySound("assets/sounds/bringbricks.wav", NULL, SND_ASYNC | SND_NOSTOP);
                        int currentBallIndex = b;

                        if (brick[i][j] == 2) // unbreakable brick
                        {
                            balls[b].dy = -balls[b].dy;
                        }
                        if (brick[i][j] == 3) // explosive
                        {

                            for (int a = -1; a <= 1; a++)
                            {
                                for (int c = -1; c <= 1; c++)
                                {
                                    int ni = i + a;
                                    int nj = j + c;
                                    if (ni >= 0 && ni < row && nj >= 0 && nj < column && brick[ni][nj] != 2)
                                    {
                                        if (brick[ni][nj] != false) // only if the brick was not already broken
                                        {
                                            brick[ni][nj] = false;
                                            score += 10;
                                            if (fireBallActive == false)
                                                balls[b].dy = -balls[b].dy;
                                        }
                                    }
                                }
                            }
                        }

                        if (brick[i][j] == 4) // multiball brick ...
                        {
                            activateMultiball(currentBallIndex);
                            brick[i][j] = false;
                            score += 10;
                            if (fireBallActive == false)
                                balls[b].dy = -balls[b].dy;
                        }
                        if (brick[i][j] == 5)
                        {
                            activate_fire_ball();
                            brick[i][j] = 0;
                            score += 10;
                        }
                        if (brick[i][j] == 6) // paddle increase...
                        {
                            msg3 = TRUE;

                            paddle_increase();
                            score += 10;
                            brick[i][j] = 0;

                            if (fireBallActive == false)
                                balls[b].dy = -balls[b].dy;
                        }
                        if (brick[i][j] == 7) // paddle decrease...
                        {

                            msg4 = TRUE;

                            paddle_decrease();
                            brick[i][j] = 0;
                            score += 10;
                            if (fireBallActive == false)
                                balls[b].dy = -balls[b].dy;
                        }
                        if (brick[i][j] == 8) // life increasing...
                        {

                            msg1 = TRUE;

                            game_life++;
                            brick[i][j] = 0;
                            score += 10;
                            if (fireBallActive == false)
                                balls[b].dy = -balls[b].dy;
                        }
                        if (brick[i][j] == 9)       //life decreas---
                        {

                            msg2 = TRUE;
                            if (game_life > 0)
                            {
                                game_life--;
                                brick[i][j] = 0;
                                score += 10;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                            }

                            if (game_life <= 0)
                            {
                                gameover = 1;
                                scoreSaved = false;
                                iPauseTimer(t);
                            }
                            score += 10;
                            brick[i][j] = 0;
                        }

                        if (brick[i][j] == 10)  //speed increase.....
                        {
                            speed_increase = true;
                            if (speed_increase)
                            {
                                velocity = 1.1 * velocity;
                                speed = 1.1 * speed;
                                score += 10;
                                time_for_velocity = time(NULL);
                                brick[i][j] = 0;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                                if (difftime(time(NULL), time_for_velocity) >= time_standered_for_velocity)
                                {
                                    speed_increase = false;
                                }
                            }
                        }
                        if (brick[i][j] == 11)  //speed decrease.....
                        {
                            speed_decrease = true;
                            if (speed_decrease)
                            {
                                velocity = .7 * velocity;
                                speed = .7 * speed;
                                score += 10;
                                brick[i][j] = 0;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                                time_for_velocity = time(NULL);

                                if (difftime(time(NULL), time_for_velocity) >= time_standered_for_velocity)
                                {
                                    speed_decrease = false;
                                }
                            }
                        }
                        if (brick[i][j] == 12)  // invisible----
                        {
                            invisible_brick = false;

                            if (invisible_brick == false)
                            {
                                brick[i][j] = 0;
                                score += 10;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                            }
                        }

                        if (brick[i][j] == 1) // normal brick
                        {
                            if (!fireBallActive)
                            {
                                score += 10;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                                brick[i][j] = 0;
                            }
                            else
                            {
                                score += 10;
                                if (fireBallActive == false)
                                    balls[b].dy = -balls[b].dy;
                                brick[i][j] = 0;
                            }
                        }
                    }

                    iPauseSound(breakin_sound);
                    iResumeSound(bgSoundIdx);
                }
            }
        }
    }
}
void activateMultiball(int mainBallIndex)
{
    int created = 0;

    float angle = atan2(balls[mainBallIndex].dy, balls[mainBallIndex].dx);

    float angle_offset = 60 * 3.1416 / 180.0;

    for (int i = 0; i < 3 && created < 2; i++)
    {
        if (!balls[i].active && i != mainBallIndex)
        {
            balls[i].x = balls[mainBallIndex].x;
            balls[i].y = balls[mainBallIndex].y;

            if (created == 0)
            {
                float new_angle = angle + angle_offset;
                balls[i].dx = velocity * cos(new_angle);
                balls[i].dy = velocity * sin(new_angle);
                if (balls[i].dy == 0)
                    balls[i].dy = -2;
                created++;
            }
            else if (created == 1)
            {
                float new_angle = angle - angle_offset;
                balls[i].dx = velocity * cos(new_angle);
                balls[i].dy = velocity * sin(new_angle);
                if (balls[i].dy == 0)
                    balls[i].dy = -2;
            }

            balls[i].active = true;
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
            if (brick[i][j] > 0 && brick[i][j] != 2)
            {
                win1 = false;
                break;
            }
        }
        if (!win1)
            break;
    }
    if (win1)
        win();
}

void win()
{
    if (game_completed)
    {
        iPauseTimer(t);

        iShowImage(0, 0, "assets/images/gameover.jpg");
        char finalScore[50];
        sprintf(finalScore, "Final Score: %d", score);
        iTextAdvanced(max_width / 2 - 220, max_height / 2 - 100, finalScore, 0.5, 2.0);
        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
}

void level_increase()
{
    if (win1)
    {

        game_life++;

        win1 = false;
        if (level <= MAX_LEVEL)
        {
            level++;

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
void save_game()
{
    FILE *fp = fopen("savegame.txt", "w");
    if (fp == NULL)
    {
        printf("savegame.txt file failed to open ");
        return;
    }
    fprintf(fp, "%d\n%d\n%d\n%d\n", level, score, game_life, xboard);

    int active_ball_count = 0;
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].active)
            active_ball_count++;
    }
    fprintf(fp, "%d\n", active_ball_count);
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].active)
        {
            fprintf(fp, "%d %d %d %d\n", balls[i].x, balls[i].y, balls[i].dx, balls[i].dy);
        }
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            fprintf(fp, "%d ", brick[i][j]);
        }
        fprintf(fp, "\n");
    }

    double paddle_remaining_time = 0;
    if (paddle_increase_activate || paddle_decrease_activate)
    {
        double elapsed = difftime(time(NULL), paddle_power_start_time);
        paddle_remaining_time = paddle_power_duration - elapsed;
        if (paddle_remaining_time < 0)
            paddle_remaining_time = 0;
    }
    if (paddle_increase_activate)
    {
        fprintf(fp, "1\n");
    }
    else
        fprintf(fp, "0\n");
    if (paddle_decrease_activate)
    {
        fprintf(fp, "1\n");
    }
    else
        fprintf(fp, "0\n");

    fprintf(fp, "%lf\n", paddle_remaining_time);

    if (fireBallActive == TRUE)
    {
        fprintf(fp, "1\n");
    }
    else
        fprintf(fp, "0\n");
    fprintf(fp, "%d\n", fireBallTimer);
    if (invisible_brick)
        fprintf(fp, "1\n");
    else
        fprintf(fp, "0\n");
    fprintf(fp, "%s\n", playerName);

    fclose(fp);
}

void loadGame()
{
    FILE *fp = fopen("savegame.txt", "r");
    if (fp == NULL)
    {
        printf("savegame.txt file failed to open ");
        return;
    }
    fscanf(fp, "%d%d%d%d", &level, &score, &game_life, &xboard);
    int active_ball_count;
    fscanf(fp, "%d", &active_ball_count);
    for (int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].active = false;
    }
    for (int i = 0; i < active_ball_count; i++)
    {
        fscanf(fp, "%d %d %d %d", &balls[i].x, &balls[i].y, &balls[i].dx, &balls[i].dy);
        balls[i].active = true;
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            fscanf(fp, "%d", &brick[i][j]);
            if (brick[i][j] != 0)
            {
                brickRed[i][j] = rand() % 256;
                brickGreen[i][j] = rand() % 256;
                brickBlue[i][j] = rand() % 256;
                if (bricks[i][j] == 4)      // multiball
                    iSetColor(0, 0, 255);   // blue
                if (bricks[i][j] == 5)      // fireball
                    iSetColor(255, 140, 0); // orange
            }
        }
    }
    int temp;
    fscanf(fp, "%d", &temp);
    if (temp)
        paddle_increase_activate = TRUE;
    else
        paddle_increase_activate = FALSE;
    fscanf(fp, "%d", &temp);
    if (temp)
        paddle_decrease_activate = TRUE;
    else
        paddle_decrease_activate = FALSE;
    double paddle_remaining_time;
    fscanf(fp, "%lf", &paddle_remaining_time);
    if (paddle_increase_activate || paddle_decrease_activate)
    {
        paddle_power_start_time = time(NULL) - (paddle_power_duration - paddle_remaining_time);
        if (paddle_increase_activate)
            dxboard = 150;
        else if (paddle_decrease_activate)
            dxboard = 75;
        else
            dxboard = 100;
    }
    fscanf(fp, "%d", &temp);
    if (temp == 1)
        fireBallActive = true;
    else
        fireBallActive = false;
    fscanf(fp, "%d", &fireBallTimer);
    fscanf(fp, "%d", &temp);
    if (temp)
        invisible_brick = TRUE;
    else
        invisible_brick = FALSE;
    fscanf(fp, "%s", playerName);
    fclose(fp);
    enteringName = 0;
    fornewgame = 1;
    menu = 0;
    initialize = false;
    iResumeTimer(t);
}

int main(int argc, char *argv[])
{

    srand(time(NULL));
    glutInit(&argc, argv);
    t = iSetTimer(25, ballchange);
    t1 = iSetTimer(29, timer);

    loadResources();

    iInitializeSound();

    bgSoundIdx = iPlaySound("assets/sounds/background.wav", true, 40);

    iInitialize(1200, 600, "DXBALL");

    return 0;
}
