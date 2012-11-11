#include "Object.h"

Object::Object() : x(), y(), z(), type() {}

Object::Object(double x_, double y_, double z_) :
    x(x_), y(y_), z(z_), angle(), type()
{
    //srand(time(NULL));

    //type = rand_next(NUM - 1);
    angle = next_rand() * 90;
    if (z < eps) {
        type = BOAT;
    } else {
        type = TREE;
    }
}

Object::Type
Object::get_type(void) const
{
    return type;
}

void
Object::init_list(void)
{
    double width = 0;
    double height = 0;
    double length = 0;
    float a[3], b[3], c[3], d[3], n[3];
    switch (type)
    {
    case TREE:
        width = 2;
        height = 2;
        glNewList(TREE, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        a[0] = width / 2.0; a[1] = 0; a[2] = 0;
        b[0] = 0; b[1] = 0; b[2] = height;
        c[0] = -width / 2.0; c[1] = 0; c[2] =0;
        get_normal(a, b, c, n);
        glNormal3fv(n);
        glVertex3fv(a);
        glVertex3fv(b);
        glVertex3fv(c);
        a[0] = 0; a[1] = width / 2; a[2] = 0;
        b[0] = 0; b[1] = 0; b[2] = height;
        c[0] = 0; c[1] = -width / 2; c[2] = 0;
        get_normal(a, b, c, n);
        glNormal3fv(n);
        glVertex3fv(a);
        glVertex3fv(b);
        glVertex3fv(c);
        glEnd();
        glEndList();
        break;
    case BOAT:
        width = 1;
        length = 2;
        height = 1.3;
        glNewList(BOAT, GL_COMPILE);
        glBegin(GL_QUADS);
        a[0] = -length / 2.0; a[1] = 0; a[2] = 0;
        b[0] = length / 2.0; b[1] = 0; b[2] = 0;
        c[0] = length / 2.0; c[1] = 0; c[2] = height;
        d[0] = -length / 2.0; d[1] = 0; d[2] = height;
        get_normal(a, b, c, n);
        glNormal3fv(n);
        glVertex3fv(a);
        glVertex3fv(b);
        glVertex3fv(c);
        glVertex3fv(d);
        glEnd();
        glEndList();
        break;
    default:
        break;
    }
}

double
Object::get_x(void) const
{
    return x;
}

double
Object::get_y(void) const
{
    return y;
}

double
Object::get_z(void) const
{
    return z;
}

void
Object::set_x(double x_)
{
    x = x_;
}

void
Object::set_y(double y_)
{
    y = y_;
}

void
Object::set_z(double z_)
{
    z = z_;
}

void
Object::draw(void) const
{
    switch(type)
    {
    case TREE:
        draw_tree();
        break;
    case HOUSE:
        draw_house();
        break;
    case BOAT:
        draw_boat();
        break;
    default:
        break;
    }
}

void
Object::draw_tree(void) const
{
    enum
    {
        MAX_WIDTH = 2,
        MAX_HEIGHT = 2,
        R = 75,
        G = 255,
        B = 70
    };
    double width = MAX_WIDTH;
    double height = MAX_HEIGHT;
    glColor3ub(R, G, B);
    float a[3], b[3], c[3], n[3];
    glPushMatrix();
    glTranslated(x, y, z);
    glRotatef(angle, 0, 0, 1);
    glCallList(TREE);
    glPopMatrix();
}

void
Object::draw_house(void) const
{

}

void
Object::draw_boat(void) const
{
    const double MAX_WIDTH = 1;
    const double MAX_HEIGHT = 0.5;
    const double MAX_LENGTH = 1.7;
    enum
    {
        R = 123,
        G = 63,
        B = 0
    };
    double width = MAX_WIDTH;
    double height = MAX_HEIGHT;
    double length = MAX_LENGTH;
    glColor3ub(R, G, B);
    float a[3], b[3], c[3], d[3], n[3];
    double alpha = 70 * M_PI / 180.0, beta = 45 * M_PI / 180.0;
    double depth = 0.2;
    double shift_y = width / 2 - height / tan(alpha);
    double shift_x = length / 2 - height / tan(beta);
    /*
    glBegin(GL_TRIANGLE_STRIP);
    a[0] = x - length / 2; a[1] = y; a[2] = z;
    b[0] = x - shift_x; b[1] = y - shift_y; b[2] = z;
    c[0] = x - shift_x; c[1] = y + shift_y; b[2] = z;
    get_normal(a, b, c, n);
    glNormal3fv(n);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    a[0] = x + shift_x; a[1] = y - shift_y; a[2] = z;
    get_normal(b, c, a, n);
    glNormal3fv(n);
    glVertex3fv(a);
    b[0] = x + shift_x; b[1] = y + shift_y; b[2] = z;
    get_normal(c, a, b, n);
    glNormal3fv(n);
    glVertex3fv(b);
    c[0] = x + length / 2; c[1] = y; c[2] = z;
    get_normal(a, b, c, n);
    glNormal3fv(n);
    glVertex3fv(c);
    glEnd();
    */
    glPushMatrix();
    glTranslatef(x, y, z);
    glCallList(BOAT);
    glPopMatrix();
}
