#include "Engpch.hpp"
#include "glCore.hpp"
#include "OpenGLVertexArray.hpp"

namespace Engine
{
	GLvertexArray::GLvertexArray()
	{
		GLCALL(glGenVertexArrays(1, &m_renderer_id));
	}

	GLvertexArray::~GLvertexArray()
	{
		GLCALL(glDeleteVertexArrays(1, &m_renderer_id));
	}

	void GLvertexArray::addBuffer(Ref_ptr<vertexBuffer> buffer)
	{
		m_vertexBuffer = std::static_pointer_cast<GLvertexBuffer, vertexBuffer>(buffer);
		bind();
		m_vertexBuffer->bind();
		m_vertexBuffer->bindLayout();
	}

	void GLvertexArray::addBuffer(Ref_ptr<indexBuffer> buffer)
	{
		m_indexBuffer = std::static_pointer_cast<GLindexBuffer, indexBuffer>(buffer);
		bind();
		m_indexBuffer->bind();
	}

	void GLvertexArray::bind() const
	{
		GLCALL(glBindVertexArray(m_renderer_id));
	}

	void GLvertexArray::unbind() const
	{
		GLCALL(glBindVertexArray(0));
	}

	void GLvertexArray::load(const std::string& name)
	{
		std::string path("res/models/");
		path.append(name);

		//get the vertexBuffer
		uint32_t size = 0;
		void* data = NULL;
		std::ifstream input(path.c_str(), std::ios::binary);

		input.read((char*)&size, sizeof(uint32_t));//retrieve the vertexBuffer's size in bytes
		ENG_CORE_ASSERT(input.is_open(), "Error when trying to access file.");
		data = malloc(size);
		input.read((char*)data, size);
		Ref_ptr<GLvertexBuffer> vertexBuffer = std::make_shared<GLvertexBuffer>(size, data, STATIC_DRAW);
		free(data); data = NULL;
		//get the indexBuffer
		uint32_t count = 0;
		input.read((char*)&count, sizeof(uint32_t));//retrieve the indexBuffer's size in bytes
		data = malloc(count * sizeof(uint32_t));
		input.read((char*)data, count * sizeof(uint32_t));
		Ref_ptr<GLindexBuffer> indexBuffer = std::make_shared<GLindexBuffer>(count, (uint32_t*)data, STATIC_DRAW);
		free(data); data = NULL;
		//get the layout
		Ref_ptr<GLvertexBufferLayout> temp = std::make_shared<GLvertexBufferLayout>();
		uint32_t AttribCount;
		input.read((char*)&AttribCount, sizeof(uint32_t));//get the vertexAttributes count
		for (uint32_t i = 0; i < AttribCount; i++)//read all the attributes
		{
			vertexBufferElement tempElement;
			input.read((char*)&tempElement, sizeof(vertexBufferElement));
			temp->push(tempElement);
		}
		input.close();

		vertexBuffer->setLayout(temp);
		addBuffer(vertexBuffer);
		addBuffer(indexBuffer);
	}
}