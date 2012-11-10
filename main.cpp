#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
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
void set_camera(void);

GLint Width = 512, Height = 512;

double x_pos = 0;
double y_pos = 0;
double z_pos = 0;

vector<double> eye(3, 0);
vector<double> at(3, 0);
double phi = 0, psi = 0, r = 0;
const double shift = 1.5;

enum
{
    N = 7,
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
    srand(time(NULL));
    for (int i = 0; i < objects.size(); ++i) {
        int x = MARGIN + next_rand((terrain_matrix.get_size() - 2) / MARGIN) * MARGIN;
        int y = MARGIN + next_rand((terrain_matrix.get_size() - 2) / MARGIN) * MARGIN;

        objects[i] = Object(x, y, terrain_matrix.at(x, y));
    }
    terrain_matrix.generate_land_arrays(land_pointers, land_normals,
                                        land_colors, land_indices);
    terrain_matrix.generate_water_arrays(water_pointers, water_normals,
                                         water_colors, water_indices);
    r = 60;

    phi = 45;
    psi = 135;

    double size = terrain_matrix.get_size();
    at[0] = size / 2;
    at[1] = size / 2;
    at[2] = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(Width, Height);
    glutCreateWindow("Virtual Terrain");

    init_light();
    init_material();
    init_fog();

    //glEnable(GL_NORMALIZE);

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Navi);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        cerr << "Error: " << glewGetErrorString(err) << endl;
        return 0;
    }
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Render: " << glGetString(GL_RENDERER) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    cout << endl;

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
    float mat_dif[] = {0.9, 0.9, 0.9};
    float mat_amb[] = {0.2, 0.2, 0.2};
    float mat_spec[] = {0.2, 0.2, 0.2};
    float mat_shi = 0.2 * 128;
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
    GLfloat fog_color[4] = {0.5, 0.5, 0.55, 1};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.01);
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

    glDisable(GL_STENCIL_TEST);

    render_water();

    glFlush();
}

void
render_scene(void)
{
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

    set_camera();
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
    double dx = 0, dy = 0, dz = 0;
    double r_psi = psi * M_PI / 180;
    double r_phi = phi * M_PI / 180;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        dx = -shift * cos(r_phi - M_PI / 2);
        dy = -shift * sin(r_phi - M_PI / 2);
        break;
    case GLUT_KEY_RIGHT:
        dx = shift * cos(r_phi - M_PI / 2);
        dy = shift * sin(r_phi - M_PI / 2);
        break;
    case GLUT_KEY_DOWN:
        r += shift;
        break;
    case GLUT_KEY_UP:
        r -= shift;
        break;
    case GLUT_KEY_PAGE_DOWN:
        dx = -shift * sin(r_psi - M_PI / 2) * cos(r_phi);
        dy = -shift * sin(r_psi - M_PI / 2) * sin(r_phi);
        dz = -shift * cos(r_psi - M_PI / 2);
        break;
    case GLUT_KEY_PAGE_UP:
        dx = shift * sin(r_psi - M_PI / 2) * cos(r_phi);
        dy = shift * sin(r_psi - M_PI / 2) * sin(r_phi);
        dz = shift * cos(r_psi - M_PI / 2);
        break;
    default:
        break;
    }

    at[0] += dx;
    at[1] += dy;
    at[2] += dz;

    set_camera();
    Display();
}

void
set_camera(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    double r_psi = psi * M_PI / 180;
    double r_phi = phi * M_PI / 180;
    double x = r * sin(r_psi) * cos(r_phi);
    double y = r * sin(r_psi) * sin(r_phi);
    double z = r * cos(r_psi);
    eye[0] = at[0] - x;
    eye[1] = at[1] - y;
    eye[2] = at[2] - z;
    gluLookAt(eye[0], eye[1], eye[2],
            at[0], at[1], at[2],
            sin(r_psi - M_PI / 2) * cos(r_phi),
            sin(r_psi - M_PI / 2) * sin(r_phi),
            cos(r_psi - M_PI / 2));
}
