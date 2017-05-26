#include "camera.hpp"

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
    zoom(camera_defaults::ZOOM)
{
    this->update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix(void)
{
    return glm::lookAt(this->pos, this->pos + this->front, this->up);
}

void Camera::process_directional_movement(Camera_Movement direction, GLfloat delta_time)
{
    GLfloat velocity = this->movement_speed * delta_time;
    if (direction == Camera_Movement::FORWARDS) {
        this->pos += this->front * velocity;
    }
    if (direction == Camera_Movement::BACKWARDS) {
        this->pos -= this->front * velocity;
    }
    if (direction == Camera_Movement::LEFTWARDS) {
        this->pos -= this->right * velocity;
    }
    if (direction == Camera_Movement::RIGHTWARDS) {
        this->pos += this->right * velocity;
    }   
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
        