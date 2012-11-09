#include "Object.h"

Object::Object() : x(), y(), z(), type() {}

Object::Object(double x_, double y_, double z_) :
    x(x_), y(y_), z(z_), type()
{
    //srand(time(NULL));

    //type = rand_next(NUM - 1);
    type = TREE;
    if (z < eps) {
        type = BOAT;
    }
}

Object::Type
Object::get_type(void) const
{
    return type;
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
        MAX_WIDTH = 1,
        MAX_HEIGHT = 1,
        R = 75,
        G = 255,
        B = 70
    };
    double width = MAX_WIDTH;
    double height = MAX_HEIGHT;
    glColor3ub(R, G, B);
    float a[3], b[3], c[3], n[3];
    glBegin(GL_TRIANGLES);
    a[0] = x + width / 2; a[1] = y; a[2] = z;
    b[0] = x; b[1] = y; b[2] = z + height;
    c[0] = x - width / 2; c[1] = y; c[2] = z;
    get_normal(a, b, c, n);
    glNormal3fv(n);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    a[0] = x; a[1] = y + width / 2; a[2] = z;
    b[0] = x; b[1] = y; b[2] = z + height;
    c[0] = x; c[1] = y - width / 2; c[2] = z;
    get_normal(a, b, c, n);
    glNormal3fv(n);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    glEnd();
}

void
Object::draw_house(void) const
{

}

void
Object::draw_boat(void) const
{
    const double MAX_WIDTH = 0.5;
    const double MAX_HEIGHT = 0.5;
    const double MAX_LENGTH = 0.7;
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
    glColor3ub(R, G, B);
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
    glBegin(GL_QUADS);
    a[0] = x - length / 2; a[1] = y; a[2] = z;
    b[0] = x + length / 2; b[1] = y; b[2] = z;
    c[0] = x + length / 2; c[1] = y; c[2] = z + height;
    d[0] = x - length / 2; d[1] = y; d[2] = z + height;
    get_normal(a, b, c, n);
    glNormal3fv(n);
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
    glVertex3fv(d);
    glEnd();
}
