
bool temp_test_collision(Player* you, Collider* c, glm::vec3* out)
{
    auto sensors = you->floor_sensor_rays();

    std::pair<glm::vec3, glm::vec3>* ray0 = &sensors.first;
    std::pair<glm::vec3, glm::vec3>* ray1 = &sensors.second;
    std::pair<glm::vec3, glm::vec3> collider = {
        c->a,
        c->b
    };


        // printf("COLLIDER: ");
        // Collider_print(c);
        // printf("\nagainst\n");
        // vec3_pair_print(&ray0.first, &ray0.second);
        // printf("\nand\n");
        // vec3_pair_print(&ray1.first, &ray1.second);
        // printf("\n-------------------------\n");

    glm::vec3 va(POSITIVE_INFINITY);
    glm::vec3 vb(POSITIVE_INFINITY);
    glm::vec3* choice = &va;
    bool possibly_collided = false;

    if (line_segment_intersection(ray0, &collider, &va)) {
        choice = &va;
        possibly_collided = true;
    }

    if (line_segment_intersection(ray1, &collider, &vb)) {
        choice = (va.y < vb.y) ? &va : &vb;
        possibly_collided = true;
    }

    if (!possibly_collided) {
        return false;
    }

    // TODO FIX BUG: HEIGHT OVERRIDDEN BY SUCCESSIVE COLLIDERS EVEN IF LOWER,
    // MUST COMPARE ALL COLLIDERS BEFORE MODIFYING VALUE
   // std::cout << "ON_GROUND: " << ((you->on_ground) ? "TRUE" : "FALSE") << std::endl;
    if (!you->on_ground && you->bound.spatial.y + you->bound.height >= choice->y) {
        f64 new_y = choice->y;
        // if (!first_check && new_y >= you->bound.spatial.y) {
        //     return false;
        // }

        //you->bound.spatial.y = new_y;

        if (new_y > out->y) {
            return false;
        }

        out->x = choice->x;
        out->y = choice->y;
        out->z = 0.0;
        
        return true;
    } else if (you->on_ground) {
        f64 new_y = choice->y;
        // if (!first_check && new_y >= you->bound.spatial.y) {
        //     return false;
        // }

        //you->bound.spatial.y = new_y;
        if (new_y > out->y) {
            return false;
        }

        out->x = choice->x;
        out->y = choice->y;
        out->z = 0.0;
        
        return true;        
    }


    return false;

}


            bool collided = false;

            glm::vec3 out(POSITIVE_INFINITY);
            for (auto it = collision_map.begin(); it != collision_map.first_free(); ++it)
            {
                //Collider_print(it);
                
                if (temp_test_collision(&you, it, &out)) {
                    //printf("COLLISION\n");
                    gl_draw2d.line(glm::vec3(0.0), out);
                    you.on_ground = true;
                    you.state_change_time = t_now;
                    collided = true;

                    //puts("COLLIDED");
                } else {
                    //puts("NO COLLISION");
                }
            }