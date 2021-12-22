#pragma once

#include "Engine/datatypes/include.hpp"
#include "buffers.hpp"
#include "shader.hpp"
#include "LightMatrixStructs.hpp"

#include "Engine/ECS/Registry.hpp"
#include "Engine/objects/components.hpp"

#define MAX_DIR_LIGHTS 3
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define DIRECTIONAL_LIGHTS_BIND 1
#define POINT_LIGHTS_BIND 2
#define SPOT_LIGHTS_BIND 3

//defines for shadows
#define Shadow_Width 2048U
#define Shadow_Height 2048U

#define SPOT_LIGHT_CAM 1.0f, 20.0f, 90.0f, 90.0f//default projectionMatrix for a spotLight
#define POINT_LIGHT_CAM 1.0f, 60.0f, 90.0f, 90.0f//default camera for a pointLight

namespace Engine
{
	class lightMatrixCalculator
	{
	public:
		inline static const cascadedDirLightMatrices getMatrix(const DirLightComponent& light)
		{
			vec3 frustaPoints_lightSpace[3][8];
			mat4 toLightSpace = mat4::lookAt(light.direction);
			toLightSpace.transpose();
			for (uint8_t i = 0; i < 3; i++)
			{
				for (uint8_t j = 0; j < 8; j++)
				{
					frustaPoints_lightSpace[i][j] = (toLightSpace * vec4(frustaPoints_worldSpace[i][j], 1.0f)).xyz();
				}
			}
			//calculate the actual matrices
			cascadedDirLightMatrices matrices;
			float dimensions[6];
			calcFrustumBox(frustaPoints_lightSpace[0], dimensions);
			matrices.nearCascade = mat4::transposed(mat4::projMatOrtho(dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4], dimensions[5]) * toLightSpace);
			calcFrustumBox(frustaPoints_lightSpace[1], dimensions);
			matrices.middleCascade = mat4::transposed(mat4::projMatOrtho(dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4], dimensions[5]) * toLightSpace);
			calcFrustumBox(frustaPoints_lightSpace[2], dimensions);
			matrices.farCascade = mat4::transposed(mat4::projMatOrtho(dimensions[0], dimensions[1], dimensions[2], dimensions[3], dimensions[4], dimensions[5]) * toLightSpace);
			return matrices;
		}
		inline static const pointLightMatrices getMatrix(const PointLightComponent& light)//maybe let this function take a ptr to pointLightMatrices and instead of returning just modify that ptr
		{
			vec4 upvector(0.0f, -1.0f, 0.0f, 0.0f);
			pointLightMatrices matrices;
			mat4 translation = mat4::transMat(-light.position);
			matrices.positiveX = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ 1.0f, 0.0f, 0.0f, 0.0f }, upvector)) * translation);//looking at positve x
			matrices.negativeX = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ -1.0f, 0.0f, 0.0f, 0.0f }, upvector)) * translation);//looking at negative x
			matrices.positiveY = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ 0.0f, 1.0f, 0.0f, 0.0f })) * translation);//looking at positive y
			matrices.negativeY = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ 0.0f, -1.0f, 0.0f, 0.0f }, { 0.0f, -0.7071067f, -0.07071067f, 0.0f })) * translation);//looking at negative y
			matrices.positiveZ = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ 0.0f, 0.0f, 1.0f, 0.0f }, upvector)) * translation);//looking at positive z
			matrices.negativeZ = mat4::transposed(s_projPoint * mat4::transposed(mat4::lookAt({ 0.0f, 0.0f, -1.0f, 0.0f }, upvector)) * translation);//looking at negative z
			return matrices;
		}
		inline static const mat4 getMatrix(const SpotLightComponent& light)
		{
			return s_projSpot * mat4::transposed(mat4::lookAt(light.direction)) * mat4::transMat(-light.position);
		}

		inline static void setViewMat(const mat4& viewMat)
		{
			s_INV_viewMat = mat4::inverse3x3(viewMat) * mat4::transMat({ -viewMat.mat[0][3], -viewMat.mat[1][3], -viewMat.mat[2][3], 1.0f });
			for (uint8_t i = 0; i < 3; i++)
			{
				for (uint8_t j = 0; j < 8; j++)
				{
					frustaPoints_worldSpace[i][j] = (s_INV_viewMat * vec4(frustaPoints_viewSpace[i][j], 1.0f)).xyz();
				}
			}
		}
		inline static void calcFrustumBox(vec3 points[8], float dimensionsOut[6])
		{
			dimensionsOut[0] = std::numeric_limits<float>::max();
			dimensionsOut[1] = std::numeric_limits<float>::min();
			dimensionsOut[2] = std::numeric_limits<float>::max();
			dimensionsOut[3] = std::numeric_limits<float>::min();
			dimensionsOut[4] = std::numeric_limits<float>::min();//near
			dimensionsOut[4] = -100000.0f;
			dimensionsOut[5] = std::numeric_limits<float>::min();//far
			dimensionsOut[5] = -10.0f;//a value that is between znear and zfar from which we can go in the negative direction
			for (uint8_t i = 0; i < 8; i++)
			{
				dimensionsOut[0] = std::fmin(dimensionsOut[0], points[i].x);//left
				dimensionsOut[1] = std::fmax(dimensionsOut[1], points[i].x);//right
				dimensionsOut[2] = std::fmin(dimensionsOut[2], points[i].y);//bottom
				dimensionsOut[3] = std::fmax(dimensionsOut[3], points[i].y);//top
				dimensionsOut[4] = std::fmax(dimensionsOut[4], points[i].z);//near
				dimensionsOut[5] = std::fmin(dimensionsOut[5], points[i].z);//far
			}
		}

		static void init(const float hFov, const float vFov, const float nearPlane, const float farPlane);
	private:
		static mat4 s_INV_viewMat;//gets set every frame
		static mat4 s_projPoint;
		static mat4 s_projSpot;

		static float s_fovs[2];
		static vec3 frustaPoints_viewSpace[3][8];//8 points per frustum in viewSpace that get calculated once on startup
		static vec3 frustaPoints_worldSpace[3][8];//gets calculated once per frame
	};

	class LightRenderingSystem : public System<5, 3>//somehow automatically retrieve these numbers
	{
	public:
		LightRenderingSystem(Registry<5, 3>& reg)
			: m_registry(reg)
		{
			//technically we don't need to set any requirements here because this doesn't even manage any entities. It just get's all components of a certain type and performs calculations with them
			//gonna set requirements anyways because the current system requires it
			setRequirements<TransformComponent, MeshComponent, DirLightComponent, PointLightComponent, SpotLightComponent>();//just require all types because we won't use the feature
		}
		void init();
		void clear();

		const std::vector<DirLightComponent>& getDirLights() const { return m_registry.getAllOf<DirLightComponent>(); }
		const std::vector<PointLightComponent>& getPointLights() const { return m_registry.getAllOf<PointLightComponent>(); }
		const std::vector<SpotLightComponent>& getSpotLights() const { return m_registry.getAllOf<SpotLightComponent>(); }
		WeakRef_ptr<ShadowMap2dArray> getDirLightMapArray() { return m_dirLightMapArray; }
		WeakRef_ptr<ShadowMap3dArray> getPointLightMapArray() { return m_pointLightMapArray; }
		WeakRef_ptr<ShadowMap2dArray> getSpotLightMapArray() { return m_spotLightMapArray; }
		WeakRef_ptr<FrameBuffer> getDepthFB() { return m_depthFrameBuffer; }
		WeakRef_ptr<shader> getDepthShader_dir() { return m_depthShader_dir; }
		WeakRef_ptr<shader> getDepthShader_point() { return m_depthShader_point; }
		WeakRef_ptr<shader> getDepthShader_spot() { return m_depthShader_spot; }

		void updateAndFlush()//update the lightMatrices and flush everything over to the GPU
		{
			applyTransforms(); updateLightCount(); updateLightMatrices(); flushDirLights(); flushPointLights(); flushSpotLights();
		}
		void onImGuiRender();
	private:
		void updateLightMatrices();
		void flushDirLights();
		void flushPointLights();
		void flushSpotLights();
		void updateLightCount();
		void applyTransforms();//for lights we have to apply the global transforms manually

		Registry<5, 3>& m_registry;

		Ref_ptr<ShadowMap2dArray> m_dirLightMapArray;
		Ref_ptr<ShadowMap3dArray> m_pointLightMapArray;
		Ref_ptr<ShadowMap2dArray> m_spotLightMapArray;

		Ref_ptr<FrameBuffer> m_depthFrameBuffer;
		Ref_ptr<shader> m_depthShader_dir;
		Ref_ptr<shader> m_depthShader_point;
		Ref_ptr<shader> m_depthShader_spot;

		Scope_ptr<globalBuffer> directionalLightBuffer;
		Scope_ptr<globalBuffer> pointLightBuffer;
		Scope_ptr<globalBuffer> spotLightBuffer;
	};
}