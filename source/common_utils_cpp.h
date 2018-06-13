#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

inline f64 dist2(glm::vec3 v, glm::vec3 w);
inline f64 dist_to_segment_squared(glm::vec3 v, glm::vec3 w, glm::vec3 p);
inline f64 dist_to_segment(glm::vec3 v, glm::vec3 w, glm::vec3 p);

// }
#endif


#ifdef COMMON_UTILS_IMPLEMENTATION_CPP


inline f64 dist2(glm::vec3 v, glm::vec3 w)
{
    f64 a = v.x - w.x;
    f64 b = v.y - w.y;
    return (a * a) + (b * b);
}

inline f64 dist_to_segment_squared(glm::vec3 v, glm::vec3 w, glm::vec3 p)
{ 
    const f64 l2 = dist2(v, w);
    if (l2 == 0.0) {
        return dist2(p, v);
    }

    const f64 t = glm::max(0.0, glm::min(1.0, dot(p - v, w - v) / l2));
    return dist2(p, glm::vec3(v.x + t * (w.x - v.x), v.y + t * (w.y - v.y), 0.0));
}

inline f64 dist_to_segment(glm::vec3 v, glm::vec3 w, glm::vec3 p)
{
    return glm::sqrt(dist_to_segment_squared(v, w, p));
}


#undef COMMON_UTILS_IMPLEMENTATION_CPP

#endif
