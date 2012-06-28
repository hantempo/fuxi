#ifndef _INCLUDE_MATH3D_
#define _INCLUDE_MATH3D_

#include "common.h"

void rotate_matrix(double angle, double x, double y, double z, float *R);
void perspective_matrix(double fovy, double aspect, double znear, double zfar, float *P);
void multiply_matrix(float *A, float *B, float *C);

namespace Math
{
    const float PI = 3.14159265358979323846f;
    const float Rad2Deg = 180.0f / PI;
    const float Deg2Rad = PI / 180.0f;

    inline float Rad(float deg)
    {
        return deg * Deg2Rad;
    }

    inline float Deg(float rad)
    {
        return rad * Rad2Deg;
    }
};

class Vector3;
class Vector4D;

class Matrix4x4
{
public:
    union
    {
        float v[16];
        struct
        {
            float v11, v12, v13, v14,
                  v21, v22, v23, v24,
                  v31, v32, v33, v34,
                  v41, v42, v43, v44;
        };
    };

    Matrix4x4(float m11 = 1, float m12 = 0, float m13 = 0, float m14 = 0, 
              float m21 = 0, float m22 = 1, float m23 = 0, float m24 = 0, 
              float m31 = 0, float m32 = 0, float m33 = 1, float m34 = 0, 
              float m41 = 0, float m42 = 0, float m43 = 0, float m44 = 1);
    explicit Matrix4x4(const float v[]);

    Matrix4x4 & operator*=(const Matrix4x4 &M);
    Matrix4x4 & operator+=(const Matrix4x4 &M);

    operator const float *() const { return v; }

    static Matrix4x4 Translate(const Vector3 &v);
    static Matrix4x4 Rotate(const Vector3 &axis, float angle);
    static Matrix4x4 Perspective(float fovy, float aspect, float zNear, float zFar);

    static float Determinant3x3(const Matrix4x4 &M);
    static Matrix4x4 Invert4x3(const Matrix4x4 &M);
};

class Vector3
{
public:
    union
    {
        float v[3];
        struct
        {
            float x, y, z;
        };
    };

    Vector3();
    Vector3(float nx, float ny, float nz);
    explicit Vector3(const float v[]);
    Vector3(const Vector3 &V);
};

//////////////////////////////////////////////////////////////////
// Implementation of Matrix4x4
//////////////////////////////////////////////////////////////////

inline Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14,
                            float m21, float m22, float m23, float m24,
                            float m31, float m32, float m33, float m34,
                            float m41, float m42, float m43, float m44) :
    v11(m11), v12(m12), v13(m13), v14(m14),
    v21(m21), v22(m22), v23(m23), v24(m24),
    v31(m31), v32(m32), v33(m33), v34(m34),
    v41(m41), v42(m42), v43(m43), v44(m44)
{
}

inline Matrix4x4::Matrix4x4(const float vv[])
{
    memcpy(v, vv, sizeof(float) * 16);
}

inline Matrix4x4 operator*(const Matrix4x4 &A, const Matrix4x4 &B)
{
    return Matrix4x4
    (
        A.v11*B.v11 + A.v12*B.v21 + A.v13*B.v31 + A.v14*B.v41,
        A.v11*B.v12 + A.v12*B.v22 + A.v13*B.v32 + A.v14*B.v42,
        A.v11*B.v13 + A.v12*B.v23 + A.v13*B.v33 + A.v14*B.v43,
        A.v11*B.v14 + A.v12*B.v24 + A.v13*B.v34 + A.v14*B.v44,

        A.v21*B.v11 + A.v22*B.v21 + A.v23*B.v31 + A.v24*B.v41,
        A.v21*B.v12 + A.v22*B.v22 + A.v23*B.v32 + A.v24*B.v42,
        A.v21*B.v13 + A.v22*B.v23 + A.v23*B.v33 + A.v24*B.v43,
        A.v21*B.v14 + A.v22*B.v24 + A.v23*B.v34 + A.v24*B.v44,

        A.v31*B.v11 + A.v32*B.v21 + A.v33*B.v31 + A.v34*B.v41,
        A.v31*B.v12 + A.v32*B.v22 + A.v33*B.v32 + A.v34*B.v42,
        A.v31*B.v13 + A.v32*B.v23 + A.v33*B.v33 + A.v34*B.v43,
        A.v31*B.v14 + A.v32*B.v24 + A.v33*B.v34 + A.v34*B.v44,

        A.v41*B.v11 + A.v42*B.v21 + A.v43*B.v31 + A.v44*B.v41,
        A.v41*B.v12 + A.v42*B.v22 + A.v43*B.v32 + A.v44*B.v42,
        A.v41*B.v13 + A.v42*B.v23 + A.v43*B.v33 + A.v44*B.v43,
        A.v41*B.v14 + A.v42*B.v24 + A.v43*B.v34 + A.v44*B.v44
    );
}

inline Matrix4x4 Matrix4x4::Translate(const Vector3 &v)
{
    Matrix4x4 M;
    M.v41 = v.x;
    M.v42 = v.y;
    M.v43 = v.z;
    return M;
}

inline Matrix4x4 Matrix4x4::Rotate(const Vector3 &axis, float angle)
{
    const float rad = Math::Rad(angle);
    const float co = cos(rad);
    const float si = sin(rad);
    const float nx = axis.x;
    const float ny = axis.y;
    const float nz = axis.z;

    Matrix4x4 M;

    M.v11 = nx*nx*(1.0f - co) + co;
    M.v12 = (1.0f - co)*nx*ny + si*nz;
    M.v13 = (1.0f - co)*nx*nz - si*ny;

    M.v21 = (1.0f - co)*nx*ny - si*nz;
    M.v22 = ny*ny*(1.0f - co) + co;
    M.v23 = (1.0f - co)*ny*nz + si*nx;

    M.v31 = (1.0f - co)*nx*nz + si*ny;
    M.v32 = (1.0f - co)*ny*nz - si*nx;
    M.v33 = nz*nz*(1.0f - co) + co;

    return M;
}

//////////////////////////////////////////////////////////////////
// Implementation of Vector3
//////////////////////////////////////////////////////////////////

inline Vector3::Vector3() : x(0), y(0), z(0)
{
}

inline Vector3::Vector3(float nx, float ny, float nz) :
    x(nx), y(ny), z(nz)
{
}

inline Vector3::Vector3(const Vector3 &V)
{
    memcpy(v, V.v, sizeof(float) * 3);
}

#endif // _INCLUDE_MATH3D_
