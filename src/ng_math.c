#include "ng_math.h"
#include <math.h>

// Nonstandard?
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif


// Vector 2 functions
nvec2 ng_addv2(nvec2 a, nvec2 b)
{
    return (nvec2){a.x+b.x,a.y+b.y};
}
nvec2 ng_subv2(nvec2 a, nvec2 b)
{
    return (nvec2){a.x-b.x,a.y-b.y};
}
nvec2 ng_scalev2(nvec2 a, float b)
{
    return (nvec2){a.x*b,a.y*b};
}
nvec2 ng_normalizev2(nvec2 a)
{
    float mag = sqrtf(a.x*a.x + a.y*a.y);
    return (nvec2){a.x / mag, a.y / mag};
}
float ng_dotv2(nvec2 a, nvec2 b)
{
    return a.x*b.x + a.x*b.x;
}
float ng_magv2(nvec2 a)
{
    return sqrtf(a.x*a.x + a.y*a.y);
}
nvec2 ng_rotatev2(nvec2 a, float angle)
{
    nvec2 b;
    b.x =  cosf(angle) * a.x + sinf(angle) * a.y;
    b.y = -sinf(angle) * a.x + cosf(angle) * a.y;
    return b;
}

// Pointer versions
float ng_dotv2p(nvec2* a, nvec2* b)
{
    return a->x*b->x + a->y*b->y;
}



// Vector 3 functions
nvec3 ng_crossv3(nvec3 a, nvec3 b)
{
    return (nvec3){a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}
nvec3 ng_addv3(nvec3 a, nvec3 b)
{
    return (nvec3){a.x+b.x,a.y+b.y,a.z+b.z};
}
nvec3 ng_subv3(nvec3 a, nvec3 b)
{
    return (nvec3){a.x-b.x,a.y-b.y,a.z-b.z};
}
nvec3 ng_scalev3(nvec3 a, float b)
{
    return (nvec3){a.x*b,a.y*b,a.z*b};
}
nvec3 ng_normalizev3(nvec3 a)
{
    float mag = sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
    return (nvec3){a.x / mag, a.y / mag, a.z / mag};
}
float ng_dotv3(nvec3 a, nvec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
float ng_magv3(nvec3 a)
{
    return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}

// Pointer versions
float ng_dotv3p(nvec3* a, nvec3* b)
{
    return a->x*b->x + a->y*b->y + a->z*b->z;
}

// Vec4 functions
nvec4 ng_addv4(nvec4 a, nvec4 b)
{
    return (nvec4){a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w};
}
nvec4 ng_subv4(nvec4 a, nvec4 b)
{
    return (nvec4){a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w};
}
nvec4 ng_scalev4(nvec4 a, float b)
{
    return (nvec4){a.x*b,a.y*b,a.z*b,a.w*b};
}
nvec4 ng_normalizev4(nvec4 a)
{
    float mag = sqrtf(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
    return (nvec4){a.x / mag, a.y / mag, a.z / mag, a.w / mag};
}
float ng_dotv4(nvec4 a, nvec4 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
float ng_magv4(nvec4 a)
{
    return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

// Pointer versions
float ng_dotv4p(nvec4* a, nvec4* b)
{
    return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}


// Matrix operations

// Matrix 2x2 //
void ng_identity2x2(nmat2x2* out)
{
    *out = (nmat2x2)
    {{1.0f, 0.0f},
     {0.0f, 1.0f}};
}
void ng_mul2x2(nmat2x2* out, nmat2x2* l, nmat2x2* r)
{
    // Unrolled
    nmat2x2 m;
    nvec2 row;

    row = (nvec2){ l->a.x, l->b.x };
    m.a.x = ng_dotv2p(&row, &r->a);
    m.b.x = ng_dotv2p(&row, &r->b);

    row = (nvec2){ l->a.y, l->b.y };
    m.a.y = ng_dotv2p(&row, &r->a);
    m.b.y = ng_dotv2p(&row, &r->b);

    *out = m;
}
void ng_mulvec2x2(nvec2* out, nmat2x2* left, nvec2* right)
{
    // Unrolled
    nvec2 v;
    v = ng_scalev2(left->a, right->x);
    v = ng_addv2(v, ng_scalev2(left->b, right->y));

    *out = v;
}
void ng_scale2x2(nmat2x2* out, nmat2x2* nmat, nvec2* scale)
{
    out->a = (nvec2){ nmat->a.x * scale->x, nmat->a.y * scale->x };
    out->b = (nvec2){ nmat->b.x * scale->y, nmat->b.y * scale->y };
}
void ng_rotation2x2(nmat2x2* out, float angle)
{
    *out = (nmat2x2)
    {{cosf(angle), -sinf(angle)},
     {sinf(angle),  cosf(angle)}};
}

// Matrix 2x3 //
void ng_identity2x3(nmat2x3* out)
{
    *out = (nmat2x3)
    {{1.0f, 0.0f},
     {0.0f, 1.0f}};
}
void ng_mul2x3(nmat2x3* out, nmat2x2* l, nmat2x3* r)
{
    // Unrolled
    nmat2x3 m;
    nvec2 row;

    row = (nvec2){ l->a.x, l->b.x };
    m.a.x = ng_dotv2p(&row, &r->a);
    m.b.x = ng_dotv2p(&row, &r->b);
    m.c.x = ng_dotv2p(&row, &r->c);

    row = (nvec2){ l->a.y, l->b.y };
    m.a.y = ng_dotv2p(&row, &r->a);
    m.b.y = ng_dotv2p(&row, &r->b);
    m.c.y = ng_dotv2p(&row, &r->c);

    *out = m;
}
void ng_mulvec2x3(nvec2* out, nmat2x3* left, nvec3* right)
{
    // Unrolled
    nvec2 v;
    v = ng_scalev2(left->a, right->x);
    v = ng_addv2(v, ng_scalev2(left->b, right->y));
    v = ng_addv2(v, ng_scalev2(left->c, right->z));

    *out = v;
}
void ng_scale2x3(nmat2x3* out, nmat2x3* nmat, nvec3* scale)
{
    out->a = (nvec2){ nmat->a.x * scale->x, nmat->a.y * scale->x };
    out->b = (nvec2){ nmat->b.x * scale->y, nmat->b.y * scale->y };
    out->c = (nvec2){ nmat->b.x * scale->z, nmat->b.y * scale->z };
}
void ng_rotation2x3(nmat2x3* out, float angle)
{
    *out = (nmat2x3)
    {{cosf(angle), -sinf(angle)},
     {sinf(angle),  cosf(angle)},
     {       0.0f,        0.0f}};
}

// Matrix 3x3 //
void ng_identity3x3(nmat3x3* out)
{
    *out = (nmat3x3)
    {{1.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 1.0f}};
}
void ng_mul3x3(nmat3x3* out, nmat3x3* l, nmat3x3* r)
{
    // Unrolled
    nmat3x3 m;
    nvec3 row;

    row = (nvec3){ l->a.x, l->b.x, l->c.x };
    m.a.x = ng_dotv3p(&row, &r->a);
    m.b.x = ng_dotv3p(&row, &r->b);
    m.c.x = ng_dotv3p(&row, &r->c);

    row = (nvec3){ l->a.y, l->b.y, l->c.y };
    m.a.y = ng_dotv3p(&row, &r->a);
    m.b.y = ng_dotv3p(&row, &r->b);
    m.c.y = ng_dotv3p(&row, &r->c);

    row = (nvec3){ l->a.z, l->b.z, l->c.z };
    m.a.z = ng_dotv3p(&row, &r->a);
    m.b.z = ng_dotv3p(&row, &r->b);
    m.c.z = ng_dotv3p(&row, &r->c);

    *out = m;
}
void ng_mulvec3x3(nvec3* out, nmat3x3* left, nvec3* right)
{
    // Unrolled
    nvec3 v;
    v = ng_scalev3(left->a, right->x);
    v = ng_addv3(v, ng_scalev3(left->b, right->y));
    v = ng_addv3(v, ng_scalev3(left->c, right->z));

    *out = v;
}
void ng_scale3x3(nmat3x3* out, nmat3x3* nmat, nvec3* scale)
{
    out->a = (nvec3){ nmat->a.x * scale->x, nmat->a.y * scale->x, nmat->a.z * scale->x };
    out->b = (nvec3){ nmat->b.x * scale->y, nmat->b.y * scale->y, nmat->b.z * scale->y };
    out->c = (nvec3){ nmat->c.x * scale->z, nmat->c.y * scale->z, nmat->c.z * scale->z };
}
void ng_xRotation3x3(nmat3x3* out, float angle)
{
    *out = (nmat3x3)
    {{1.0f,        0.0f,         0.0f},
     {0.0f, cosf(angle), -sinf(angle)},
     {0.0f, sinf(angle),  cosf(angle)}};
}
void ng_yRotation3x3(nmat3x3* out, float angle)
{
    *out = (nmat3x3)
    {{ cosf(angle), 0.0f, sinf(angle)},
     {        0.0f, 1.0f,        0.0f},
     {-sinf(angle), 0.0f, cosf(angle)}};
}
void ng_zRotation3x3(nmat3x3* out, float angle)
{
    *out = (nmat3x3)
    {{cosf(angle), -sinf(angle), 0.0f},
     {sinf(angle),  cosf(angle), 0.0f},
     {       0.0f,         0.0f, 1.0f}};
}
void ng_xyzRotation3x3(nmat3x3* out, float x, float y, float z)
{
    // TODO: Optimize this down into one big nmatrix 
    nmat3x3 nmat, rot;
    ng_zRotation3x3(&nmat, z); // Roll
    ng_xRotation3x3(&rot, x); // Pitch
    ng_mul3x3(&nmat, &rot, &nmat);
    ng_yRotation3x3(&rot, y); // Yaw
    ng_mul3x3(&nmat, &rot, &nmat);
    *out = nmat;
}


// Matrix 4x4 //
void ng_identity4x4(nmat4x4* out)
{
    *out = (nmat4x4)
    {{1.0f, 0.0f, 0.0f, 0.0f},
     {0.0f, 1.0f, 0.0f, 0.0f},
     {0.0f, 0.0f, 1.0f, 0.0f},
     {0.0f, 0.0f, 0.0f, 1.0f}};
}
void ng_mul4x4(nmat4x4* out, nmat4x4* l, nmat4x4* r)
{
    // Unrolled
    nmat4x4 m;
    nvec4 row;

    row = (nvec4){ l->a.x, l->b.x, l->c.x, l->d.x };
    m.a.x = ng_dotv4p(&row, &r->a);
    m.b.x = ng_dotv4p(&row, &r->b);
    m.c.x = ng_dotv4p(&row, &r->c);
    m.d.x = ng_dotv4p(&row, &r->d);

    row = (nvec4){ l->a.y, l->b.y, l->c.y, l->d.y };
    m.a.y = ng_dotv4p(&row, &r->a);
    m.b.y = ng_dotv4p(&row, &r->b);
    m.c.y = ng_dotv4p(&row, &r->c);
    m.d.y = ng_dotv4p(&row, &r->d);

    row = (nvec4){ l->a.z, l->b.z, l->c.z, l->d.z };
    m.a.z = ng_dotv4p(&row, &r->a);
    m.b.z = ng_dotv4p(&row, &r->b);
    m.c.z = ng_dotv4p(&row, &r->c);
    m.d.z = ng_dotv4p(&row, &r->d);

    row = (nvec4){ l->a.w, l->b.w, l->c.w, l->d.w };
    m.a.w = ng_dotv4p(&row, &r->a);
    m.b.w = ng_dotv4p(&row, &r->b);
    m.c.w = ng_dotv4p(&row, &r->c);
    m.d.w = ng_dotv4p(&row, &r->d);

    *out = m;
}
void ng_mulvec4x4(nvec4* out, nmat4x4* left, nvec4* right)
{
    // Unrolled
    nvec4 v;
    v = ng_scalev4(left->a, right->x);
    v = ng_addv4(v, ng_scalev4(left->b, right->y));
    v = ng_addv4(v, ng_scalev4(left->c, right->z));
    v = ng_addv4(v, ng_scalev4(left->d, right->w));

    *out = v;
}
void ng_scale4x4(nmat4x4* out, nmat4x4* nmat, nvec4* scale)
{
    out->a = (nvec4){ nmat->a.x * scale->x, nmat->a.y * scale->x, nmat->a.z * scale->x, nmat->a.w * scale->x };
    out->b = (nvec4){ nmat->b.x * scale->y, nmat->b.y * scale->y, nmat->b.z * scale->y, nmat->b.w * scale->y };
    out->c = (nvec4){ nmat->c.x * scale->z, nmat->c.y * scale->z, nmat->c.z * scale->z, nmat->c.w * scale->z };
    out->d = (nvec4){ nmat->d.x * scale->w, nmat->d.y * scale->w, nmat->d.z * scale->w, nmat->d.w * scale->w };
}
void ng_xRotation4x4(nmat4x4* out, float angle)
{
    *out = (nmat4x4)
    {{1.0f,        0.0f,         0.0f, 0.0f},
     {0.0f, cosf(angle), -sinf(angle), 0.0f},
     {0.0f, sinf(angle),  cosf(angle), 0.0f},
     {0.0f,        0.0f,         0.0f, 1.0f}};
}
void ng_yRotation4x4(nmat4x4* out, float angle)
{
    *out = (nmat4x4)
    {{ cosf(angle), 0.0f, sinf(angle), 0.0f},
     {        0.0f, 1.0f,        0.0f, 0.0f},
     {-sinf(angle), 0.0f, cosf(angle), 0.0f},
     {        0.0f, 0.0f,        0.0f, 1.0f}};
}
void ng_zRotation4x4(nmat4x4* out, float angle)
{
    *out = (nmat4x4)
    {{cosf(angle), -sinf(angle), 0.0f, 0.0f},
     {sinf(angle),  cosf(angle), 0.0f, 0.0f},
     {       0.0f,         0.0f, 1.0f, 0.0f},
     {       0.0f,         0.0f, 0.0f, 1.0f} };
}
void ng_xyzRotation4x4(nmat4x4* out, float x, float y, float z)
{
    // TODO: Optimize this down into one big nmatrix 
    nmat4x4 nmat, rot;
    ng_zRotation4x4(&nmat, z); // Roll
    ng_xRotation4x4(&rot, x); // Pitch
    ng_mul4x4(&nmat, &rot, &nmat);
    ng_yRotation4x4(&rot, y); // Yaw
    ng_mul4x4(&nmat, &rot, &nmat);
    *out = nmat;
}



// Camera functions //
void ng_perspective4x4(nmat4x4* out, float fov, float near, float far, float aspect)
{
    float s = 1/tan((fov/2)*(M_PI/180));

    *out = (nmat4x4){
    {s * aspect, 0,                      0,  0},
    {         0, s,                      0,  0},
    {         0, 0,        -far/(far-near), -1},
    {         0, 0, -(far*near)/(far-near),  0}};
}
void ng_ortho4x4(nmat4x4* out, float l, float r, float t, float b, float near, float far)
{
	*out = (nmat4x4){
    {  2.0f/(r-l),            0,                      0, 0},
    {           0,   2.0f/(t-b),                      0, 0},
    {           0,            0,       -2.0f/(far-near), 0},
    {-(r+l)/(r-l), -(t+b)/(t-b), -(far+near)/(far-near), 1}};
}

