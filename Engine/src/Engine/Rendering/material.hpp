#pragma once
#include "shader.hpp"
#include "buffers.hpp"
#include "texture.hpp"
#include "rendererAPI.hpp"

#define flag_count 4
#define flag_depth_test 0x1
#define flag_no_backface_cull 0x2 //->whether an object, which has only one side should be rendered from both sides regardless
#define flag_cast_shadow 0x4
#define flag_no_deferred 0x8
#define flags_default flag_depth_test | flag_cast_shadow

namespace Engine
{
	struct MaterialProperties
	{
		MaterialProperties(const uint32_t Flags, const uint32_t Blend_src, const uint32_t Blend_dest) : flags(Flags), blend(Blend_src, Blend_dest) {  }
		uint32_t flags;
		std::pair<uint32_t, uint32_t> blend;//fist is scr, second is dest
	};

	class material
	{
		friend class materialLib;
		//friend class NodeEditorLayer;
	public:
		material(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags, uint32_t blend_src, uint32_t blend_dest);//take by reference to avoid unnessecary incrementing the ref-count
		~material();

		static Ref_ptr<material> create(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags = flags_default, uint32_t blend_src = ENG_SRC_ALPHA, uint32_t blend_dest =  ENG_ONE_MINUS_SRC_ALPHA );

		void bind(const uint16_t slot = 10);//reserve the first 10 slots for stuff like matrices and lights

		/*REMEMBER: THE FIRST SHADER ALWAYS IS THE INITIALIZATION SHADER, THAT DEFINES THE MATERIAL'S PROPERTIES. The Rest of the shaders are just for rendering the material a different way(but they should
		the same uniforms nonetheless)*/

		inline const std::string& getName() const { return m_name; }
		inline const WeakRef_ptr<shader> getShader() const { return m_shaderRef; }

		inline void enableFlags(const uint32_t flags) { m_properties.flags = m_properties.flags | flags; }
		inline void dissableFlags(const uint32_t flags) { m_properties.flags = m_properties.flags & (~flags);/*inverts the flags, that are to be dissabled, and masks out with all the flags that stay untouched*/ }
		inline uint32_t getFlags() const { return m_properties.flags; }
		inline void setFlags(const uint32_t flags) { m_properties.flags = flags; }
		inline uint32_t getBlendSrc() const { return m_properties.blend.first; }
		inline uint32_t getBlendDest() const { return m_properties.blend.second; }
		/*first is scr, second is dest*/
		inline void setBlend(const uint32_t blend[2]) { m_properties.blend.first = blend[0]; m_properties.blend.second = blend[1]; }
		inline void setBlendScr(const uint32_t blend_src) { m_properties.blend.first = blend_src; }
		inline void setBlendDest(const uint32_t blend_dest) { m_properties.blend.second = blend_dest; }

		//for setting a single uniform and instantly updating it
		void setUniform1bF(const std::string& name, const bool bValue);
		void setUniform1iF(const std::string& name, const int iValue);
		void setUniform1fF(const std::string& name, const float fValue);
		void setUniform2fF(const std::string& name, const float v0, const float v1);
		void setUniform3fF(const std::string& name, const float v0, const float v1, const float v2);
		void setUniform3fF(const std::string& name, const vec3& vec);
		void setUniform4fF(const std::string& name, const float v0, const float v1, const float v2, const float v3);
		void setUniform4fF(const std::string& name, const vec4& vec);
		void setUniformMat4F(const std::string& name, const mat4& mat);

		//for setting a single uniform but only updating on CPU-side->usefull when loading a material because one can just flush everything over with flushAll() -> only one glCall
		void setUniform1b(const std::string& name, const bool bValue);
		void setUniform1i(const std::string& name, const int bValue);
		void setUniform1f(const std::string& name, const float bValue);
		void setUniform2f(const std::string& name, const float v0, const float v1);
		void setUniform3f(const std::string& name, const vec3& vec);
		void setUniform3f(const std::string& name, const float v0, const float v1, const float v2);
		void setUniform4f(const std::string& name, const vec4& vec);
		void setUniform4f(const std::string& name, const float v0, const float v1, const float v2, const float v3);
		void setUniformMat4(const std::string& name, const mat4& mat);

		void setTexture(const std::string& name, const Ref_ptr<texture>& tex);

		void flushAll() const;//flush the whole data Stored on cpu-side onto the GPU(useful when loading the material)->add it so that a setUniform... -function also uploads that specific uniform on the GPU

		static void renderMaterial(WeakRef_ptr<material>& mat);//render the imGuiPanel for a material
	private:
		static void renderComponent(const std::pair<std::string, shader::uniformProps>& props, const unsigned char* data);
		inline const void* getData() const { return m_data; }
		const std::string m_name;
		WeakRef_ptr<shader> m_shaderRef;//store a weak_ptr because a material shouldn't take ownership over it's shader(if one would delete the shaderLib for some reason)
		std::unordered_map<std::string, Ref_ptr<texture>> m_textures;
		Scope_ptr<globalBuffer> m_globalBuffer;//this pointer inherits the material's lifetime, because it only belongs to this material
		MaterialProperties m_properties;//pipeline state when the material is rendered
		void* m_data;//where all the material's data is stored on cpu-side
	};

	class materialLib
	{
		//friend class NodeEditorLayer;
	public:
		static void init();
		//____________________
		//add something static here, that handles "materialBindingPoints" (the points that the globalBuffers and the shaders get bound to)<- might become nessecary in the future but not for now
		//____________________
		void add(const Ref_ptr<material>& material);//take the smartPointer in as reference, because it will get copied when it is pushed into the map(so we don't make an extra ref-count for no reason)
		void add(const Ref_ptr<material>& material, const std::string& name);//inserts the shader into the lib under keyVal "name"
		void addDynamic(const Ref_ptr<material>& Material, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues);
		void updateDynamic();
		Ref_ptr<material> get(const std::string& name);//get a material by keyVal "name"
		void remove(const std::string& name);//remove the material with name "name"
		void name(const std::string& nameOld, const std::string& nameNew);//save the material that is currently stored with keyVal "nameOld" with the new keyval "nameNew"
		bool exists(const std::string& name);
		void clear();//remove all materials from the library

		void onImGuiRender();
		void renderComponent(const std::pair<std::string, shader::uniformProps>& props, const unsigned char* data);
		
		inline static const uint8_t getMaxTexSlots() { return s_maxTexSlots; }
	private:
		struct dynamicMat
		{
			WeakRef_ptr<material> mat;
			const void* updateValues;
			std::function<void(const void* updateValues, material* mat)> updateFunc;
		};
		std::unordered_map<std::string, Ref_ptr<material>> m_materials;//first is name, second is corresponding material
		std::unordered_map<std::string, dynamicMat> m_dynamicMats;//holds references to the dynamic materials -> ownership is in "m_materials" regardless
		//static members for "materialBindingPoint" management
		static unsigned int s_maxMats;//maximum of globalBuffers(materials) that can be bound simultaneously
		static uint8_t s_maxTexSlots;//maximum Number of texSlots the graphics card supports
	};
}