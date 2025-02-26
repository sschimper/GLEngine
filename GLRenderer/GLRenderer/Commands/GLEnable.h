#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

class C_GLEnable : public Renderer::I_RenderCommand {
public:
	enum class E_GLEnableValues {
		DEPTH_TEST = GL_DEPTH_TEST,
		CULL_FACE = GL_CULL_FACE,
		PRIMITIVE_RESTART = GL_PRIMITIVE_RESTART,
	};

	C_GLEnable(E_GLEnableValues value);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override { return nullptr; };


protected:
	E_GLEnableValues m_Value;
};

}}}