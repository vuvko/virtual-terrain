#include <iostream>
#include <cstdlib>
#include <GL/glut.h>

#include "Terrain.h"
#include "Random.h"

using namespace std;

void Display();
void Reshape(GLint w, GLint h);
void Keyboard(unsigned char key, int x, int y);
void init_light();

GLint Width = 512, Height = 512;

const int CubeSize = 200;

enum {N = 3};
Terrain terrain_matrix(N);

int
main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Virtual Terrain");

    init_light();

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    glutMainLoop();

    return 0;
}

void
init_light()
{
    GLfloat light_ambient[] = {0, 0, 0, 1};
    GLfloat light_diffuse[] = {1, 1, 1, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};
    GLfloat light_position[] = {1, 1, 1, 0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void
Display(void)
{
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFlush();
}

void
Reshape(GLint w, GLint h)
{
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (GLfloat)w / h, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 4,
              0, 0, 0,
              0, 1, 0);
}

void
Keyboard(unsigned char key, int x, int y)
{
    const char ESCAPE = '\033';

    if (key == ESCAPE)
    {
        exit(0);
    }
}
