#version 330 core
precision highp float;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_uv;

out vec3 v_position;
out vec4 v_color;
out vec3 l_position;
out vec2 v_uv;
out vec3 v_position_cam;

uniform mat4 u_matrix;
uniform float u_time;
uniform vec2 u_resolution;
uniform vec3 u_position_cam;

void main(void) 
{
   gl_Position = u_matrix * vec4(a_position, 1.0);
   v_position = a_position.xyz;//gl_Position.xyz;
   l_position = gl_Position.xyz;
   v_color = a_color;

   // TODO animation
   v_uv = a_uv;
   v_position_cam = u_position_cam;
}