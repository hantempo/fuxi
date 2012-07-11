#include "math3d.h"

namespace Color
{

const Vector4 Black = Vector4(0.f, 0.f, 0.f, 1.f);
const Vector4 White = Vector4(1.f, 1.f, 1.f, 1.f);
const Vector4 Silver = Vector4(0.92f, 0.91f, 0.98f);

}

//////////////////////////////////////////////////////////////////
// Implementation of Matrix4x4
//////////////////////////////////////////////////////////////////

Matrix4x4 Matrix4x4::Perspective(float fovy, float aspect, float zNear, float zFar)
{
    const float slopey = 1 / (float)(tan(Math::Rad(fovy/2.0f)));
    Matrix4x4 M;
    M.v11 = slopey / aspect;
    M.v22 = slopey;
    M.v33 = (zNear + zFar) / (zNear - zFar);
    M.v34 = -1.0f;
    M.v43 = 2.0f * zNear * zFar / (zNear - zFar);
    M.v44 = 0.0f;
    return M;
}

Matrix4x4 Matrix4x4::LookAt(const Vector3 &eye, const Vector3 &center, const Vector3 &up)
{
    const Vector3 neg_z = Vector3::Normalize(center - eye);
    const Vector3 pos_x = Vector3::Normalize(Vector3::Cross(neg_z, up));
    const Vector3 pos_y = Vector3::Cross(pos_x, neg_z);
    return Matrix4x4(pos_x, pos_y, -neg_z, -eye);
}

float Matrix4x4::Determinant3x3(const Matrix4x4 &M)
{
    return M.v11 * (M.v22*M.v33 - M.v23*M.v32)
         + M.v12 * (M.v23*M.v31 - M.v21*M.v33)
         + M.v13 * (M.v21*M.v32 - M.v22*M.v31);
}

Matrix4x4 Matrix4x4::Invert4x3(const Matrix4x4 &m)
{
    const float det = Determinant3x3 (m);
    const float k1OverDet = 1.0f / det;
    Matrix4x4 r;

    r.v11 = (m.v22*m.v33 - m.v23*m.v32) * k1OverDet;
    r.v12 = (m.v13*m.v32 - m.v12*m.v33) * k1OverDet;
    r.v13 = (m.v12*m.v23 - m.v13*m.v22) * k1OverDet;

    r.v21 = (m.v23*m.v31 - m.v21*m.v33) * k1OverDet;
    r.v22 = (m.v11*m.v33 - m.v13*m.v31) * k1OverDet;
    r.v23 = (m.v13*m.v21 - m.v11*m.v23) * k1OverDet;

    r.v31 = (m.v21*m.v32 - m.v22*m.v31) * k1OverDet;
    r.v32 = (m.v12*m.v31 - m.v11*m.v32) * k1OverDet;
    r.v33 = (m.v11*m.v22 - m.v12*m.v21) * k1OverDet;

    r.v41 = -(m.v41*r.v11 + m.v42*r.v21 + m.v43*r.v31);
    r.v42 = -(m.v41*r.v12 + m.v42*r.v22 + m.v43*r.v32);
    r.v43 = -(m.v41*r.v13 + m.v42*r.v23 + m.v43*r.v33);
    r.v44 = 1.0f;

    return r;
}

Matrix4x4 Matrix4x4::Transpose(const Matrix4x4 &m)
{
    Matrix4x4 M = m;

    Math::swap(M.v12, M.v21);
    Math::swap(M.v13, M.v31);
    Math::swap(M.v14, M.v41);
    Math::swap(M.v24, M.v42);
    Math::swap(M.v34, M.v43);
    Math::swap(M.v23, M.v32);

    return M;
}
