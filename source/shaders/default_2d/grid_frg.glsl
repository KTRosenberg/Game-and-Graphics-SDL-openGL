#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;
in vec2 v_uv;
in vec3 v_position_cam;

uniform vec3 u_position_cam;

out vec4 color;


void main(void)
{
   if (mod(gl_FragCoord.x - v_position_cam.x, (1280.0 / (32.0 / 9.0))) < 5.0 ||
       mod(gl_FragCoord.y - v_position_cam.y, (720.0 / (2.0))) < 5.0) {
      color = v_color;
   } else {
      discard;
   }
   // if (fract(v_position_cam.x + (v_position.x)) < 0.01f || fract(v_position_cam.y + v_position.y) < 0.01f) {
   //    color = v_color;
   // } else {
   //    discard;
   // } 
}
