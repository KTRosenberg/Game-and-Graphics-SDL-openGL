#version 330 core
precision highp float;

in vec2 t0c, t1c, t2c, t3c, t4c;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

out vec4 color;

void main(void)
{
    vec4 t0, t1, t2, t3, t4;

    t0 = texture(tex0, t0c);
    t1 = texture(tex1, t1c);
    t2 = texture(tex2, t2c);
    t3 = texture(tex3, t3c);    
    t4 = texture(tex4, t4c);

    color = t0;
    color = vec4(t1.a) * t1 + vec4(1.0 - t1.a) * color;
    color = vec4(t2.a) * t2 + vec4(1.0 - t2.a) * color;
    color = vec4(t3.a) * t3 + vec4(1.0 - t3.a) * color;
    color = vec4(t4.a) * t4 + vec4(1.0 - t4.a) * color;
}
