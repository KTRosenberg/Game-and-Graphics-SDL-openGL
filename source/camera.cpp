#include "camera.hpp"


void ViewCamera_create(ViewCamera* view, glm::vec3 position_start, GLfloat speed, GLfloat min_z, GLfloat max_z, GLfloat min_x, GLfloat max_x, GLfloat min_y, GLfloat max_y)
{
    view->position = -position_start; 
    view->speed   = speed;
    view->matrix  = glm::mat4(1.0f);

    view->min_x = min_x;
    view->max_x = max_x;
    view->min_y = min_y;
    view->max_y = max_y;
    view->min_z = min_z;
    view->max_z = max_z;
}

void ViewCamera_process_directional_movement(ViewCamera* view, Movement_Direction direction, GLfloat delta_time)
{
    const GLfloat velocity = view->speed * delta_time;
    glm::vec3* p = &view->position;

    #define DB
    switch (direction) {
    case Movement_Direction::FORWARDS:
        #ifdef DB 
        std::cout << "FORWARDS" << std::endl; 
        #endif
        p->z -= velocity;
        break;
    case Movement_Direction::BACKWARDS:
        #ifdef DB 
        std::cout << "BACKWARDS" << std::endl;
        #endif
        p->z += velocity;
        break;
    case Movement_Direction::LEFTWARDS:
        #ifdef DB 
        std::cout << "LEFTWARDS" << std::endl; 
        #endif
        p->x -= velocity;
        break;
    case Movement_Direction::RIGHTWARDS:
        #ifdef DB 
        std::cout << "RIGHTWARDS" << std::endl; 
        #endif
        p->x += velocity;
        break;
    case Movement_Direction::UPWARDS:
        #ifdef DB 
        std::cout << "UPWARDS" << std::endl; 
        #endif
        p->y -= velocity;
        break;
    case Movement_Direction::DOWNWARDS:
        #ifdef DB 
        std::cout << "DOWNWARDS" << std::endl; 
        #endif
        p->y += velocity;
        break;
    }


    p->x = glm::clamp(view->position.x, view->min_x, view->max_x);  
    p->y = glm::clamp(view->position.y, view->min_y, view->max_y);  
    p->z = glm::clamp(view->position.z, view->min_z, view->max_z);
}


static const glm::mat4 m_identity(1.0f);
glm::mat4 ViewCamera_calc_view_matrix(ViewCamera* view) 
{
    return view->matrix = glm::translate(m_identity, -view->position);
}


///////////////////////////////////////////////////////////////////////

// constructor (vector values)
Camera::Camera(glm::vec3 pos, glm::vec3 up, GLfloat yaw, GLfloat pitch) 
:   pos(pos),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(up),
    world_up(up),
    yaw(yaw),
    pitch(pitch),
    movement_speed(camera_defaults::SPEED),
    mouse_sensitivity(camera_defaults::SENSITIVITY),
    zoom(camera_defaults::ZOOM)
{
    this->update_camera_vectors();    
}

// constructor (scalar values)
Camera::Camera(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z,
                GLfloat up_x, GLfloat up_y, GLfloat up_z,
                GLfloat yaw, GLfloat pitch)
:   pos(glm::vec3(pos_x, pos_y, pos_z)),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(glm::vec3(up_x, up_y, up_z)),
    world_up(glm::vec3(up_x, up_y, up_z)),
    yaw(yaw),
    pitch(pitch),
    movement_speed(camera_defaults::SPEED),
    mouse_sensitivity(camera_defaults::SENSITIVITY),
    zoom(camera_defaults::ZOOM),
    minZ(-1.0f),
    maxZ(1.0f)
{
    this->update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix(void)
{

    return glm::translate(glm::mat4(1.0f), this->pos);
}

void Camera::process_directional_movement(Camera_Movement direction, GLfloat delta_time)
{
    const GLfloat velocity = this->movement_speed * delta_time;
    
    switch (direction) {
    case Camera_Movement::FORWARDS:
        this->pos.z -= velocity;
        break;
    case Camera_Movement::BACKWARDS:
        this->pos.z += velocity;
        break;
    case Camera_Movement::LEFTWARDS:
        this->pos.x -= velocity;
        break;
    case Camera_Movement::RIGHTWARDS:
        this->pos.x += velocity;
        break;
    case Camera_Movement::UPWARDS:
        this->pos.y += velocity;
        break;
    case Camera_Movement::DOWNWARDS:
        this->pos.y -= velocity;
        break;
    }

    this->pos.z = glm::clamp(this->pos.z, this->minZ, this->maxZ);  
}
void Camera::process_mouse_movement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch)
{
    x_offset *= this->mouse_sensitivity;
    y_offset *= this->mouse_sensitivity;
    
    this->yaw = glm::mod(this->yaw + x_offset, 360.0f);
    this->pitch += y_offset;
    
    if (constrain_pitch == GL_TRUE) {
        if (this->pitch > MAX_BOUND) {
            this->pitch = MAX_BOUND;
        } else if (this->pitch < MIN_BOUND) {
            this->pitch = MIN_BOUND;
        }
    }
    
    this->update_camera_vectors();
}
void Camera::process_mouse_scroll(GLfloat y_offset)
{
    if (this->zoom >= MIN_MOUSE_ZOOM && this->zoom <= MAX_MOUSE_ZOOM) {
        this->zoom -= y_offset;
    }
    if (this->zoom <= MIN_MOUSE_ZOOM) {
        this->zoom = MIN_MOUSE_ZOOM;
    } else if (this->zoom >= MAX_MOUSE_ZOOM) {
        this->zoom = MAX_MOUSE_ZOOM;
    }
}

void Camera::update_camera_vectors(void)
{
    // new front vector calculation
    glm::vec3 front(
        cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
        sin(glm::radians(this->pitch)),
        sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
    );
    this->front = glm::normalize(front);
    // re-calculate the right and ip vector
    this->right = glm::normalize(glm::cross(this->front, this->world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up    = glm::normalize(glm::cross(this->right, this->front));
}
        