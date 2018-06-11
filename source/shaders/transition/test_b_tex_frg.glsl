#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;
in vec3 l_position;
in vec2 v_uv;
in vec3 v_position_cam;

uniform float u_time;
uniform vec2 u_resolution;
uniform sampler2D tex0;
uniform sampler2D tex1;

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

   float T = v_position_cam.x;
   float Y = float(int(v_uv.y * 10.0)) / 10.0;

   vec4 tex  = vec4(texture(tex0, vec2(v_uv.x + (T * Y), v_uv.y)));
   vec4 tex1 = vec4(texture(tex1, vec2(v_uv.x + (T * Y), v_uv.y)));

   float d = distance(pl.x, pr.x);
   float s = sign(pr.x - pl.x);
   float N = (.15) * noise(vec3(vec2(v_position + u_time), 0.0)) / 0.5;

   float off = (sin_pos(u_time + N) * .1) + .07;
   float f = smoothstep(-off, off, d * -s);

   c = mix(mix(tex1, vec4(vec3(1.0), 0.2), f), tex, f);
   color = vec4(sqrt(c.rgb), c.a);
}
