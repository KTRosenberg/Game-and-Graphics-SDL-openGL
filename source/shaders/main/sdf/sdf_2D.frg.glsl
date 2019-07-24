#version 410 core
precision highp float;

in vec3 v_position;
in vec4 v_color;

out vec4 color;

uniform float u_time;
uniform vec2 u_dimensions;

#define RES vec2(1280.0, 720.0)
#define ASPECT (1280.0 / 720.0)

float opU(float d1, float d2);
float opI(float d1, float d2);
float opSub(float d1, float d2);

float sdCircle(in vec2 p, in vec2 pos, float radius);
float sdBox(in vec2 p, in vec2 pos, in vec2 size);
float sminCubic(float a, float b, float k);
float opBlend(float d1, float d2);

vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d);
vec3 shade(float sd);

float sdLine(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}
float sdf(vec2 p)
{
    return opSub(
        opBlend(
            sdLine(p, vec2(-0.5, 0.5), vec2(0.5, -0.5)), 
            sdLine(p, vec2(-0.5, -0.5), vec2(0.5, 0.5))
        ), sdCircle(p, vec2(0.0), 0.25));
}

float sin01(float val)
{
    return (sin(val) + 1.0) / 2.0;
}
vec2 TR = vec2(0.0, 0.0);
vec2 SC = vec2(1.0);
void main(void)
{
    vec2 pos = (((v_position.xy / RES) + TR) - 0.5) * SC;
    pos.x *= ASPECT;
    float sd;
    vec3 col;
    sd = sdf(pos);
    //sd = sdBox(pos, vec2(0.0), vec2(sin01(u_time)));
    col = shade(sd);
    //col = vec3(shade(step(0.0, sd)));
    color = vec4(col, 1.0); //vec4(sqrt(v_color.rgb), v_color.a);
}

// --- SDF utility library

float sdCircle(in vec2 p, in vec2 pos, float radius)
{
    return length(p-pos)-radius;
}

float sdBox(in vec2 p, in vec2 pos, in vec2 size)
{
    vec2 d = abs(p-pos)-size;
    return min(0.0, max(d.x, d.y))+length(max(d,0.0));
}

// polynomial smooth min (k = 0.1);
float sminCubic(float a, float b, float k)
{
    float h = max(k-abs(a-b), 0.0);
    return min(a, b) - h*h*h/(6.0*k*k);
}

float opU(float d1, float d2)
{
    return min(d1, d2);
}
float opI(float d1, float d2)
{
    return max(d1, d2);
}
float opSub(float d1, float d2)
{
    return max(-d1, d2);
}

float opBlend(float d1, float d2)
{
    float k = 0.2;
    return sminCubic(d1, d2, k);
}

/////////////////////////////////////
vec3 palette(in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d)
{
    t = clamp(t, 0., 1.);
    return a + b*cos(6.28318*(c*t+d));
}

vec3 shade(float sd)
{
    float maxDist = 2.0;
    vec3 palCol = palette(clamp(0.5-sd*0.4, -maxDist,maxDist), 
                      vec3(0.3,0.3,0.0),vec3(0.8,0.8,0.1),vec3(0.9,0.7,0.0),vec3(0.3,0.9,0.8));

    vec3 col = palCol;
    
    // Darken around surface
    col = mix(col, col*1.0-exp(-10.0*abs(sd)), 0.4);
    // repeating lines
    col *= 0.8 + 0.2*cos(150.0*sd);
    // White outline at surface
    col = mix(col, vec3(1.0), 1.0-smoothstep(0.0,0.01,abs(sd)));
    
    return col;
}
