#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/gl.h>

#include "Terrain.h"
#include "Random.h"

using namespace std;

void Display();
void Reshape(GLint w, GLint h);
void Keyboard(unsigned char key, int x, int y);
void Navi(int key, int x, int y);
void init_light();

GLint Width = 512, Height = 512;

double x_pos = 0;
double y_pos = 20;
double z_pos = 20;

const int CubeSize = 200;

enum {N = 3};
Terrain terrain_matrix(N);

GLfloat *
get_normal(float x, float y, int type)
{
    GLfloat n[3];
    float a[3], b[3], c[3];
    if (type == 1) {
        a[0] = x;
        a[1] = y;
        a[2] = terrain_matrix.at(x,  y);
        b[0] = x + 1;
        b[1] = y;
        b[2] = terrain_matrix.at(x + 1, y);
        c[0] = x + 1;
        c[1] = y + 1;
        c[2] = terrain_matrix.at(x + 1, y + 1);
    } else {
        a[0] = x;
        a[1] = y;
        a[2] = terrain_matrix.at(x,  y);
        b[0] = x;
        b[1] = y + 1;
        b[2] = terrain_matrix.at(x, y + 1);
        c[0] = x + 1;
        c[1] = y + 1;
        c[2] = terrain_matrix.at(x + 1, y + 1);
    }

    n[0] = (b[1] - a[1]) * (c[2] - a[2]) - (b[2] - a[2]) * (c[1] - a[1]);
    n[1] = (c[0] - a[0]) * (b[2] - a[2]) - (b[0] - a[0]) * (c[2] - a[2]);
    n[2] = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]);
    float mult = 0;
    for (int i = 0; i < 3; ++i)
        mult += a[i] * n[i];
    if (mult < 0)
        for (int i = 0; i < 3; ++i)
            n[i] = -n[i];
    return n;
}

int
main(int argc, char *argv[])
{
    terrain_matrix.print();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Virtual Terrain");

    init_light();

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Navi);

    glutMainLoop();

    return 0;
}

void
init_light()
{
    GLfloat light_ambient[] = {0, 0, 0, 1};
    GLfloat light_diffuse[] = {1, 1, 1, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};
    GLfloat light_position[] = {100, 100, 100, 0};
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


    float mat_dif[] = {0.9, 0.9, 0};
    float mat_amb[] = {0.2, 0.2, 0.2};
    float mat_spec[] = {0.6, 0.6, 0.6};
    float mat_shi = 0.5 * 128;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shi);

    //glRotatef(270, 1, 0, 0);

    glBegin(GL_TRIANGLES);
    for (int x = 0; x < terrain_matrix.get_size() - 1; ++x) {
        for (int y = 0; y < terrain_matrix.get_size() - 1; ++y) {
            glNormal3fv(get_normal(x, y, 1));
            glVertex3f(x, y, terrain_matrix.at(x, y));
            glVertex3f(x + 1, y, terrain_matrix.at(x + 1, y));
            glVertex3f(x + 1, y + 1, terrain_matrix.at(x + 1, y + 1));
            glNormal3fv(get_normal(x, y, 2));
            glVertex3f(x, y, terrain_matrix.at(x, y));
            glVertex3f(x, y + 1, terrain_matrix.at(x, y + 1));
            glVertex3f(x + 1, y + 1, terrain_matrix.at(x + 1, y + 1));

        }
    }
    glEnd();

    glutSolidTorus(0.275, 0.85, 15, 15);

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
    gluPerspective(40, (GLfloat)w / h, 1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_pos, y_pos, z_pos,
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

void
Navi(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        x_pos -= 0.2;
        break;
    case GLUT_KEY_RIGHT:
        x_pos += 0.2;
        break;
    case GLUT_KEY_DOWN:
        y_pos -= 0.2;
        break;
    case GLUT_KEY_UP:
        y_pos += 0.2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        z_pos -= 1;
        break;
    case GLUT_KEY_PAGE_UP:
        z_pos += 1;
        break;
    default:
        break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_pos, y_pos, z_pos,
              0, 0, 0,
              0, 1, 0);

    glutPostRedisplay();
}
