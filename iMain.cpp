#include "iGraphics.h"
#include <iostream>
#include "iSound.h"
#include <ctime>
using namespace std;

int bgSoundIdx = -1; // for background sound
int gameover = 0;    // game over
int about_us_pg = 0;
int menu = 1;
int exiting = 0;
Image bg, mario, aboutus, gamepad, heart_img, game_over_img, ball_img;
int fornewgame = 0;
#define max_width 1200
#define max_height 600 // Fixed typo: max_hight -> max_height
#define row 10
#define column 40
#define PI 3.14159265359

int xball = max_width / 2, yball = 50, radius = 7;
int velocity = 7; // ball speed
/*double initialBallAngle = (PI / 4) + ((double)(rand() % 90) / 180.0 * PI); // Random angle between 45° and 135°
int dxball = velocity * cos(initialBallAngle);
int dyball = velocity * sin(initialBallAngle);*/
int dxball = velocity * cos(rand()), dyball = velocity * sin(rand());

// board initialization
int dxboard = 70;
int dyboard = 10;
int xboard = max_width / 2 - dxboard / 2;
int yboard = 60 - radius - dyboard;
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
int brickRed[row][column];
int brickGreen[row][column];
int brickBlue[row][column];
#define MAX_LEVEL 5
int level = 1;
// functions
char playerName[30] = "";
int enteringName = 1; // 1 = name input mode
int nameCharIndex = 0;
int highscores = 0;
bool scoreSaved = false;

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
            strcpy(highScores[i].name, "AAA");
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
    iSetColor(0,0,0);
    iFilledRectangle(0,0,max_width,max_height);

    iSetColor(68, 240, 34);
    iText(450, 500, " Top 10 High Scores", GLUT_BITMAP_HELVETICA_18); // Adjusted y-coordinate

    for (int i = 0; i < highScoreCount; i++)
    {
        char display[100];
        sprintf(display, "%d. %s - %d", i + 1, highScores[i].name, highScores[i].score);
        iText(450, 450 - i * 40, display, GLUT_BITMAP_HELVETICA_18); // Adjusted y-coordinate
    }
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
    iResizeImage(&heart_img, 20, 19);
    iLoadImage(&gamepad, "assets/images/game_paddle.png");
    iResizeImage(&gamepad, 70, 10);
    iLoadImage(&game_over_img, "assets/images/gameover.jpg");
    iResizeImage(&game_over_img, max_width, max_height);
    iLoadImage(&ball_img, "assets/images/ball.bmp");
    iResizeImage(&ball_img, 14, 14);
}

void iDraw()
{
    iClear();
    if (gameover == 1)
    {
        iShowImage(0, 0, "assets/images/background.png");
        iSetColor(255, 0, 0);
        iTextAdvanced(max_width / 2 - 200, max_height / 2, "GAME OVER!", 0.5, 2.0);
        char finalScore[50];
        sprintf(finalScore, "Final Score: %d", score);
        iTextAdvanced(max_width / 2 - 200, max_height / 2 - 100, finalScore, 0.5, 2.0);
        iSetColor(68, 240, 34);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
        if (!scoreSaved)
        {
            addHighScore(score, playerName);
            scoreSaved = true;
        }
    }
    else if (game_completed) // Then check win condition
    {
        win();
    }
    else if (menu == 1)
    {
        iShowImage(0, 0, "assets/images/homepg.jpeg");
        iSetColor(52, 235, 88);
        iTextBold(550, 360, "NEW GAME", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 330, "LOAD GAME", GLUT_BITMAP_HELVETICA_18); // Removed unimplemented option
        iTextBold(550, 300, "OPTIONS", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 270, "HIGH SCORES", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 240, "HELP", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 210, "ABOUT US", GLUT_BITMAP_HELVETICA_18);
        iTextBold(550, 180, "EXIT", GLUT_BITMAP_HELVETICA_18);
    }
    else if (highscores == 1)
    {
        iShowImage(0, 0, "assets/images/background.png");
        showHighScores();
        iSetColor(68, 240, 34);
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
            iSetColor(255, 255, 255);
            iFilledRectangle(0, 0, max_width, max_height);
            iSetColor(68, 240, 34);
            iText(400, 400, "Enter Your Name:", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(400, 360, playerName, GLUT_BITMAP_HELVETICA_18);
            return;
        }

        game_completed = 0;
        iSetColor(255, 255, 0);
        // iShowImage(0, 0, "assets/images/gamebg.jpg");

        iShowImage(xball - radius, yball - radius, "assets/images/ball.bmp");

        iShowImage(xboard, yboard, "assets/images/game_paddle.png");
        loadHighScores();
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
        iText(60, max_height - 20, scoreText, GLUT_BITMAP_HELVETICA_18);

        char lifeText[50];
        sprintf(lifeText, "Lives: %d", game_life);
        iText(200, max_height - 20, lifeText, GLUT_BITMAP_HELVETICA_18);

        char leveltext[50];
        sprintf(leveltext, "Level: %d", level);
        iText(340, max_height - 20, leveltext, GLUT_BITMAP_HELVETICA_18);

        iSetColor(255, 0, 0);
        for (int i = 0; i < game_life; i++)
        {
            // iFilledCircle(i * 18 + 1025, max_height - 25, 7); // Fixed position
            iShowImage(i * 25 + 1025, max_height - 25, "assets/images/heart.png");
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

        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
}

void iMouseMove(int mx, int my)
{
}

void iMouseDrag(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 210 && my <= 225))
        {
            about_us_pg = 1;
            menu = 0;
        }
        else if (about_us_pg == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            about_us_pg = 0;
        }
        else if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 240 && my <= 260))
        {
            menu = 0;
            help = 1;
        }
        else if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 270 && my <= 290))
        {
            menu = 0;
            highscores = 1;
        }
        else if (highscores == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            highscores = 0;
        }
        else if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 180 && my <= 200))
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
        else if (help == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            help = 0;
        }
        else if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 360 && my <= 380))
        {
            menu = 0;
            fornewgame = 1;
            score = 0;
            game_life = 3;
            initialize = false;
            scoreSaved = false;
            start();
        }
        else if (menu == 1 && (mx >= 550 && mx <= 590) && (my >= 300 && my <= 320))
        {
            menu = 0;
            options = 1;
        }
        else if (options == 1 && (mx >= 660 && mx <= 680) && (my >= 350 && my <= 380))
        {
            iPauseSound(bgSoundIdx);
        }
        else if (options == 1 && (mx >= 620 && mx <= 640) && (my >= 350 && my <= 380))
        {
            iResumeSound(bgSoundIdx);
        }
        else if (options == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            options = 0;
        }
        else if (gameover == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            menu = 1;
            gameover = 0;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            xball = max_width / 2;
            yball = 50;
            dxball = velocity * cos(rand());
            dyball = velocity * sin(rand());

            initialize = false;
            xboard = max_width / 2 - dxboard / 2;
            yboard = 60 - radius - dyboard;
            level = 1;
            level_initialize(level);
        }
        else if (fornewgame == 1 && (mx >= 0 && mx <= 20) && (my >= 570 && my <= 600))
        {
            menu = 1;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            xball = max_width / 2;
            yball = 50;
            dxball = velocity * cos(rand());
            dyball = velocity * sin(rand());
            initialize = false;
            xboard = max_width / 2 - dxboard / 2;
            yboard = 60 - radius - dyboard;
            level = 1;
            level_initialize(level);
        }
        else if (game_completed == 1 && (mx >= 20 && mx <= 80) && (my >= 20 && my <= 60))
        {
            game_completed = 0;
            menu = 1;
            fornewgame = 0;
            score = 0;
            game_life = 3;
            xball = max_width / 2;
            yball = 50;
            dxball = velocity * cos(rand());
            dyball = velocity * sin(rand());

            initialize = false;
            xboard = max_width / 2 - dxboard / 2;
            level = 1;
            level_initialize(level);
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
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_RIGHT && xboard + dxboard < max_width)
    {
        xboard += 50; // Smoother movement
        if (!initialize)
            xball += 50;
    }
    else if (key == GLUT_KEY_LEFT && xboard > 0)
    {
        xboard -= 50;
        if (!initialize)
            xball -= 50;
    }
}

void level_initialize(int level)
{
    char name[20];
    sprintf(name, "level%d.txt", level);
    FILE *f = fopen(name, "r");
    if (f == NULL)
    {
        printf("Level %d file not found. Using default layout.\n", level);
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                brick[i][j] = (i < row / 2 && rand() % 2);
                if (brick[i][j])
                {
                    brickRed[i][j] = rand() % 255;
                    brickGreen[i][j] = rand() % 255;
                    brickBlue[i][j] = rand() % 255;
                }
            }
        }
        return;
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            int value;
            if (fscanf(f, "%d", &value) != 1)
            {
                brick[i][j] = false;
            }
            else
            {
                brick[i][j] = (value != 0);
            }
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
    xball = max_width / 2;
    yball = 50;
    dxboard = 70;
    dyboard = 10;
    xboard = max_width / 2 - dxboard / 2;
    yboard = 60 - radius - dyboard;
    initialize = false;
    dxball = velocity * cos(rand());
    dyball = velocity * sin(rand());

    scoreSaved = false;
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
        if (yball + radius >= max_height)
        {
            dyball = -dyball;
        }
        if (xball > xboard && xball < xboard + dxboard && yball - radius <= yboard + dyboard)
        {
            dyball = -dyball;
        }
        /*if (yball - radius <= yboard + dyboard &&
            yball + radius >= yboard &&
            xball + radius >= xboard &&
            xball - radius <= xboard + dxboard)
        {
            dyball = -abs(dyball);
        }*/

        if (yball - radius < 0)
        {
            restart();
        }
        decision_of_winning();
        level_increase();
    }
}

void restart()
{
    game_life--;
    if (game_life > 0)
    {
        xball = max_width / 2;
        yball = 50;
        dxboard = 70;
        dyboard = 10;
        xboard = max_width / 2 - dxboard / 2;
        yboard = 60 - radius - dyboard;
        initialize = false;
        dxball = velocity * cos(rand());
        dyball = velocity * sin(rand());
        scoreSaved = false;
    }
    else
    {
        gameover = 1;
        scoreSaved = false;
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
                int x = dx * (j + 1);
                int y = max_height - (row - i) * dy - 100;
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
                if (xball + radius >= dx * (j + 1) && xball - radius <= dx * (j + 1) + dx && yball + radius >= max_height - (row - i)  + dy * i && yball - radius <= max_height - (row - i)  + dy * i + dy)
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
            if (brick[i][j])
            {
                win1 = false;
                break;
            }
        }
        if (!win1)
            break;
    }
}

void win()
{
    if (game_completed)
    {
        iPauseTimer(t); // Fixed timer index
        iShowImage(0, 0, "assets/images/gameover.jpg");

        iSetColor(34, 240, 212);
        iRectangle(20, 20, 60, 40);
        iTextBold(30, 30, "Back", GLUT_BITMAP_HELVETICA_18);
    }
}

void level_increase()
{
    if (win1)
    {
        level++;
        game_life = 3;
        if (level <= MAX_LEVEL)
        {
            level_initialize(level);
            xball = max_width / 2;
            yball = 50;
            dxboard=70;
            dxboard=10;
            
            dxball = velocity * cos(rand());
            dyball = velocity * sin(rand());
            initialize = false;
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
    srand(time(NULL));
    glutInit(&argc, argv);
    t = iSetTimer(20, ballchange);
    loadResources();
    iInitializeSound();
    bgSoundIdx = iPlaySound("assets/sounds/background.wav", true, 100);
    iInitialize(1200, 600, "DXBALL");
    return 0;
}
