#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;
in vec3 l_position;
in vec2 v_uv;

uniform float u_time;
uniform vec2 u_resolution;

out vec4 color;

float sin_pos(float f) { return (1.0 + sin(f)) / 2.0; }

vec3 sin_pos_v3(vec3 v3)
{
   return vec3(
      sin_pos(v3.x),
      sin_pos(v3.y),
      sin_pos(v3.z)
   );
}

void main(void)
{
   vec4 c = vec4(0.0);
   
   vec3 cl = vec3(0.0, 0.0, 1.0);
   vec3 pl = v_position;
   for (int i = 0; i < 1; ++i) {
      pl.x += (.15) * noise(pl + u_time);
   }

   vec3 cr = vec3(1.0, 0.0, 0.0);
   vec3 pr = vec3(0.5, 0.0, 0.0);

   float d = distance(pl.x, pr.x);
   float s = sign(pr.x - pl.x);
   float N = (.15) * noise(vec3(vec2(v_position + u_time), 0.0)) / 0.5;

   float off = (sin_pos(u_time + N) * .1);
   float f = smoothstep(-.1 - off, .1 + off, d * -s);

   c = mix(mix(vec4(cl, 1.0), vec4(1.0, 0.0, 1.0, pl.y), f), vec4(cr, 1.0), f);
   color = vec4(sqrt(c.rgb), c.a);
/*
   vec4 A = vec4(1.0);
   vec4 B = vec4(0.0, 0.0, 1.0, 1.0);
   vec3 vpos = v_position;
   //vpos.x = sin(u_time + vpos.x);
   float X = vpos.x;

   if (X < -1.0) {
      color = A;
   } else if (X < -.5) {
      color = B;
   } else if (X < 0.0) {
      color = A;
   } else if (X < 0.5) {
      color = B;
   } else if (X < 1.0) {
      color = A;
   } else if (X < 1.5) {
      color = B;
   }
*/
   //color = vec4(sqrt(v_color.rgb), v_color.a); 
   //color = vec4(vec3(v_position / vec3(u_resolution.xy, 1.0)), 1.0);
}
