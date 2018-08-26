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
uniform float u_time;
uniform float u_scale;

float sin01(float x)
{
   return (sin(x) + 1.0) / 2.0;
}

#define SCALE_TEST
void main(void) 
{
   gl_Position = u_matrix * vec4(a_position, 1.0);
   v_position =  a_position.xyz;

   vec2 c = vec2(640.0, 360.0);
#ifdef SCALE_TEST
   vec2 scaled_pos = ((-u_position_cam.xy - c) * u_scale) + c;
#else
   vec2 scaled_pos = -u_position_cam.xy;
#endif
   v_position_cam = vec3(
      scaled_pos,
      1.0
   );

   v_color = u_color;
   v_uv = a_uv;
}
