#pragma once

#include <Core/WindowInfo.h>


namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
struct S_OpenGLWindowInfo : public Core::S_WindowInfo {
	S_OpenGLWindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height) {}
	unsigned int m_MajorVersion = 4;
	unsigned int m_MinorVersion = 3;

	virtual E_Driver GetDriver() const override {
		return S_WindowInfo::E_Driver::OpenGL;
	}
};

}}}