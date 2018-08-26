#version 330 core
precision highp float;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

out vec2 t0c, t1c, t2c, t3c, t4c;



uniform mat4 u_matrix;
//uniform float u_aspect;
uniform vec3 u_position_cam;

uniform float u_time;

uniform float u_scale;

//uniform float u_precompute;

#define SCALE_TEST


float sin01(float x)
{
    return (sin(x) + 1.0) / 2.0;
}


vec2 abs_fract(vec2 x) 
{
    x = abs(x);
    return x - floor(x);
}

vec2 scale(vec2 v, vec2 c, vec2 scaler)
{
    return (((v - (c + 0.5)) * scaler) + (c + 0.5));

    // // rotation
    // vec2 cold = c;
    // vec2 trans = ((v - (c + 0.5)) * scaler);
    // float x = (cos(u_time) * trans.x) - (sin(u_time) * trans.y);
    // float y = (sin(u_time) * trans.x) + (cos(u_time) * trans.y);
    // trans.x = x;
    // trans.y = y;
    // return trans + (c + 0.5);
}

void main(void) 
{
#ifdef SCALE_TEST
    vec2 scaler = vec2(1.0 / u_scale);
#endif
    // vec2 tex_res = vec2(2048.0, 1024.0);
    // vec3 world_bguv_factor = vec3(vec2(1.0) / tex_res, 1.0);
    // vec2 cam_offset = vec2(640.0, 480.0) * world_bguv_factor.xy;

    gl_Position = u_matrix * vec4(a_position, 1.0);

    // float x_off = ((u_position_cam.x - (u_position_cam.x + cam_offset.x)) * scale) + u_position_cam.x;
    // float y_ = clamp(u_position_cam.y, -1.45, 1.45);
    // float y_off = ((u_position_cam.y - (u_position_cam.y + cam_offset.y)) * scale) + u_position_cam.y;

    float x_off = u_position_cam.x;
    float y_off = clamp(u_position_cam.y, -1.45, 1.45);

#ifdef SCALE_TEST
    #define SCALED(v, c, sc) scale(v, c, sc)
#else
    #define SCALED(v, c, sc) v
#endif


    t0c = a_uv;
    t0c.x += (x_off / 4.0);
    t0c.y += (y_off / 10.0);
    t0c = SCALED(t0c, vec2(x_off / 4.0, y_off / 10.0), scaler);
    //t0c = fract(t0c);

    t1c = a_uv;
    t1c.x += (x_off / 4.0);
    t1c.y += (y_off / 10.0);
    t1c = SCALED(t1c, vec2(x_off / 4.0, y_off / 10.0), scaler);
    //t1c = fract(t1c);

    t2c = a_uv;
    t2c.x += (x_off / 2.8);
    t2c.y += (y_off / 10.0);
    t2c = SCALED(t2c, vec2(x_off / 2.8, y_off / 10.0), scaler);
    //t2c = fract(t2c);

    t3c = a_uv;
    t3c.x += (x_off / 2.3);
    t3c.y += (y_off / 7.0);
    t3c = SCALED(t3c, vec2(x_off / 2.3, y_off / 7.0), scaler);
    //t3c = fract(t3c);

    t4c = a_uv;
    t4c.x += (x_off / 1.0);
    t4c.y += (y_off / 1.0);
    t4c = SCALED(t4c, vec2(x_off / 1.0, y_off / 1.0), scaler);
    //t4c = abs_fract(t4c);
    #undef SCALED


    // hard-coded version of above
    // vec2 CONST = (a_uv * scaler) + (vec2(-x_off, -y_off) * scaler) - vec2(0.5 * scaler) + vec2(x_off, y_off) + vec2(0.5);

    // t0c = CONST + ((vec2(x_off, y_off) / vec2(4.0, 10.0)) * scaler);
    // t1c = CONST + ((vec2(x_off, y_off) / vec2(4.0, 10.0)) * scaler);
    // t2c = CONST + ((vec2(x_off, y_off) / vec2(2.8, 10.0)) * scaler);
    // t3c = CONST + ((vec2(x_off, y_off) / vec2(2.3,  7.0)) * scaler);
    // t4c = CONST + ((vec2(x_off, y_off) / vec2(1.0,  1.0)) * scaler);



}
