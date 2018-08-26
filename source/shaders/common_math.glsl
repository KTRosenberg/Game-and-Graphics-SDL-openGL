float sin01(float x)
{
   return (sin(x) + 1.0) / 2.0;
}

vec2 abs_fract(vec2 x) 
{
    x = abs(x);
    return x - floor(x);
}
