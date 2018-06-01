#version 330 core
precision highp float;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

out vec3 v_position;
out vec3 v_position_cam;
out vec4 v_color;
out vec2 v_uv;

uniform mat4 u_matrix;
uniform vec3 u_position_cam;
uniform vec4 u_color;



void main(void) 
{
   gl_Position = u_matrix * vec4(a_position, 1.0);
   v_position =  a_position.xyz;
   
   v_position_cam = vec3(
      -(u_position_cam.x - 640.0), 
      -(u_position_cam.y - 360.0), 
      1.0
   );

   v_color = u_color;
   v_uv = a_uv;
}
