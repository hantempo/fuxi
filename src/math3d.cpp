#include "math3d.h"

/* 
 * Simulates desktop's glRotatef. The matrix is returned in column-major 
 * order. 
 */
void rotate_matrix(double angle, double x, double y, double z, float *R) {
    double radians, c, s, c1, u[3], length;
    int i, j;

    radians = (angle * M_PI) / 180.0;

    c = cos(radians);
    s = sin(radians);

    c1 = 1.0 - cos(radians);

    length = sqrt(x * x + y * y + z * z);

    u[0] = x / length;
    u[1] = y / length;
    u[2] = z / length;

    for (i = 0; i < 16; i++) {
        R[i] = 0.0;
    }

    R[15] = 1.0;

    for (i = 0; i < 3; i++) {
        R[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
        R[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            R[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
        }
    }
}

/* 
 * Simulates gluPerspectiveMatrix 
 */
void perspective_matrix(double fovy, double aspect, double znear, double zfar, float *P) {
    int i;
    double f;

    f = 1.0/tan(fovy * 0.5);

    for (i = 0; i < 16; i++) {
        P[i] = 0.0;
    }

    P[0] = f / aspect;
    P[5] = f;
    P[10] = (znear + zfar) / (znear - zfar);
    P[11] = -1.0;
    P[14] = (2.0 * znear * zfar) / (znear - zfar);
    P[15] = 0.0;
}

/* 
 * Multiplies A by B and writes out to C. All matrices are 4x4 and column
 * major. In-place multiplication is supported.
 */
void multiply_matrix(float *A, float *B, float *C) {
    int i, j, k;
    float aTmp[16];

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            aTmp[j * 4 + i] = 0.0;

            for (k = 0; k < 4; k++) {
                aTmp[j * 4 + i] += A[k * 4 + i] * B[j * 4 + k];
            }
        }
    }

    for (i = 0; i < 16; i++) {
        C[i] = aTmp[i];
    }
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
    Matrix4x4    r;

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
