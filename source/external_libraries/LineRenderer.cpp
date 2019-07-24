/*
IMPORTANT INFO:
This code is made to be renderer-agnostic.
You would complete the implementation by implementing the triangle() function for your particular renderer.
The code is not written to make use of an index buffer, so it is not perfectly efficient in terms of memory usage.
You can replace the vector / matrix math with types from your own math library if you want, or leave it as-is.
This implementation assumes coordinates are in screen pixel space (circleDetail() relies on that fact).
If you want to draw partially transparent lines, you will need render all triangles of a line at the same depth
    and use the less-than or greater-than or not-equal depth test (e.g. glDepthFunc(GL_LESS) or GL_GREATER or GL_NOTEQUAL)
    because some triangles overlap.

EXAMPLE USAGE:
    LineRenderer line;
    line.cap = ...;
    line.join = ...;
    line.r = ...; //in pixels
    line.strokeColor = ...;

    line.beginLine();
    line.lineVertex(...);
    line.lineVertex(...);
    line.lineVertex(...);
    //etc.
    line.endLine(...); //boolean argument = whether to connect the first and last point of the line together
*/

#include <stdint.h>
#include <math.h>

namespace nnnv { // notnullnotvoid

static const float nnnv_PI = 3.1415926535;
static const float HALF_PI = nnnv_PI / 2;
static const float QUARTER_PI = nnnv_PI / 4;

struct nnnv_Vec2 { float x, y; };
struct nnnv_Mat2 { float m00, m01, m10, m11; };

static inline nnnv_Vec2 nnnv_vec2(float f) { return { f, f }; }
static inline nnnv_Vec2 nnnv_vec2(float x, float y) { return { x, y }; }

static inline nnnv_Vec2 operator-(nnnv_Vec2 v) { return { -v.x, -v.y }; }
static inline nnnv_Vec2 operator-(nnnv_Vec2 l, nnnv_Vec2 r) { return { l.x - r.x, l.y - r.y }; }
static inline nnnv_Vec2 operator+(nnnv_Vec2 l, nnnv_Vec2 r) { return { l.x + r.x, l.y + r.y }; }
static inline nnnv_Vec2 operator*(float f, nnnv_Vec2  v) { return { f * v.x, f * v.y }; }
static inline nnnv_Vec2 operator*(nnnv_Vec2  v, float f) { return { f * v.x, f * v.y }; }
static inline nnnv_Vec2 operator*(nnnv_Vec2  l, nnnv_Vec2  r) { return { l.x * r.x, l.y * r.y }; }
static inline nnnv_Vec2 operator/(nnnv_Vec2  v, float f) { return { v.x / f, v.y / f }; }
static inline nnnv_Vec2 operator/(nnnv_Vec2  l, nnnv_Vec2  r) { return { l.x / r.x, l.y / r.y }; }

static inline nnnv_Vec2 nor(nnnv_Vec2 v) {
    float f = 1 / sqrtf(v.x * v.x + v.y * v.y);
    return { v.x * f, v.y * f };
}

static inline float dot(nnnv_Vec2 l, nnnv_Vec2 r) {
    return l.x * r.x + l.y * r.y;
}

static inline float cross(nnnv_Vec2 l, nnnv_Vec2 r) {
    return l.x * r.y - l.y * r.x;
}

static inline nnnv_Vec2 operator*(nnnv_Mat2 m, nnnv_Vec2 v) {
    return { m.m00 * v.x + m.m01 * v.y,
             m.m10 * v.x + m.m11 * v.y, };
}

typedef struct Color { uint8_t r, g, b, a; };

//draws a triangle, or adds one to whatever mesh we are building


struct LineRenderer {
    enum Cap { CAP_SQUARE, CAP_PROJECT, CAP_ROUND };
    enum Join { JOIN_MITER, JOIN_BEVEL, JOIN_ROUND };
    Cap cap;
    Join join;
    float r; //line width (radius)
    Color strokeColor;

    //line drawing state
    int lineVertexCount;
    float fx, fy; //first vertex
    float sx, sy, sdx, sdy; //second vertex
    float px, py, pdx, pdy; //previous vertex
    float lx, ly; //last vertex

    void triangle(float x1, float y1, float x2, float y2, float x3, float y3, Color fill)
    {
        ::triangle(renderer(), 0, {x1, y1}, {x2, y2}, {x3, y3});
    }

    //returns roughly the number of triangles needed for a triangle fan of a given arc length
    //        to look perfectly circular at a given size on screen
    int circleDetail(float radius, float delta) {
        return fminf(11, sqrtf(radius / 4)) / QUARTER_PI * fabsf(delta) + 1;
    }

    void arcJoin(float x, float y, float dx1, float dy1, float dx2, float dy2) {
        nnnv_Vec2 a = nnnv_vec2(dx1, dy1), b = nnnv_vec2(dx2, dy2);
        float theta = atan2f(cross(a, b), dot(a, b));
        int segments = circleDetail(r, theta);
        float px = x + dx1, py = y + dy1;
        if (segments > 1) {
            float c = cosf(theta / segments);
            float s = sinf(theta / segments);
            nnnv_Mat2 rot = { c, -s, s, c };
            for (int i = 1; i < segments; ++i) {
                a = rot * a;
                float nx = x + a.x;
                float ny = y + a.y;
                triangle(x, y, px, py, nx, ny, strokeColor);
                px = nx;
                py = ny;
            }
        }
        triangle(x, y, px, py, x + dx2, y + dy2, strokeColor);
    }

    void lineCap(float x, float y, float dx, float dy) {
        int segments = circleDetail(r, HALF_PI);
        nnnv_Vec2 p = nnnv_vec2(dy, -dx);
        if (segments > 1) {
            float c = cosf(HALF_PI / segments);
            float s = sinf(HALF_PI / segments);
            nnnv_Mat2 rot = { c, -s, s, c };
            for (int i = 1; i < segments; ++i) {
                nnnv_Vec2 n = rot * p;
                triangle(x, y, x + p.x, y + p.y, x + n.x, y + n.y, strokeColor);
                triangle(x, y, x - p.y, y + p.x, x - n.y, y + n.x, strokeColor);
                p = n;
            }
        }
        triangle(x, y, x + p.x, y + p.y, x + dx, y + dy, strokeColor);
        triangle(x, y, x - p.y, y + p.x, x - dy, y + dx, strokeColor);
    }

    void beginLine() {
        lineVertexCount = 0;
    }

    void lineVertex(float x, float y) {
        //disallow adding consecutive duplicate totalVerts,
        //as it is pointless and just creates an extra edge case
        if (lineVertexCount > 0 && x == lx && y == ly) {
            return;
        }

        if (lineVertexCount == 0) {
            fx = x;
            fy = y;
        } else if (lineVertexCount == 1) {
            sx = x;
            sy = y;
        } else {
            nnnv_Vec2 p = nnnv_vec2(px, py);
            nnnv_Vec2 l = nnnv_vec2(lx, ly);
            nnnv_Vec2 v = nnnv_vec2(x, y);

            float cosPiOver15 = 0.97815f;
            nnnv_Vec2 leg1 = nor(l - p);
            nnnv_Vec2 leg2 = nor(v - l);
            if (join == JOIN_BEVEL || join == JOIN_ROUND || dot(leg1, -leg2) > cosPiOver15 || dot(leg1, -leg2) < -0.999) {
                float tx =  leg1.y * r;
                float ty = -leg1.x * r;

                if (lineVertexCount == 2) {
                    sdx = tx;
                    sdy = ty;
                } else {
                    triangle(px - pdx, py - pdy, px + pdx, py + pdy, lx - tx, ly - ty, strokeColor);
                    triangle(px + pdx, py + pdy, lx - tx, ly - ty, lx + tx, ly + ty, strokeColor);
                }

                float nx =  leg2.y * r;
                float ny = -leg2.x * r;

                if (join == JOIN_ROUND) {
                    if (cross(leg1, leg2) > 0) {
                        arcJoin(lx, ly, tx, ty, nx, ny);
                    } else {
                        arcJoin(lx, ly, -tx, -ty, -nx, -ny);
                    }
                } else if (cross(leg1, leg2) > 0) {
                    triangle(lx, ly, lx + tx, ly + ty, lx + nx, ly + ny, strokeColor);
                } else {
                    triangle(lx, ly, lx - tx, ly - ty, lx - nx, ly - ny, strokeColor);
                }

                pdx = nx;
                pdy = ny;
            } else {
                nnnv_Vec2 a = leg2 - leg1;
                nnnv_Vec2 b = nnnv_vec2(leg1.y, -leg1.x);
                nnnv_Vec2 c = a * (r / dot(a, b));

                float bx = c.x, by = c.y;

                if (lineVertexCount == 2) {
                    sdx = bx;
                    sdy = by;
                } else {
                    triangle(px - pdx, py - pdy, px + pdx, py + pdy, lx - bx, ly - by, strokeColor);
                    triangle(px + pdx, py + pdy, lx - bx, ly - by, lx + bx, ly + by, strokeColor);
                }

                pdx = bx;
                pdy = by;
            }
        }

        px = lx;
        py = ly;
        lx = x;
        ly = y;

        lineVertexCount += 1;
    }

    void endLine(bool close) {
        if (lineVertexCount < 3) {
            return;
        }

        if (close) {
            //draw the last two legs
            lineVertex(fx, fy);
            lineVertex(sx, sy);

            //connect first and second vertices
            triangle(px - pdx, py - pdy, px + pdx, py + pdy, sx - sdx, sy - sdy, strokeColor);
            triangle(px + pdx, py + pdy, sx - sdx, sy - sdy, sx + sdx, sy + sdy, strokeColor);
        } else {
            //draw last line (with cap)
            float dx = lx - px;
            float dy = ly - py;
            float d = sqrtf(dx*dx + dy*dy);
            float tx =  dy / d * r;
            float ty = -dx / d * r;

            if (cap == CAP_PROJECT) {
                lx -= ty;
                ly += tx;
            }

            triangle(px - pdx, py - pdy, px + pdx, py + pdy, lx - tx, ly - ty, strokeColor);
            triangle(px + pdx, py + pdy, lx - tx, ly - ty, lx + tx, ly + ty, strokeColor);

            if (cap == CAP_ROUND) {
                lineCap(lx, ly, -ty, tx);
            }

            //draw first line (with cap)
            dx = fx - sx;
            dy = fy - sy;
            d = sqrtf(dx*dx + dy*dy);
            tx =  dy / d * r;
            ty = -dx / d * r;

            if (cap == CAP_PROJECT) {
                fx -= ty;
                fy += tx;
            }

            triangle(sx - sdx, sy - sdy, sx + sdx, sy + sdy, fx + tx, fy + ty, strokeColor);
            triangle(sx + sdx, sy + sdy, fx + tx, fy + ty, fx - tx, fy - ty, strokeColor);

            if (cap == CAP_ROUND) {
                lineCap(fx, fy, -ty, tx);
            }
        }
    }
};

}
