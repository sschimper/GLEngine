#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer::Buffers {
//=================================================================================
template <GLenum TYPE> C_GLBuffer<TYPE>::C_GLBuffer()
{
	glGenBuffers(1, &m_id);
}

//=================================================================================
template <GLenum TYPE> C_GLBuffer<TYPE>::~C_GLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::unbind() const
{
	glBindBuffer(TYPE, 0);
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::bind() const
{
	glBindBuffer(TYPE, m_id);
}

//=================================================================================
template <GLenum TYPE> constexpr GLenum C_GLBuffer<TYPE>::GetBufferType() const noexcept
{
	return TYPE;
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::AllocateMemory(const std::size_t size, GLenum usage, const void* data /*= nullptr*/)
{
	// TODO: here should go the memory counting
	bind();
	glBufferData(GetBufferType(), size, data, usage);
	unbind();
}
} // namespace GLEngine::GLRenderer::Buffers

namespace GLEngine::GLRenderer {
//=================================================================================
inline GLenum GLBuffer::GetType() const
{
	return GetBufferType(desc.type);
}

//=================================================================================
inline GLenum GLBuffer::GetUsage() const
{
	switch (desc.usage)
	{
	case Renderer::E_ResourceUsage::Immutable:
		return GL_STATIC_DRAW;
	case Renderer::E_ResourceUsage::Dynamic:
		return GL_STREAM_DRAW;
	}
	return GL_STATIC_DRAW; // todo
}

//=================================================================================
inline void GLBuffer::bind() const
{
	glBindBuffer(GetType(), m_id);
}

//=================================================================================
inline void GLBuffer::unbind() const
{
	glBindBuffer(GetType(), 0);
}

//=================================================================================
inline uint32_t GLBuffer::GetSize() const
{
	return desc.size;
}
} // namespace GLEngine::GLRenderer