#pragma once

// Matrices are in Column-major!


// Vectors //
typedef struct nvec2 {
	float x, y;
} nvec2;
typedef struct nvec2i {
	int x, y;
} nvec2i;
typedef struct nvec3 {
	float x, y, z;
} nvec3;
typedef struct nvec4 {
	float x, y, z, w;
} nvec4;

// Matrices //
typedef struct nmat2x2 {
	nvec2 a, b;
} nmat2x2;
typedef struct nmat2x3 {
	nvec2 a, b, c;
} nmat2x3;
typedef struct nmat3x3 {
    nvec3 a, b, c;
} nmat3x3;
typedef struct nmat4x4 {
    nvec4 a, b, c, d;
} nmat4x4;


// Vector 2 //
nvec2 ng_addv2(nvec2 a, nvec2 b);
nvec2 ng_subv2(nvec2 a, nvec2 b);
nvec2 ng_scalev2(nvec2 a, float b);
nvec2 ng_normalizev2(nvec2 a);
float ng_dotv2(nvec2 a, nvec2 b);
float ng_magv2(nvec2 a);

nvec2 ng_rotatev2(nvec2 a, float angle);

// Vector 3 //
nvec3 ng_addv3(nvec3 a, nvec3 b);
nvec3 ng_subv3(nvec3 a, nvec3 b);
nvec3 ng_scalev3(nvec3 a, float b);
nvec3 ng_normalizev3(nvec3 a);
float ng_dotv3(nvec3 a, nvec3 b);
float ng_magv3(nvec3 a);

nvec3 ng_crossv3(nvec3 a, nvec3 b);

// Vector 4 //
nvec4 ng_addv4(nvec4 a, nvec4 b);
nvec4 ng_subv4(nvec4 a, nvec4 b);
nvec4 ng_scalev4(nvec4 a, float b);
nvec4 ng_normalizev4(nvec4 a);
float ng_dotv4(nvec4 a, nvec4 b);
float ng_magv4(nvec4 a);


// Matrix 2x2 //
void ng_identity2x2(nmat2x2* out);
void ng_mul2x2(nmat2x2* out, nmat2x2* left, nmat2x2* right);
void ng_mulvec2x2(nvec2* out, nmat2x2* left, nvec2* right);
void ng_scale2x2(nmat2x2* out, nmat2x2* nmat, nvec2* scale);

void ng_rotation2x2(nmat2x2* out, float angle);

// Matrix 2x3 //
void ng_identity2x3(nmat2x3* out);
void ng_mul2x3(nmat2x3* out, nmat2x2* left, nmat2x3* right); // Left is 2x2!!
void ng_mulvec2x3(nvec2* out, nmat2x3* left, nvec3* right);
void ng_scale2x3(nmat2x3* out, nmat2x3* nmat, nvec3* scale);

void ng_rotation2x3(nmat2x3* out, float angle);

// Matrix 3x3 //
void ng_identity3x3(nmat3x3* out);
void ng_mul3x3(nmat3x3* out, nmat3x3* left, nmat3x3* right);
void ng_mulvec3x3(nvec3* out, nmat3x3* left, nvec3* right);
void ng_scale3x3(nmat3x3* out, nmat3x3* nmat, nvec3* scale);

void ng_xRotation3x3(nmat3x3* out, float angle);
void ng_yRotation3x3(nmat3x3* out, float angle);
void ng_zRotation3x3(nmat3x3* out, float angle);
void ng_xyzRotation3x3(nmat3x3* out, float x, float y, float z);


// Matrix 4x4 //
void ng_identity4x4(nmat4x4* out);
void ng_mul4x4(nmat4x4* out, nmat4x4* left, nmat4x4* right);
void ng_mulvec4x4(nvec4* out, nmat4x4* left, nvec4* right);
void ng_scale4x4(nmat4x4* out, nmat4x4* nmat, nvec4* scale);

void ng_xRotation4x4(nmat4x4* out, float angle);
void ng_yRotation4x4(nmat4x4* out, float angle);
void ng_zRotation4x4(nmat4x4* out, float angle);
void ng_xyzRotation4x4(nmat4x4* out, float x, float y, float z);


// Camera functions //
void ng_perspective4x4(nmat4x4* out, float fov, float near, float far, float aspect);
void ng_ortho4x4(nmat4x4* out, float l, float r, float t, float b, float near, float far);
