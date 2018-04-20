/**
 * Autor: Marius Micluta-Campeanu
 * Grupa: 252
 */

#include <GL/freeglut.h>
#include <mmsystem.h>
#include <cstdio>
#include <cmath>
#include <GL/SOIL.h>
//#include <windows.h>

int const N = 28, gN = 5;
int offset, offsetStep, difference, myWindow;
double const PI = 3.1415926535, PII = PI/180;
double alpha[N+1], gears[gN + 1], initialAlpha, scaleFactor, epsilon;
double xHigh, yHigh, xLow, yLow, yMiddle, xDotInit, yDotInit, lowDifX, highDifY;
bool pauseSwitch, sound, hit[N+1];

GLuint texture1, texture2;

typedef struct Rectangle {
    double coordinates[4], colour[4], xDot, yDot;

    double &operator()(int i) {return coordinates[i];}
} MyRectangle;
MyRectangle rectangles[N+1];

void drawRectangle(MyRectangle &rectangle, double yMiddle, int i);
void drawDots(double xDot, double yDot, int i, int step);
void drawOneDot(double xDot, double yDot, double x, double y, double z = 0);
void move();
void mouse(int, int, int, int);
double turn(double, double, double, double, double, double);
void reset();
void initGears();

void loadTexture1();
void loadTexture2();

void initialise(int diff = 280, double scale = 6) {
    glClearColor(1, 1, 1, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glOrtho(-1280, 1280, -720, 720, -1, 1);         // set the view 1:1 with the window

    initGears();       // experimental feature, may not work correctly
    offset = 0;             // the first piece has no offset
    difference = diff;      // set admissible error according to the distance between pieces
    if(difference < 150) {
        epsilon = 3.5e4;
    }
    else if(difference < 200) {
        epsilon = 2.3e4;
    }
    else if(difference < 250) {
        epsilon = 1.7e4;
    }
    else if(difference < 300) {
        epsilon = 1e4;
    }
    else {
        epsilon = 5e3;
    }
    scaleFactor = 1/scale;
    lowDifX = highDifY = 60;    // difference from top for the dot
    xLow = yLow = 0;            // left & bottom
    xHigh = 250;                // right
    yHigh = 500;                // top
    yMiddle = (yLow + yHigh)/2;
    xDotInit = xLow + lowDifX;      // top top left
    yDotInit = yHigh - highDifY;
    offsetStep = (int) (xHigh + difference);
    sound = false;

    glEnable(GL_BLEND);
    loadTexture1();
    loadTexture2();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initGears() { gears[0] = 0.5, gears[1] = 1, gears[2] = 1.5; }

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();

    offset = 0;
    glPointSize((GLfloat) (44 * scaleFactor));
    glEnable(GL_POINT_SMOOTH);      // round dots
    glLineWidth((GLfloat) (34 * scaleFactor * scaleFactor));
    glColor3d(0, 0, 0);
    glBegin(GL_LINES);     // reference line, known as The Floor
        glVertex2d(-12800, yLow);
        glVertex2d(12800, yLow);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glColor4d(1, 1, 1, 0.3);
    glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0); glVertex2d(-2000, -2000);
        glTexCoord2d(0.0, 15.0); glVertex2d(-2000, 2000);
        glTexCoord2d(15.0, 15.0); glVertex2d(2000, 2000);
        glTexCoord2d(15.0, 0.0); glVertex2d(2000, -2000);
    glEnd();

    glPushMatrix();
    glTranslated(-1250, 0, 0);      // put pieces to the left
    glScaled(scaleFactor, scaleFactor, 1);
    for(int i = 0; i < N; ++i) {

        glPushMatrix();             // save state before moving current piece
        rectangles[i](0) = xLow + offset;
        rectangles[i](1) = yLow;
        rectangles[i](2) = xHigh + offset;
        rectangles[i](3) = yHigh;
        rectangles[i].xDot = xDotInit + offset;
        rectangles[i].yDot = yDotInit;
        drawRectangle(rectangles[i], yMiddle, i);

        glPopMatrix();      // get the previous state back
        offset += offsetStep;   // offset for the next piece
    }
    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
    glFlush();
}

void drawRectangle(MyRectangle &rectangle, double yMiddle, int i) {
    double xDot, yDot;
    xDot = rectangle.xDot;
    yDot = rectangle.yDot;

    glTranslated(rectangle(2), 0, 0);
    glRotated(-alpha[i], 0, 0, 1);      // push piece
    glTranslated(-rectangle(2), 0, 0);

    glBindTexture(GL_TEXTURE_2D, texture1);
    glColor4d(0.7, 0.7, 0.7, 0.4);
    glPolygonMode(GL_FRONT, GL_FILL);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2d(rectangle(0), rectangle(1));
    glTexCoord2d(0, 1);glVertex2d(rectangle(2), rectangle(1));
    glTexCoord2d(1, 1);glVertex2d(rectangle(2), rectangle(3));
    glTexCoord2d(1, 0);glVertex2d(rectangle(0), rectangle(3));
    glEnd();

    //glColor3d(i*1./N, (N)*1./(N+i), (N-i)*1./N);
    rectangle.colour[0] = i * 1./N;     // set some random deterministic colour
    rectangle.colour[1] = N * 1./(N+i);
    rectangle.colour[2] = (N - i) * 1./N;
    rectangle.colour[3] = 1;
    glColor4dv(rectangle.colour);
    glPolygonMode(GL_FRONT, GL_LINE);
    glRectd(rectangle(0), rectangle(1), rectangle(2), rectangle(3));    // the piece

    glBegin(GL_LINES);      // middle line of the piece
        glVertex2d(rectangle(0), yMiddle);
        glVertex2d(rectangle(2), yMiddle);
    glEnd();
    glBegin(GL_POINTS);     // the dot top top left
        glVertex2d(xDot, yDot);
    glEnd();

    drawDots(xDot, yDot, i, 0);

    if(i%12 >= 6) {     // >= 7 dots
        drawOneDot(xDot, yDot, 0, -195);     // the dot bottom top left
    }

    drawDots(xDot, yDot, i, 6);
}

void drawOneDot(double xDot, double yDot, double x, double y, double z) {
    glTranslated(x, y, z);
    glBegin(GL_POINTS);
        glVertex2d(xDot, yDot);
    glEnd();
}

void drawDots(double xDot, double yDot, int i, int step) {
    if(i % 12 >= 1 + step) {     // >=2 + step dots
        drawOneDot(xDot, yDot, 120, -120);     // the dot top/bottom bottom right
    }
    if(i%12 == 2 + step) {     // 3 + step dots
        drawOneDot(xDot, yDot, -60, 60);     // the dot top/bottom middle
    }
    if(i%12 >= 3 + step) {     // >=4 + step dots
        drawOneDot(xDot, yDot, -120, 0);     // the dot top/bottom bottom left
        drawOneDot(xDot, yDot, 120, 120);     // the dot top/bottom top right
    }
    if(i%12 == 4 + step) {     // 5 + step dots
        drawOneDot(xDot, yDot, -60, -60);     // the dot top/bottom middle
    }
    if(i%12 >= 5 + step) {     // >= 6 + step dots
        drawOneDot(xDot, yDot, 0, -60);     // the dot top/bottom middle left
        drawOneDot(xDot, yDot, -120, 0);     // the dot top/bottom middle left
    }
}

void move() {
    bool motion = false;
    if(not hit[1]) {
        initialAlpha = alpha[0];        // save the angle to know when to stop when a piece hits the next one
    }
    double xH1, yH1, xL2, xH2, yH2;
    xH1 = rectangles[0](2);
    yH1 = rectangles[0](3);
    xL2 = rectangles[1](0);
    xH2 = rectangles[1](2);
    yH2 = rectangles[1](3);

    double xH11, xL22;
    xH11 = xH1 + yH1 * sin(alpha[0] * PII);             // the top right corner of the first piece
    xL22 = xL2 + (xH2 - xL2) * (1 - cos(alpha[1] * PII));
    if(xH1 + yH1 * sin(alpha[0] * PII) < xL2) {         // the first piece is still behind the second piece
        alpha[0] += gears[2];
        motion = true;
    }
    else if(alpha[0] < 90 - gears[1] and (turn(xL22, (xH2 - xL2) * sin(alpha[1] * PII),
                 xL2 + yH2 * sin(alpha[1] * PII), yH2 * cos(alpha[1] * PII),
                 xH11, yH1 * cos(alpha[0] * PII)) > epsilon or xH11 < xL22)) {
        alpha[0] += gears[1];                           // the second piece is hit, so the first piece
        motion = true;                                  // shouldn't pass through it
    }

    for(int i = 1; i < N; ++i) {
        xH1 = rectangles[i](2);
        yH1 = rectangles[i](3);
        xL2 = rectangles[i + 1](0);
        xH2 = rectangles[i + 1](2);
        yH2 = rectangles[i + 1](3);
        xH11 = xH1 + yH1 * sin(alpha[i] * PII);                         // top right corner of the i piece
        xL22 = xL2 + (xH2 - xL2) * (1 - cos(alpha[i + 1] * PII));       // bottom left corner of the (i + 1) piece

        if(alpha[i-1] >= initialAlpha and not hit[i + 1] and
           (rectangles[i - 1](2) + rectangles[i - 1](3) * sin(alpha[i - 1] * PII)) > rectangles[i](0) ) {
            alpha[i] += gears[2];       // the i piece didn't hit the next piece yet, so it moves fast
            if(sound && !hit[i]) {
                PlaySound("Project1\\boing.wav", NULL, SND_ASYNC | SND_FILENAME);
            }
            hit[i] = true;
            motion = true;
        }
        else if(alpha[i] > initialAlpha and alpha[i] < 90 - gears[0] and
                (turn(xL22, (xH2 - xL2) * sin(alpha[i + 1] * PII),
                      xL2 + yH2 * sin(alpha[i + 1] * PII), yH2 * cos(alpha[i + 1] * PII),
                      xH11, yH1 * cos(alpha[i] * PII)) > epsilon or xH11 + 24*scaleFactor*scaleFactor < xL22) ) {
            alpha[i] += gears[1];       // the next piece has been hit, so the piece should move slower
            motion = true;
        }
    }

    if(hit[N - 1] and alpha[N - 1] > 89.5) {
        hit[N] = true;      // the last piece looks for the next piece, so we use an imaginary one
    }
    if(not motion) {           // if nothing happened, we should stop drawing
        glutIdleFunc(NULL);
        if(sound) {
            PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
        }
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int, int) {
    if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            glutIdleFunc(move);         // start the party!
            //glutIdleFunc(simpleMove);
            pauseSwitch = true;
        } else if(button == GLUT_RIGHT_BUTTON) {
            draw();
            reset();
        } else if(button == GLUT_MIDDLE_BUTTON) {
            if(sound) {
                PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            }
            glutIdleFunc(NULL);         // pause
            glutPostRedisplay();
            pauseSwitch = false;
        }
    }
}

void reset() {
    if(sound) {
        PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
    }
    for(int i = 0; i <= N; ++i) {
        alpha[i] = 0;           // reset
        hit[i] = false;
    }
    initGears();
    pauseSwitch = false;
    glutIdleFunc(NULL);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch(key) {
        case ' ':
            if(pauseSwitch) {
                glutIdleFunc(NULL);
                if(sound) {
                    PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                }
                glutPostRedisplay();
                pauseSwitch = false;
            }
            else {
                /*if(sound) {
                    PlaySound("Project1\\boing.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                }*/
                glutIdleFunc(move);         // start the party!
                pauseSwitch = true;
            }
            break;
        case 'e':
            for(int i = 0; i < 3; ++i) {
                ++gears[i];
            }
            break;
        case 'q':
            for(int i = 0; i < 3; ++i) {
                --gears[i];
            }
            break;
        case 's':
            glScaled(0.9, 0.9, 1);                      // zoom out the scene
            scaleFactor *= 0.9;
            glPointSize((GLfloat) (2*scaleFactor));       // zoom out the dots
            glLineWidth((GLfloat) (34 * scaleFactor * scaleFactor));
            glutPostRedisplay();                        // zoom out the lines and redraw
            break;
        case 'w':
            glScaled(1.1, 1.1, 1);                      // zoom in the scene
            scaleFactor *= 1.1;
            glPointSize((GLfloat) (2*scaleFactor));       // zoom in the dots
            glLineWidth((GLfloat) (34 * scaleFactor * scaleFactor));
            glutPostRedisplay();                        // zoom in the lines and redraw
            break;
        case 'a':
            glTranslated(50. / scaleFactor, 0, 0);         // go left (translate the scene to the right)
            glutPostRedisplay();
            break;
        case 'd':
            glTranslated(-50. / scaleFactor, 0, 0);        // go right (translate the scene to the left)
            glutPostRedisplay();
            break;
        case 'l':
            if(sound) {
                PlaySound(NULL, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            }
            printf("Introduceti distanta dintre piese: ");
            int diff;
            scanf("%d", &diff);
            reset();                // reset everything so the old values don't mix with the new ones
            glLoadIdentity();       // just in case... reset the matrix
            initialise(diff);       // send the new distance read from console
            draw();
            glutPostRedisplay();
            break;
        case 'z':
            sound = not sound;
            break;
        case 'x':
            glutDestroyWindow(myWindow);
            exit(0);
            //break;
        default:
            break;
    }
}

double turn(double x1, double y1, double x2, double y2, double x3, double y3) {     // returns > 0 if (x3, y3) is to
    return x3 * y1 + x1 * y2 + y3 * x2 - y1 * x2 - y3 * x1 - x3 * y2;   // the left of the (x1, y1), (x2, y2) line
}

void loadTexture1() {
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Set texture1 wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width1, height1;
    unsigned char *image1 = SOIL_load_image("Project1/tx2.png", &width1, &height1, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    SOIL_free_image_data(image1);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

void loadTexture2() {
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Set texture1 wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width2, height2;
    unsigned char *image2 = SOIL_load_image("Project1/tx1.png", &width2, &height2, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    SOIL_free_image_data(image2);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1280, 720);
    myWindow = glutCreateWindow("Domino");

    if(argc > 2) {
        initialise(strtol(argv[1], NULL, 10), strtod(argv[2], NULL));
    }
    else if(argc > 1) {
        initialise(strtol(argv[1], NULL, 10));
    }
    else {
        initialise();
    }
    //initialise(argc > 1 ? strtol(argv[1], NULL, 10) : 280, argc > 2 ? strtod(argv[2], NULL) : 6);
    glClear(GL_COLOR_BUFFER_BIT);
    glutDisplayFunc(draw);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
