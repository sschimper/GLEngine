#include <RendererStdafx.h>

#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::Renderer {

//=================================================================================
// I_TextureViewStorage
//=================================================================================
I_TextureViewStorage::I_TextureViewStorage(std::size_t width, std::size_t height)
	: m_Dimensions({width, height})
	, m_Channels({E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha})
{
}

//=================================================================================
I_TextureViewStorage::~I_TextureViewStorage() = default;

//=================================================================================
void I_TextureViewStorage::Set(double value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
void I_TextureViewStorage::Set(int value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
glm::ivec2 I_TextureViewStorage::GetDimensions() const
{
	return m_Dimensions;
}

//=================================================================================
bool I_TextureViewStorage::IsSwizzled() const
{
	return !(m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None});
}

//=================================================================================
glm::vec4 I_TextureViewStorage::Swizzle(const glm::vec4& value) const
{
	if (!IsSwizzled())
		return value;
	auto result = glm::vec4(0.f);
	const auto channelGetter = [](const glm::vec4& value, E_TextureChannel channel) { 
		switch (channel)
		{
		case E_TextureChannel::Red: return value.r;
		case E_TextureChannel::Green: return value.g;
		case E_TextureChannel::Blue: return value.b;
		case E_TextureChannel::Alpha: return value.a;
		case E_TextureChannel::None:
		default: return 0.f;
		}
	};

	std::uint8_t i = 0;
	for (const auto& ch : m_Channels) {
		result[i++] = channelGetter(value, ch);
	}
	return result;
}

} // namespace GLEngine::Renderer
