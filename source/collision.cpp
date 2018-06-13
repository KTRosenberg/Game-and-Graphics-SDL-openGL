#include "collision.h"

i64 TEST(void)
{
    return 0.0;
}

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

