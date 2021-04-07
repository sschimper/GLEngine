#pragma once

#include <GLRendererStdafx.h>

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Textures/TextureStorage.h>

#include <Core/CoreMacros.h>

// clang-format off
namespace GLEngine::GLRenderer {

//=================================================================================
template <class T> struct T_TypeToGL;

template <> struct T_TypeToGL<std::uint8_t> {	static constexpr GLenum value = GL_UNSIGNED_BYTE;};
template <> struct T_TypeToGL<char>			{	static constexpr GLenum value = GL_BYTE;};

template <> struct T_TypeToGL<float>		{	static constexpr GLenum value = GL_FLOAT;};
template <> struct T_TypeToGL<double>		{	static constexpr GLenum value = GL_DOUBLE;};

template <> struct T_TypeToGL<int>			{	static constexpr GLenum value = GL_INT;};
template <> struct T_TypeToGL<unsigned int> {	static constexpr GLenum value = GL_UNSIGNED_INT;};

template <> struct T_TypeToGL<short>		{	static constexpr GLenum value = GL_SHORT;};
template <> struct T_TypeToGL<unsigned short> {	static constexpr GLenum value = GL_UNSIGNED_SHORT;};

template <> struct T_TypeToGL<glm::vec1>	{	static constexpr GLenum value = GL_FLOAT;};
template <> struct T_TypeToGL<glm::vec2>	{	static constexpr GLenum value = GL_FLOAT;};
template <> struct T_TypeToGL<glm::vec3>	{	static constexpr GLenum value = GL_FLOAT;};
template <> struct T_TypeToGL<glm::vec4>	{	static constexpr GLenum value = GL_FLOAT;};

template <> struct T_TypeToGL<glm::ivec1>	{	static constexpr GLenum value = GL_INT;};
template <> struct T_TypeToGL<glm::ivec2>	{	static constexpr GLenum value = GL_INT;};
template <> struct T_TypeToGL<glm::ivec3>	{	static constexpr GLenum value = GL_INT;};
template <> struct T_TypeToGL<glm::ivec4>	{	static constexpr GLenum value = GL_INT;};

//=================================================================================
[[nodiscard]] inline GLenum GetUnderlyingType(const Renderer::I_TextureViewStorage* tex)
{
	if (tex->GetStorageType() == Renderer::E_TextureTypes::Floating)
	{
		return GL_FLOAT;
	}
	if (tex->GetStorageType() == Renderer::E_TextureTypes::IntegralNormalized)
	{
		return GL_UNSIGNED_BYTE;
	}
	if (tex->GetStorageType() == Renderer::E_TextureTypes::Integral)
	{
		return GL_BYTE;
	}
	GLE_ERROR("Unsupported type");

	return GL_BYTE;
}

//=================================================================================
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
template <GLenum type> struct T_GLTypeIsIntegral : public std::false_type {};

template <> struct T_GLTypeIsIntegral<GL_BYTE> : public std::true_type {};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_BYTE> : public std::true_type {};
template <> struct T_GLTypeIsIntegral<GL_SHORT> : public std::true_type {};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_SHORT> : public std::true_type {};
template <> struct T_GLTypeIsIntegral<GL_INT> : public std::true_type {};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_INT> : public std::true_type {};

template <GLenum type> inline constexpr bool T_GLTypeIsIntegral_v = T_GLTypeIsIntegral<type>::value;

//=================================================================================
template <GLenum type> struct T_GLTypeIsDouble : public std::false_type {};

template <> struct T_GLTypeIsDouble<GL_DOUBLE> : public std::true_type {};

template <GLenum type> inline constexpr bool T_GLTypeIsDouble_v = T_GLTypeIsDouble<type>::value;

//=================================================================================
template <class T> struct T_GLNumComponenets;

template <> struct T_GLNumComponenets<unsigned char>  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<char>			  {	static constexpr std::size_t value = 1;};

template <> struct T_GLNumComponenets<float>		  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<double>		  {	static constexpr std::size_t value = 1;};

template <> struct T_GLNumComponenets<int>			  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<unsigned int>	  {	static constexpr std::size_t value = 1;};

template <> struct T_GLNumComponenets<short>		  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<unsigned short> {	static constexpr std::size_t value = 1;};

template <> struct T_GLNumComponenets<glm::vec1>	  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<glm::vec2>	  {	static constexpr std::size_t value = 2;};
template <> struct T_GLNumComponenets<glm::vec3>	  {	static constexpr std::size_t value = 3;};
template <> struct T_GLNumComponenets<glm::vec4>	  {	static constexpr std::size_t value = 4;};

template <> struct T_GLNumComponenets<glm::ivec1>	  {	static constexpr std::size_t value = 1;};
template <> struct T_GLNumComponenets<glm::ivec2>	  {	static constexpr std::size_t value = 2;};
template <> struct T_GLNumComponenets<glm::ivec3>	  {	static constexpr std::size_t value = 3;};
template <> struct T_GLNumComponenets<glm::ivec4>	  {	static constexpr std::size_t value = 4;};

template <class type> inline constexpr std::size_t T_GLNumComponenets_v = T_GLNumComponenets<type>::value;

//=================================================================================
enum class E_FramebufferTarget
{
	Framebuffer,
	Read,
	Draw,
};

template <E_FramebufferTarget framebuffer> struct T_FramebufferTarget;

template <> struct T_FramebufferTarget<E_FramebufferTarget::Framebuffer>  {	static constexpr GLenum value = GL_FRAMEBUFFER;};
template <> struct T_FramebufferTarget<E_FramebufferTarget::Read>		  {	static constexpr GLenum value = GL_READ_FRAMEBUFFER;};
template <> struct T_FramebufferTarget<E_FramebufferTarget::Draw>		  {	static constexpr GLenum value = GL_DRAW_FRAMEBUFFER;};
// clang-format on

//=================================================================================
// Wrap functions
constexpr GLenum WrapFunctionToEnum(const Renderer::E_WrapFunction wrap)
{
	using namespace Renderer;
	switch (wrap)
	{
	case E_WrapFunction::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	case E_WrapFunction::ClampToBorder:
		return GL_CLAMP_TO_BORDER;
	case E_WrapFunction::MirroredRepeat:
		return GL_MIRRORED_REPEAT;
	case E_WrapFunction::Repeat:
		return GL_REPEAT;
	}
	return GL_INVALID_VALUE;
}

template <Renderer::E_WrapFunction wrapFunction> struct T_WrapFunction {
	static constexpr GLenum value = WrapFunctionToEnum(wrapFunction);
};

//=================================================================================
inline constexpr GLenum OpenGLUnderlyingType(const Renderer::E_TextureFormat format)
{
	switch (format)
	{
	case Renderer::E_TextureFormat::RGBA32f:
	case Renderer::E_TextureFormat::RGB32f:
	case Renderer::E_TextureFormat::RG32f:
	case Renderer::E_TextureFormat::R32f:
	case Renderer::E_TextureFormat::RGBA16f:
	case Renderer::E_TextureFormat::RGB16f:
	case Renderer::E_TextureFormat::RG16f:
	case Renderer::E_TextureFormat::R16f:
		return GL_FLOAT;
	case Renderer::E_TextureFormat::RGBA32i:
	case Renderer::E_TextureFormat::RGB32i:
	case Renderer::E_TextureFormat::RG32i:
	case Renderer::E_TextureFormat::R32i:
		return GL_INT;
	case Renderer::E_TextureFormat::RGBA16i:
	case Renderer::E_TextureFormat::RGB16i:
	case Renderer::E_TextureFormat::RG16i:
	case Renderer::E_TextureFormat::R16i:
		return GL_SHORT;
	case Renderer::E_TextureFormat::RGBA8i:
	case Renderer::E_TextureFormat::RGB8i:
	case Renderer::E_TextureFormat::RG8i:
	case Renderer::E_TextureFormat::R8i:
		return GL_BYTE;
	case Renderer::E_TextureFormat::D24S8:
		return GL_UNSIGNED_INT_24_8;
	case Renderer::E_TextureFormat::D32f:
		return GL_FLOAT;
	case Renderer::E_TextureFormat::D24:
		return GL_FLOAT;
	case Renderer::E_TextureFormat::D16:
		return GL_FLOAT;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown texture format");
		break;
	}
	return GL_BYTE;
}

//=================================================================================
inline constexpr GLenum GetOpenGLInternalFormat(const Renderer::E_TextureFormat format)
{
	switch (format)
	{
	case Renderer::E_TextureFormat::RGBA32f:
		return GL_RGBA32F;
	case Renderer::E_TextureFormat::RGB32f:
		return GL_RGB32F;
	case Renderer::E_TextureFormat::RG32f:
		return GL_RG32F;
	case Renderer::E_TextureFormat::R32f:
		return GL_R32F;
	case Renderer::E_TextureFormat::RGBA16f:
		return GL_RGBA16F;
	case Renderer::E_TextureFormat::RGB16f:
		return GL_RGB16F;
	case Renderer::E_TextureFormat::RG16f:
		return GL_RG16F;
	case Renderer::E_TextureFormat::R16f:
		return GL_R16F;
	case Renderer::E_TextureFormat::RGBA32i:
		return GL_RGBA32I;
	case Renderer::E_TextureFormat::RGB32i:
		return GL_RGB32I;
	case Renderer::E_TextureFormat::RG32i:
		return GL_RG32I;
	case Renderer::E_TextureFormat::R32i:
		return GL_R32I;
	case Renderer::E_TextureFormat::RGBA16i:
		return GL_RGBA16I;
	case Renderer::E_TextureFormat::RGB16i:
		return GL_RGB16I;
	case Renderer::E_TextureFormat::RG16i:
		return GL_RG16I;
	case Renderer::E_TextureFormat::R16i:
		return GL_R16I;
	case Renderer::E_TextureFormat::RGBA8i:
		return GL_RGBA;
	case Renderer::E_TextureFormat::RGB8i:
		return GL_RGB8I;
	case Renderer::E_TextureFormat::RG8i:
		return GL_RG8I;
	case Renderer::E_TextureFormat::R8i:
		return GL_R8I;
	case Renderer::E_TextureFormat::D32f:
		return GL_DEPTH_COMPONENT32F;
	case Renderer::E_TextureFormat::D24S8:
		return GL_DEPTH24_STENCIL8;
	case Renderer::E_TextureFormat::D24:
		return GL_DEPTH_COMPONENT24;
	case Renderer::E_TextureFormat::D16:
		return GL_DEPTH_COMPONENT16;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown texture format");
		break;
	}
	return GL_R8I;
}

//=================================================================================
constexpr GLenum MinMagFilterToEnum(const Renderer::E_TextureFilter filter)
{
  using namespace Renderer;
	switch (filter)
	{
	case E_TextureFilter::Linear:
		return GL_LINEAR;
	case E_TextureFilter::Nearest:
		return GL_NEAREST;
	case E_TextureFilter::NearestMipMapNearest:
		return GL_NEAREST_MIPMAP_NEAREST;
	case E_TextureFilter::LinearMipMapNearest:
		return GL_LINEAR_MIPMAP_NEAREST;
	case E_TextureFilter::LinearMipMapLinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	case E_TextureFilter::NearestMipMapLinear:
		return GL_NEAREST_MIPMAP_LINEAR;
	}
	return GL_INVALID_VALUE;
}

//=================================================================================
// Access rights
enum class E_OpenGLAccess : char
{
	Read,
	Write,
	ReadWrite,
};

constexpr GLenum AccesRightsToEnum(const E_OpenGLAccess access)
{
	switch (access)
	{
	case E_OpenGLAccess::Read:
		return GL_READ_ONLY;
		break;
	case E_OpenGLAccess::Write:
		return GL_WRITE_ONLY;
		break;
	case E_OpenGLAccess::ReadWrite:
		return GL_READ_WRITE;
		break;
	}
	return GL_INVALID_VALUE;
}
} // namespace GLEngine::GLRenderer