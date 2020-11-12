#pragma once

//For use by applications that want to use the engine
#include "Engine/Application.hpp"
#include "Engine/layer.hpp"
#include "Engine/Log.hpp"
#include "Engine/ImGui/ImGuiLayer.hpp"
#include "Engine/core/timeStep.hpp"

//___own datatypes such as mats and vectors____
#include "Engine/datatypes/include.hpp"
//_____________________________________________

//________Renderering_________
#include "Engine/Rendering/renderer.hpp"
#include "Engine/Rendering/renderCommand.hpp"

#include "Engine/Rendering/buffers.hpp"
#include "Engine/Rendering/vertexBufferLayout.hpp"
#include "Engine/Rendering/shader.hpp"

#include "Engine/Rendering/perspCam.hpp"
//_____________________________

//openGLspecic, but neccessary, because only openGL has vertexArrays
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

//____________INPUT_____________
#include "Engine/input.hpp"
#include "Engine/keyCodes.hpp"
#include "Engine/mouseButtonCodes.hpp"
//______________________________

//________ENTRY POINT___________
#include "Engine/EntryPoint.hpp"
//______________________________