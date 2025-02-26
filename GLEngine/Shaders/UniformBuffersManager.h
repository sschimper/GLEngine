/** ==============================================
 * @file 		UniformBuffersManager.h
 * @date 		2018/03/17 20:04
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <QOpenGLFunctions_4_4_Core>

#include <memory>
#include <string>
#include <vector>

namespace GLW {
	class C_UniformBuffer;
	class C_ShaderProgram;
}

/** ==============================================
 * @class C_UniformBuffersManager
 *
 * @brief	Manages UBO's over application.
 *
 * @note	This class is intended to store and bind UBO's to 
 *			shader programs. Also possibly prints some statistics
 *
 * @todo	It would be nice to extend statistics to memory usage details
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_UniformBuffersManager 
	: public QOpenGLFunctions_4_4_Core {
public:
	using T_UBOSmartPtr = std::shared_ptr<GLW::C_UniformBuffer>;

	//Singleton stuff
	C_UniformBuffersManager(C_UniformBuffersManager const&) = delete;
	void operator=(C_UniformBuffersManager const&) = delete;
	static C_UniformBuffersManager& Instance();

	void PrintStatistics() const;
	void Clear();

	void BindUBOs(GLW::C_ShaderProgram* program);
	template<class T, typename ...Params>
	std::shared_ptr<T> CreateUniformBuffer(const std::string& name, Params&&... params);
	// should be used only in debug
	T_UBOSmartPtr GetBufferByName(const std::string& name) const;


	void ProcessUBOBindingPoints(std::shared_ptr<GLW::C_ShaderProgram> program);
private:
	C_UniformBuffersManager();
	std::vector<T_UBOSmartPtr> m_UBOs;
};

//=================================================================================
template<class T, typename ...Params>
std::shared_ptr<T> C_UniformBuffersManager::CreateUniformBuffer(const std::string& name, Params&&... params)
{
	auto ubo = std::make_shared<T>(name, static_cast<unsigned int>(m_UBOs.size()), std::forward<Params>(params)...);
	assert(ubo);
	m_UBOs.push_back(ubo);

	return ubo;
}
