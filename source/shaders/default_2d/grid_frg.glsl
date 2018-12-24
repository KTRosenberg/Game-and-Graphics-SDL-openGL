#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;
in vec2 v_uv;
in vec3 v_position_cam;

uniform vec3 u_position_cam;
uniform float u_grid_square_pix;
uniform float u_time;
uniform float u_scale;

out vec4 color;

#define THRESH (2.0)

float sin01(float x)
{
   return (sin(x) + 1.0) / 2.0;
}

#define SCALE_TEST

void main(void)
{
#ifdef SCALE_TEST
   if (mod(v_position.x - v_position_cam.x + (THRESH / 2.0), u_grid_square_pix * u_scale) < THRESH ||
       mod(v_position.y - v_position_cam.y + (THRESH / 2.0), u_grid_square_pix * u_scale) < THRESH) {
      color = v_color; 
   } else {
      discard;
   }
#else
   if (mod(v_position.x - v_position_cam.x + (THRESH / 2.0), u_grid_square_pix) < THRESH ||
       mod(v_position.y - v_position_cam.y + (THRESH / 2.0), u_grid_square_pix) < THRESH) {
      color = v_color; 
   } else {
      discard;
   }
#endif

   // color = v_color * vec4(float(mod(v_position.x - v_position_cam.x, 128.0) < 5.0 ||
   //     mod(v_position.y - v_position_cam.y, 128.0) < 5.0));

}
