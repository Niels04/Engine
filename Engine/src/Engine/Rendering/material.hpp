#pragma once
#include "shader.hpp"
#include "buffers.hpp"
#include "texture.hpp"
#include "rendererAPI.hpp"

#define flag_light_infl 0x1 //whether the material's appearance is dependent on lighting
#define flag_depth_test 0x2
#define flag_no_backface_cull 0x4 //->whether an object, which has only one side should be rendered from both sides regardless
#define flag_cast_shadow 0x8
#define flags_default flag_light_infl | flag_depth_test | flag_cast_shadow

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
	public:
		material(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags, uint32_t blend_src, uint32_t blend_dest);//take by reference to avoid unnessecary incrementing the ref-count
		~material();

		static Ref_ptr<material> create(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags = flags_default, uint32_t blend_src = ENG_SRC_ALPHA, uint32_t blend_dest =  ENG_ONE_MINUS_SRC_ALPHA );

		inline void addShader(Ref_ptr<shader>& shader) { m_shaderRefs.push_back(shader); }
		void bind(const uint16_t slot = 10, const uint8_t shader = 0);//reserve the first 10 slots for stuff like matrices and lights

		/*REMEMBER: THE FIRST SHADER ALWAYS IS THE INITIALIZATION SHADER, THAT DEFINES THE MATERIAL'S PROPERTIES. The Rest of the shaders are just for rendering the material a different way(but they should
		the same uniforms nonetheless)*/

		inline const std::string& getName() const { return m_name; }
		inline const WeakRef_ptr<shader> getShader(const uint8_t index = 0) const { return m_shaderRefs[index]; }

		inline void enableFlags(const uint32_t flags) { m_properties.flags = m_properties.flags | flags; }
		inline void dissableFlags(const uint32_t flags) { m_properties.flags = m_properties.flags & (~flags);/*inverts the flags, that are to be dissabled, and masks out with all the flags that stay untouched*/ }
		inline uint32_t getFlags() const { return m_properties.flags; }
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

	private:
		const std::string m_name;
		std::vector<WeakRef_ptr<shader>> m_shaderRefs;//store a weak_ptr because a material shouldn't take ownership over it's shader(if one would delete the shaderLib for some reason)
		std::unordered_map<std::string, Ref_ptr<texture>> m_textures;
		Scope_ptr<globalBuffer> m_globalBuffer;//this pointer inherits the material's lifetime, because it only belongs to this material
		MaterialProperties m_properties;//pipeline state when the material is rendered
		void* m_data;//where all the material's data is stored on cpu-side
	};

	class materialLib
	{
	public:
		static void init();
		//____________________
		//add something static here, that handles "materialBindingPoints" (the points that the globalBuffers and the shaders get bound to)<- might become nessecary in the future but not for now
		//____________________
		void add(const Ref_ptr<material>& material);//take the smartPointer in as reference, because it will get copied when it is pushed into the map(so we don't make an extra ref-count for no reason)
		void add(const Ref_ptr<material>& material, const std::string& name);//inserts the shader into the lib under keyVal "name"
		Ref_ptr<material> get(const std::string& name);//get a material by keyVal "name"
		void remove(const std::string& name);//remove the material with name "name"
		void name(const std::string& nameOld, const std::string& nameNew);//save the material that is currently stored with keyVal "nameOld" with the new keyval "nameNew"
		bool exists(const std::string& name);
		void clear();//remove all materials from the library
		
		inline static const uint8_t getMaxTexSlots() { return s_maxTexSlots; }
	private:
		std::unordered_map<std::string, Ref_ptr<material>> m_materials;//first is name, second is corresponding material
		//static members for "materialBindingPoint" management
		static unsigned int s_maxMats;//maximum of globalBuffers(materials) that can be bound simultaneously
		static uint8_t s_maxTexSlots;//maximum Number of texSlots the graphics card supports
	};
}