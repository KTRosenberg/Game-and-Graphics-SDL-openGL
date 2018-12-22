#ifndef COLLISION_H
#define COLLISION_H

#if !(UNITY_BUILD)
    #define COMMON_UTILS_CPP_IMPLEMENTATION
    #include "common_utils_cpp.hpp"
    #include "core_utils.h"
    #include "opengl.hpp"
#endif

#define COLLIDER_MAX_SELECTION_DISTANCE (81)

//typedef CollisionStatus (*Fn_CollisionHandler)(Vec3 incoming);

struct Collider {
    Vec3 a;
    Vec3 b;
    //Fn_CollisionHandler handler;
};

void Collider_print(Collider* c);

// x, y, z, collided
struct CollisionStatus {
    Collider* collider;

    Vec3 intersection;

    inline bool collided(void)
    {
        return this->collider != nullptr;
    }
};

void CollisionStatus_init(CollisionStatus* out, Vec3 init_val = Vec3(POSITIVE_INFINITY, POSITIVE_INFINITY, 0.0));

#define MAX_COLLIDERS (2048)
extern Array<Collider, MAX_COLLIDERS> collision_map; 

Vec3 temp_test_collision(Player* you, Collider* c);

#endif

#ifdef COLLISION_IMPLEMENTATION
#undef COLLISION_IMPLEMENTATION

void CollisionStatus_init(CollisionStatus* out, Vec3 init_val)
{
    out->collider = nullptr;
    out->intersection = init_val;
}

Array<Collider, MAX_COLLIDERS> collision_map;


void Collider_print(Collider* c)
{
    printf("[[%f, %f, %f][%f, %f, %f]]", c->a.x, c->a.y, c->a.z, c->b.x, c->b.y, c->b.z);  
}


#endif
