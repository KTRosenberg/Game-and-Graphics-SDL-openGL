#version 330 core
precision highp float;

in vec3 v_position;
in vec4 v_color;

out vec4 color;


void main(void)
{
   color = vec4(sqrt(v_color.rgb), v_color.a); 
}
