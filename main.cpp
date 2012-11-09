#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>

#include "Terrain.h"
#include "Random.h"
#include "Object.h"
#include "common.h"

using namespace std;

void Display();
void Reshape(GLint w, GLint h);
void Keyboard(unsigned char key, int x, int y);
void Navi(int key, int x, int y);
void init_light(void);
void init_material(void);
void init_fog(void);
void render_scene(void);
void render_water(void);

GLint Width = 512, Height = 512;

double x_pos = 0;
double y_pos = 0;
double z_pos = 0;

enum
{
    N = 5,
    MARGIN = 2,
    OBJECTS_NUM = 50
};
Terrain terrain_matrix(N);
vector<Object> objects(OBJECTS_NUM);
vector<float> land_pointers;
vector<float> land_normals;
vector<unsigned char> land_colors;
vector<unsigned> land_indices;
vector<float> water_pointers;
vector<float> water_normals;
vector<unsigned char> water_colors;
vector<unsigned> water_indices;

int
main(int argc, char *argv[])
{
    //terrain_matrix.print();
    srand(time(NULL));
    for (int i = 0; i < objects.size(); ++i) {
        int x = MARGIN + next_rand(terrain_matrix.get_size() - 2 * MARGIN);
        int y = MARGIN + next_rand(terrain_matrix.get_size() - 2 * MARGIN);
        cerr << x << ' ' << y << endl;
        objects[i] = Object(x, y, terrain_matrix.at(x, y));
    }
    terrain_matrix.generate_land_arrays(land_pointers, land_normals,
                                        land_colors, land_indices);
    terrain_matrix.generate_water_arrays(water_pointers, water_normals,
                                         water_colors, water_indices);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Virtual Terrain");

    init_light();
    init_material();
    //init_fog();
/*
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
*/
    //glEnable(GL_NORMALIZE);

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Navi);

    glutMainLoop();

    return 0;
}

void
init_light(void)
{
    GLfloat light_ambient[] = {0, 0, 0, 1};
    GLfloat light_diffuse[] = {1, 1, 1, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};
    GLfloat light_position[] = {300, 300, 300, 1};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
init_material(void)
{
    float mat_dif[] = {0.9, 0.9, 0};
    float mat_amb[] = {0.2, 0.2, 0.2};
    float mat_spec[] = {0.6, 0.6, 0.6};
    float mat_shi = 0.5 * 128;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shi);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void
init_fog(void)
{
    GLfloat fog_color[4] = {0.5, 0.5, 0.5, 1};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 20);
    glFogf(GL_FOG_END, 100);
    glFogfv(GL_FOG_COLOR, fog_color);
}

void
Display(void)
{
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    render_scene();

    glColorMask(0, 0, 0, 0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    render_water();
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glColorMask(1, 1, 1, 1);
    glStencilFunc(GL_EQUAL, 1, 0xffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    double reflection_matrix[4][4] = {{1, 0, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 0, -1, 0},
                                      {0, 0, 0, 1}};

    glPushMatrix();
    glMultMatrixd((double *) reflection_matrix);

    render_scene();

    glPopMatrix();

    render_water();

    glDisable(GL_STENCIL_TEST);

    glFlush();
}

void
render_scene(void)
{
    /*
    glBegin(GL_TRIANGLES);
    for (int x = 0; x < terrain_matrix.get_size() - 1; ++x) {
        for (int y = 0; y < terrain_matrix.get_size() - 1; ++y) {
            float a[3], b[3], c[3], n[3];
            unsigned char cr[3];
            a[0] = x; a[1] = y; a[2] = terrain_matrix.at(a[0], a[1]);
            b[0] = x + 1; b[1] = y; b[2] = terrain_matrix.at(b[0], b[1]);
            c[0] = x + 1; c[1] = y + 1; c[2] = terrain_matrix.at(c[0], c[1]);
            if (a[2] > eps || b[2] > eps || c[2] > eps) {
                set_color(a[2], cr); glColor3ubv(cr);
                glVertex3fv(a);
                set_color(b[2], cr); glColor3ubv(cr);
                glVertex3fv(b);
                set_color(c[2], cr); glColor3ubv(cr);
                get_normal(a, b, c, n);
                glNormal3fv(n);
                glVertex3fv(c);
            }
            a[0] = x; a[1] = y; a[2] = terrain_matrix.at(a[0], a[1]);
            b[0] = x + 1; b[1] = y + 1; b[2] = terrain_matrix.at(b[0], b[1]);
            c[0] = x; c[1] = y + 1; c[2] = terrain_matrix.at(c[0], c[1]);
            if (a[2] > eps || b[2] > eps || c[2] > eps) {
                set_color(a[2], cr); glColor3ubv(cr);
                glVertex3fv(a);
                set_color(b[2], cr); glColor3ubv(cr);
                glVertex3fv(b);
                set_color(c[2], cr); glColor3ubv(cr);
                get_normal(a, b, c, n);
                glNormal3fv(n);
                glVertex3fv(c);
            }
        }
    }
    glEnd();
    */

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &land_pointers[0]);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, &land_colors[0]);
    glNormalPointer(GL_FLOAT, 0, &land_normals[0]);
    glDrawElements(GL_TRIANGLES, land_indices.size(), GL_UNSIGNED_INT, &land_indices[0]);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < objects.size(); ++i) {
        objects[i].draw();
    }
}

void
render_water(void)
{
    /*
    glBegin(GL_TRIANGLES);
    for (int x = 0; x < terrain_matrix.get_size() - 1; ++x) {
        for (int y = 0; y < terrain_matrix.get_size() - 1; ++y) {
            float a[3], b[3], c[3], n[3];
            unsigned char cr[4];
            a[0] = x; a[1] = y; a[2] = terrain_matrix.at(a[0], a[1]);
            b[0] = x + 1; b[1] = y; b[2] = terrain_matrix.at(b[0], b[1]);
            c[0] = x + 1; c[1] = y + 1; c[2] = terrain_matrix.at(c[0], c[1]);
            if (a[2] < eps && b[2] < eps && c[2] < eps) {
                set_water_color(cr); glColor4ubv(cr);
                glVertex3fv(a);
                set_water_color(cr); glColor4ubv(cr);
                glVertex3fv(b);
                set_water_color(cr); glColor4ubv(cr);
                get_normal(a, b, c, n);
                glNormal3fv(n);
                glVertex3fv(c);
            }
            a[0] = x; a[1] = y; a[2] = terrain_matrix.at(a[0], a[1]);
            b[0] = x + 1; b[1] = y + 1; b[2] = terrain_matrix.at(b[0], b[1]);
            c[0] = x; c[1] = y + 1; c[2] = terrain_matrix.at(c[0], c[1]);
            if (a[2] < eps && b[2] < eps && c[2] < eps) {
                set_water_color(cr); glColor4ubv(cr);
                glVertex3fv(a);
                set_water_color(cr); glColor4ubv(cr);
                glVertex3fv(b);
                set_water_color(cr); glColor4ubv(cr);
                get_normal(a, b, c, n);
                glNormal3fv(n);
                glVertex3fv(c);
            }
        }
    }
    glEnd();
    */

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &water_pointers[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &water_colors[0]);
    glNormalPointer(GL_FLOAT, 0, &water_normals[0]);
    glDrawElements(GL_TRIANGLES, water_indices.size(), GL_UNSIGNED_INT, &water_indices[0]);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void
Reshape(GLint w, GLint h)
{
    Width = w;
    Height = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (GLfloat)w / h, 1, 2000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-15, -15, 15,
              0, 0, 0,
              0, 0, 1);
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
    float x_tmp = x_pos;
    float y_tmp = y_pos;
    float z_tmp = z_pos;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        x_pos += 1.2;
        break;
    case GLUT_KEY_RIGHT:
        x_pos -= 1.2;
        break;
    case GLUT_KEY_DOWN:
        y_pos += 1.2;
        break;
    case GLUT_KEY_UP:
        y_pos -= 1.2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        z_pos += 1.2;
        break;
    case GLUT_KEY_PAGE_UP:
        z_pos -= 1.2;
        break;
    default:
        break;
    }

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(x_pos, y_pos, z_pos,
    //          0, 0, 0,
    //          0, 1, 0);

    //glutPostRedisplay();

    glTranslatef(x_pos, y_pos, z_pos);
    Display();
    x_pos = x_tmp;
    y_pos = y_tmp;
    z_pos = z_tmp;
}
