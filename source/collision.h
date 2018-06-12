#ifndef COLLISION_H
#define COLLISION_H

#include "common_utils.h"
#include "core_utils.h"
#include "opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

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

i64 TEST(void);



#endif
