#pragma once

//For use by applications that want to use the engine

#include "Engine/Application.hpp"
#include "Engine/layer.hpp"
#include "Engine/Log.hpp"
#include "Engine/ImGui/ImGuiLayer.hpp"

//________Renderering_________
#include "Engine/Rendering/renderer.hpp"
#include "Engine/Rendering/buffers.hpp"
#include "Engine/Rendering/vertexBufferLayout.hpp"
#include "Engine/Rendering/shader.hpp"
//openGLspecic, but neccessary, because only openGL has vertexArrays
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

#include "Engine/input.hpp"
#include "Engine/keyCodes.hpp"
#include "Engine/mouseButtonCodes.hpp"

//________ENTRY POINT___________
#include "Engine/EntryPoint.hpp"
//______________________________