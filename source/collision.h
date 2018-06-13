#ifndef COLLISION_H
#define COLLISION_H

#include "common_utils.h"
#include "common_utils_cpp.h"
#include "core_utils.h"
#include "opengl.hpp"

// x, y, z, collided
struct CollisionStatus {
    glm::vec4 data;

    float64 x(void);

    float64 y(void);

    float64 z(void);

    bool collided(void);
};

//typedef CollisionStatus (*Fn_CollisionHandler)(glm::vec3 incoming);

struct Collider {
    glm::vec3 a;
    glm::vec3 b;
    //Fn_CollisionHandler handler;
};

void Collider_print(Collider* c);

#define MAX_COLLIDERS (2048)
extern Buffer<Collider, MAX_COLLIDERS> collision_map; 

// i64 TEST(void);
#endif

#ifdef COLLISION_IMPLEMENTATION

Buffer<Collider, MAX_COLLIDERS> collision_map;

float64 CollisionStatus::x(void) 
{
    return this->data.x;
}

float64 CollisionStatus::y(void)
{
    return this->data.y;
}

float64 CollisionStatus::z(void)
{
    return this->data.z;
}

bool CollisionStatus::collided(void)
{
    return this->data.w;
}

void Collider_print(Collider* c)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", c->a.x, c->a.y, c->a.z, c->b.x, c->b.y, c->b.z);  
}

#undef COLLISION_IMPLEMENTATION
#endif
