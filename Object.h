#ifndef OBJECT_H
#define OBJECT_H

#include <GL/gl.h>
#include <ctime>

#include "common.h"
#include "Random.h"

class Object
{
public:
    enum Type
    {
        NUM = 3,

        TREE = 0,
        BOAT = 1,
        HOUSE = 2
    };

    Object();
    Object(double x_, double y_, double z_);
    void draw(void) const;
    double get_x(void) const;
    double get_y(void) const;
    double get_z(void) const;
    Type get_type(void) const;
    void set_x(double x_);
    void set_y(double y_);
    void set_z(double z_);

private:
    double x;
    double y;
    double z;
    Type type;

    void draw_tree(void) const;
    void draw_boat(void) const;
    void draw_house(void) const;
};

#endif // OBJECT_H
