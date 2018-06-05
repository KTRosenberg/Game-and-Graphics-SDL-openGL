#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;
in vec2 v_uv;
in vec3 v_position_cam;

uniform vec3 u_position_cam;
uniform float u_grid_square_pix;

out vec4 color;

#define THRESH (5.0)


void main(void)
{
   if (mod(v_position.x - v_position_cam.x, u_grid_square_pix) < THRESH ||
       mod(v_position.y - v_position_cam.y, u_grid_square_pix) < THRESH) {
      color = v_color;
   } else {
      discard;
   }

   // color = v_color * vec4(float(mod(v_position.x - v_position_cam.x, 128.0) < 5.0 ||
   //     mod(v_position.y - v_position_cam.y, 128.0) < 5.0));

}
