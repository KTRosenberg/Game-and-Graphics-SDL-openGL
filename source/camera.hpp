#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

#define MAX_BOUND 89.0f
#define MIN_BOUND -89.0f
#define MIN_MOUSE_ZOOM 1.0f
#define MAX_MOUSE_ZOOM 45.0f

enum class Camera_Movement : unsigned char 
{
    FORWARDS,
    BACKWARDS,
    LEFTWARDS,
    RIGHTWARDS
};

namespace camera_defaults {
    const GLfloat YAW = -90.0f;
    const GLfloat PITCH = 0.0f;
    const GLfloat SPEED = 4.0f;
    const GLfloat SENSITIVITY = 0.25f;
    const GLfloat ZOOM = 45.0f;
}

struct Camera {
    // attributes
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // Euler angles
    GLfloat yaw;
    GLfloat pitch;
    // camera options
    GLfloat movement_speed;
    GLfloat mouse_sensitivity;
    GLfloat zoom;
    
    // constructor (vector values)
    Camera(
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        GLfloat yaw = camera_defaults::YAW,
        GLfloat pitch = camera_defaults::PITCH
    );
    
    // constructor (scalar values)
    Camera(
        GLfloat pos_x, GLfloat pos_y, GLfloat pos_z,
        GLfloat up_x, GLfloat up_y, GLfloat up_z,
        GLfloat yaw = camera_defaults::YAW,
        GLfloat pitch = camera_defaults::PITCH
    );
    
    glm::mat4 get_view_matrix(void);

    void process_directional_movement(Camera_Movement direction, GLfloat delta_time);
    void process_mouse_movement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch = GL_TRUE);
    void process_mouse_scroll(GLfloat y_offset);
private:
    void update_camera_vectors(void);
};

// by Laurie Bradshaw, from comment in https://learnopengl.com/#!Getting-started/Camera
struct FreeCamera {
	glm::vec3 m_pos;
	glm::quat m_orient;

	FreeCamera (void) = default;
	FreeCamera (const FreeCamera &) = default;

	FreeCamera (const glm::vec3 &pos) : m_pos(pos) {}
	FreeCamera (const glm::vec3 &pos, const glm::quat &orient) : m_pos(pos), m_orient(orient) {}

	FreeCamera  &operator=(const FreeCamera&) = default;

	const glm::vec3 &position(void) const { return m_pos; }
	const glm::quat &orientation(void) const { return m_orient; }

	glm::mat4 get_view_matrix(void) const { return glm::translate(glm::mat4_cast(m_orient), m_pos); }

	void translate(const glm::vec3 &v) { m_pos += v * m_orient; }
	void translate(float x, float y, float z) { m_pos += glm::vec3(x, y, z) * m_orient; }

	void rotate(float angle, const glm::vec3& axis) { m_orient *= glm::angleAxis(angle, axis * m_orient); }
	void rotate(float angle, float x, float y, float z) { m_orient *= glm::angleAxis(angle, glm::vec3(x, y, z) * m_orient); }

	void yaw(float angle) { this->rotate(angle, 0.0f, 1.0f, 0.0f); }
	void pitch(float angle) { this->rotate(angle, 1.0f, 0.0f, 0.0f); }
	void roll(float angle) { this->rotate(angle, 0.0f, 0.0f, 1.0f); }
};



#endif // CAMERA_HPP