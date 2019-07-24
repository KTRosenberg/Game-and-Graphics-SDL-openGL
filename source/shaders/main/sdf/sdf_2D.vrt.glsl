#version 410 core
precision highp float;

layout (location = 0) in  vec3  a_position;
layout (location = 1) in  float a_angle;
layout (location = 2) in  vec4  a_color;
layout (location = 3) in  vec2  a_uv;
layout (location = 4) in  vec2  a_rotation_center;

out vec3 v_position;
out vec4 v_color;

uniform mat4 u_matrix;
uniform float u_time;
uniform vec2 u_dimensions;

void main(void) 
{
   gl_Position = u_matrix * vec4(a_position, 1.0);
   v_position = a_position.xyz;

   v_color = a_color;
}