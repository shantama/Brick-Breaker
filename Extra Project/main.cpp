#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include<math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 70
#define BRICK_HEIGHT 20
#define MAX_LIVES 3

typedef struct {
    float x, y, width, height;
} Paddle;

typedef struct {
    float x, y, dx, dy, radius;
} Ball;

typedef struct {
    float x, y;
    int strength; // how many hits it can take
    bool alive;
} Brick;

Paddle paddle;
Ball ball;
Brick bricks[BRICK_ROWS][BRICK_COLS];
int lives = MAX_LIVES;
bool gameOver = false;

void initBricks() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            bricks[i][j].x = j * (BRICK_WIDTH + 10) + 35;
            bricks[i][j].y = WINDOW_HEIGHT - (i + 1) * (BRICK_HEIGHT + 10);
            bricks[i][j].alive = true;
            bricks[i][j].strength = (i % 3) + 1; // Some take multiple hits
        }
    }
}

void resetGame() {
    paddle.x = WINDOW_WIDTH / 2 - 50;
    paddle.y = 30;
    paddle.width = 100;
    paddle.height = 15;

    ball.x = paddle.x + paddle.width / 2;
    ball.y = paddle.y + 20;
    ball.dx = 4;
    ball.dy = 4;
    ball.radius = 8;

    lives = MAX_LIVES;
    gameOver = false;
    initBricks();
}

void drawPaddle() {
    glColor3f(0.2f, 0.7f, 1.0f);
    glRectf(paddle.x, paddle.y, paddle.x + paddle.width, paddle.y + paddle.height);
}

void drawBall() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 20; i++) {
        float angle = 2.0f * 3.1416f * i / 20;
        glVertex2f(ball.x + cos(angle) * ball.radius, ball.y + sin(angle) * ball.radius);
    }
    glEnd();
}

void drawBricks() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            if (bricks[i][j].alive) {
                if (bricks[i][j].strength == 3)
                    glColor3f(1.0f, 0.2f, 0.2f);
                else if (bricks[i][j].strength == 2)
                    glColor3f(1.0f, 0.5f, 0.0f);
                else
                    glColor3f(0.2f, 1.0f, 0.2f);

                glRectf(bricks[i][j].x, bricks[i][j].y,
                        bricks[i][j].x + BRICK_WIDTH, bricks[i][j].y + BRICK_HEIGHT);
            }
        }
    }
}

void drawLives() {
    char buffer[50];
    sprintf(buffer, "Lives: %d", lives);
    glColor3f(1, 1, 1);
    glRasterPos2f(10, 10);
    for (char* c = buffer; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
}

void drawGameOver() {
    if (gameOver) {
        char* msg = "Game Over! Press R to Restart.";
        glColor3f(1, 1, 0);
        glRasterPos2f(WINDOW_WIDTH / 2 - 130, WINDOW_HEIGHT / 2);
        for (char* c = msg; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPaddle();
    drawBall();
    drawBricks();
    drawLives();
    drawGameOver();
    glutSwapBuffers();
}

void updateBall() {
    if (gameOver)
        return;

    ball.x += ball.dx;
    ball.y += ball.dy;

    if (ball.x < ball.radius || ball.x > WINDOW_WIDTH - ball.radius)
        ball.dx *= -1;
    if (ball.y > WINDOW_HEIGHT - ball.radius)
        ball.dy *= -1;

    // Paddle collision
    if (ball.y - ball.radius < paddle.y + paddle.height &&
        ball.x > paddle.x && ball.x < paddle.x + paddle.width &&
        ball.y > paddle.y) {
        ball.dy *= -1;
    }

    // Brick collision
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            Brick* b = &bricks[i][j];
            if (b->alive) {
                if (ball.x > b->x && ball.x < b->x + BRICK_WIDTH &&
                    ball.y + ball.radius > b->y && ball.y - ball.radius < b->y + BRICK_HEIGHT) {
                    b->strength--;
                    if (b->strength <= 0)
                        b->alive = false;
                    ball.dy *= -1;
                }
            }
        }
    }

    // Missed paddle
    if (ball.y < 0) {
        lives--;
        if (lives <= 0) {
            gameOver = true;
        } else {
            // Reset ball position
            ball.x = paddle.x + paddle.width / 2;
            ball.y = paddle.y + 20;
            ball.dx = 4;
            ball.dy = 4;
        }
    }
}

void timer(int v) {
    updateBall();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'a' && paddle.x > 0)
        paddle.x -= 20;
    if (key == 'd' && paddle.x + paddle.width < WINDOW_WIDTH)
        paddle.x += 20;
    if (key == 'r' || key == 'R')
        resetGame();
    glutPostRedisplay();
}

void init() {
    glClearColor(0, 0, 0.15, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    resetGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Brick Breaker - CG Final Project");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}


