/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){

	Uniforms const uniformsHandle = gpu_getUniformsHandle(
		gpu);

	UniformLocation const viewMatrixLocation = getUniformLocation(
		gpu, 
		"viewMatrix");

	Mat4 const*const view = shader_interpretUniformAsMat4(
		uniformsHandle, 
		viewMatrixLocation);

	UniformLocation const projectionMatrixLocation = getUniformLocation(
		gpu, 
		"projectionMatrix");

	Mat4 const*const proj = shader_interpretUniformAsMat4(
		uniformsHandle,
		projectionMatrixLocation);


	Vec3 const*const position = vs_interpretInputVertexAttributeAsVec3(
		gpu,
		input,
		0);

	Vec3 const*const normal = vs_interpretInputVertexAttributeAsVec3(
		gpu,
		input,
		1);

	Mat4 mvp;
	multiply_Mat4_Mat4(&mvp, proj, view);

	Vec4 pos4;
	copy_Vec3Float_To_Vec4(&pos4, position, 1.f);

	multiply_Mat4_Vec4(
		&output->gl_Position,
		&mvp,
		&pos4);

	Vec3 *const posOut = vs_interpretOutputVertexAttributeAsVec3(
		gpu, //gpu
		output, //output vertex
		0);

	Vec3 *const normalOut = vs_interpretOutputVertexAttributeAsVec3(
		gpu, //gpu
		output, //output vertex
		1);

	init_Vec3(
		posOut,
		position->data[0],
		position->data[1],
		position->data[2]);

	init_Vec3(
		normalOut,
		normal->data[0],
		normal->data[1],
		normal->data[2]);
}

float clamp(float v, float lo, float hi) {
	if (v < hi) {
		return max(v, lo);
	}

	return min(v, hi);
}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){

	Uniforms const uniformsHandle = gpu_getUniformsHandle(
		gpu);

	UniformLocation const cameraPositionLocation = getUniformLocation(
		gpu,
		"cameraPosition");

	Vec3 const*const cam = shader_interpretUniformAsVec3(
		uniformsHandle,
		cameraPositionLocation);

	UniformLocation const lightPositionLocation = getUniformLocation(
		gpu,
		"lightPosition");

	Vec3 const*const light = shader_interpretUniformAsVec3(
		uniformsHandle,
		lightPositionLocation);


	Vec3 const*const vOut_p = fs_interpretInputAttributeAsVec3 (
		gpu,
		input,
		0);


	Vec3 const*const vOut_n = fs_interpretInputAttributeAsVec3 (
		gpu,
		input,
		1);

	Vec3 dM;
	init_Vec3(&dM, 0.f, 1.f, 0.f);

	Vec3 sM;
	init_Vec3(&sM, 1.f, 1.f, 1.f);

	float s = 40.f;

	Vec3 tmp;
	Vec3 tmp2;
	sub_Vec3(&tmp, light, vOut_p);

	Vec3 L;
	normalize_Vec3(&L, &tmp);

	Vec3 N;
	normalize_Vec3(&N, vOut_n);

	float dF = clamp(dot_Vec3(&N, &L), 0.f, 1.f);

	sub_Vec3(&tmp, cam, vOut_p);

	Vec3 V;
	normalize_Vec3(&V, &tmp);

	multiply_Vec3_Float(&tmp, &V, -1.f);

	Vec3 R;
	reflect(&R, &tmp, &N);
	normalize_Vec3(&R, &R);

	float sF = pow(clamp(dot_Vec3(&R, &L), 0.f, 1.f), s);

	float dL = 1.f;
	float sL = 0.5f;

	Vec3 final;
	multiply_Vec3_Float(&tmp, &dM, dF*dL);
	multiply_Vec3_Float(&tmp2, &sM, sF*sL);

	add_Vec3(&final, &tmp, &tmp2);

	output->color.data[0] = final.data[0];
	output->color.data[1] = final.data[1];
	output->color.data[2] = final.data[2];
	output->color.data[3] = 1.f;
}

/// @}
