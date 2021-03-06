#pragma once
#include <memory>

#ifdef ENG_PLATFORM_WINDOWS
#ifdef ENG_DYNAMIC_LINK
	#ifdef ENG_BUILD_DLL
		#define ENGINE_API _declspec(dllexport)
	#else
		#define ENGINE_API _declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif
#else
	#error Engine only supports Windows!
#endif

#ifdef ENG_DEBUG
	#define ENG_ENABLE_ASSERTS
#endif

#ifdef ENG_ENABLE_ASSERTS
	#define ENG_ASSERT(x, ...) { if(!(x)) { ENG_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
	#define ENG_CORE_ASSERT(x, ...) { if(!(x)) { ENG_CORE_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ENG_ASSERT(x, ...)
	#define ENG_CORE_ASSERT(x, ...)
#endif

//this means the value 1 gets bitshifted by whatever "x" is, so we can easily filter things using this
#define BIT(x) (1 << x)

#define ENG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine
{
	//typedefining shared&unique pointers here -> stuff that specifically belongs to the engine, like assets will use these
	template<typename T>
	using Scope_ptr = std::unique_ptr<T>;//use "using" here instead of "typedef" because it let's us declare this as a template
	template<typename T>				 //could also do something like usgint shaderRef_ptr = std::shared_ptr<Engine::shader>;
	using Ref_ptr = std::shared_ptr<T>;
	template<typename T>
	using WeakRef_ptr = std::shared_ptr<T>;
	//a pointer to a pointer that provides normal use like a ptr
	template<typename T>
	class PtrPtr
	{
	public:
		PtrPtr() = default;
		PtrPtr(const T** ptr) : m_ptr(ptr) {  }
		PtrPtr(T** ptr) : m_ptr(ptr) {  }
		inline void operator =(const T** ptr) { m_ptr = ptr; }
		inline void operator =(T** ptr) { m_ptr = ptr; }
		inline operator T*() const { return (*m_ptr); }
		inline T* operator->() const { return (*m_ptr); }
		inline T** get() const { return m_ptr; }
	private:
		T** m_ptr;
	};
}