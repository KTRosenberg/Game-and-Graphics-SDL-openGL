#ifndef MATH_HPP
#define MATH_HPP

#include "common_utils.h"

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_INTRINSICS 

#include <glm/glm.hpp>
#include <glm/gtc/type_aligned.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>

typedef glm::aligned_vec2  Vector2;
typedef glm::aligned_vec3  Vector3;
typedef glm::aligned_vec4  Vector4;
typedef glm::aligned_ivec2 IntVector2;
typedef glm::aligned_ivec3 IntVector3;
typedef glm::aligned_ivec4 IntVector4;
typedef glm::aligned_mat3  Matrix3;
typedef glm::aligned_mat4  Matrix4;
typedef glm::qua<float, glm::aligned_highp> Quaternion;

typedef Vector2    Vec2;
typedef Vector3    Vec3;
typedef Vector4    Vec4;
typedef IntVector2 iVec2;
typedef IntVector3 iVec3;
typedef IntVector4 iVec4;
typedef Matrix3    Mat3;
typedef Matrix4    Mat4;
typedef Quaternion Quat;

typedef Vector2    vec2;
typedef Vector3    vec3;
typedef Vector4    vec4;
typedef IntVector2 ivec2;
typedef IntVector3 ivec3;
typedef IntVector4 ivec4;
typedef Matrix3    mat3;
typedef Matrix4    mat4;
typedef Quaternion quat;

typedef Vector2    float2;
typedef Vector3    float3;
typedef Vector4    float4;
typedef IntVector2 int2;
typedef IntVector3 int3;
typedef IntVector4 int4;

// unaligned

typedef glm::vec2  Vector2_unaligned;
typedef glm::vec3  Vector3_unaligned;
typedef glm::vec4  Vector4_unaligned;
typedef glm::ivec2 IntVector2_unaligned;
typedef glm::ivec3 IntVector3_unaligned;
typedef glm::ivec4 IntVector4_unaligned;
typedef glm::mat3  Matrix3_unaligned;
typedef glm::mat4  Matrix4_unaligned;
typedef glm::qua<float, glm::highp> Quaternion_unaligned;

typedef Vector2_unaligned    Vec2_ua;
typedef Vector3_unaligned    Vec3_ua;
typedef Vector4_unaligned    Vec4_ua;
typedef IntVector2_unaligned iVec2_ua;
typedef IntVector3_unaligned iVec3_ua;
typedef IntVector4_unaligned iVec4_ua;
typedef Matrix3_unaligned    Mat3_ua;
typedef Matrix4_unaligned    Mat4_ua;
typedef Quaternion_unaligned Quat_ua;

typedef Vector2_unaligned    vec2_ua;
typedef Vector3_unaligned    vec3_ua;
typedef Vector4_unaligned    vec4_ua;
typedef IntVector2_unaligned ivec2_ua;
typedef IntVector3_unaligned ivec3_ua;
typedef IntVector4_unaligned ivec4_ua;
typedef Matrix3_unaligned    mat3_ua;
typedef Matrix4_unaligned    mat4_ua;
typedef Quaternion_unaligned quat_ua;

typedef Vector2_unaligned    float2_ua;
typedef Vector3_unaligned    float3_ua;
typedef Vector4_unaligned    float4_ua;
typedef IntVector2_unaligned int2_ua;
typedef IntVector3_unaligned int3_ua;
typedef IntVector4_unaligned int4_ua;

typedef glm::tvec4<u8> U8Vector4;
typedef U8Vector4 u8Vec4;
typedef U8Vector4 u8vec4;

typedef glm::tvec2<u16> U16Vector2;
typedef U16Vector2 u16Vec2;
typedef U16Vector2 u8vec2;

#endif