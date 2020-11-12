#include "renderLayer.hpp"

void renderLayer::onUpdate()
{
	unsigned int height = Engine::Application::Get().getWindow().getHeight();
	unsigned int width = Engine::Application::Get().getWindow().getWidth();

	Engine::renderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Engine::renderCommand::clear();

	Engine::Renderer::beginScene();
	{
		m_shader->bind();
		Engine::Renderer::sub(m_vertexArray);
	}
	Engine::Renderer::endScene();

	Engine::Renderer::Flush();
}

void renderLayer::onImGuiRender()
{

}

void renderLayer::onEvent(Engine::Event& e)
{
	Engine::eventDispatcher dispatcher(e);
	dispatcher.dispatchEvent<Engine::windowResizeEvent>(BIND_EVENT_FN(renderLayer::onWindowResizeEvent));
}

bool renderLayer::onWindowResizeEvent(Engine::windowResizeEvent& e)
{
	return false;
}

void renderLayer::temporarySetup()
{
	m_vertexArray.reset(new Engine::GLvertexArray);//we don't use a generalized implementation here because vertexArrays only exist in openGL

	float pVertexBuffer[7 * 3] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		0.0f, 0.5f, 0.0f, 0.2f, 0.8f, 0.2f
	};
	m_vertexBuffer.reset(Engine::vertexBuffer::create(sizeof(pVertexBuffer), pVertexBuffer, STATIC_DRAW));
	unsigned int pIndexBuffer[3] = {
		0, 1, 2
	};
	m_indexBuffer.reset(Engine::indexBuffer::create(3, pIndexBuffer, STATIC_DRAW));

	std::shared_ptr<Engine::vertexBufferLayout> vertexBufferLayout;
	vertexBufferLayout.reset(Engine::vertexBufferLayout::create());
	vertexBufferLayout->push(Engine::ShaderDataType::vec3);//could also use "vertexBufferLayout.pushFloat(3);" to achieve the same thing
	vertexBufferLayout->push(Engine::ShaderDataType::vec4);//for color
	m_vertexBuffer->setLayout(vertexBufferLayout);//maybe make this accept a shared_ptr, so that we don't dealocate the memory we set the layout to
	//sadly we have to do an ugly conversion there because vertexArrays don't have a generalized implementation(cuz theire openGL only)
	m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLvertexBuffer, Engine::vertexBuffer>(m_vertexBuffer));//tie the buffer and it's layout to the vertexArray
	m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLindexBuffer, Engine::indexBuffer>(m_indexBuffer));//tie the indexBuffer to the vertexArray

	m_shader.reset(Engine::shader::create("basic.shader"));

	//unbind everything
	m_vertexArray->unbind();
	m_vertexBuffer->unbind();
	m_indexBuffer->unbind();
	m_shader->unbind();
}