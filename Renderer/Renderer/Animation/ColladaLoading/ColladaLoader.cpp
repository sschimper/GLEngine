#include <RendererStdAfx.h>

#include <Renderer/Animation/ColladaLoading/ColladaLoader.h>

#include <Renderer/Animation/ColladaLoading/FloatArray.h>

#include <Renderer/Animation/Skeleton.h>
#include <Renderer/Animation/SkeletalAnimation.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <pugixml.hpp>

#include <array>

namespace GLEngine::Renderer::Animation {

//=================================================================================
bool C_ColladaLoader::addModelFromDAEFileToScene(
	const char* filepath,
	const char* filename,
	MeshData::Mesh& oMesh,
	std::string& textureName,
	C_Skeleton& skeleton,
	C_SkeletalAnimation& animation,
	const glm::mat4& transform /*= glm::mat4(1)*/)
{
	std::string name;
	name.append(filepath);
	name.append("/");
	name.append(filename);
	CORE_LOG(E_Level::Info, E_Context::Core, "Loading dae file: {}", name);
	pugi::xml_document doc;

	pugi::xml_parse_result result;
	result = doc.load_file(name.c_str());
	if (!result.status == pugi::status_ok) {
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open dae file: {}", name);
		return false;
	}

	auto colladaNode = doc.child("COLLADA");
	if (!colladaNode)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid dae file: {}", name);
		return false;
	}

	glm::mat4 noramlizingMatrix(1.f);

	if (auto up_Axis = colladaNode.child("asset").child("up_axis"))
	{
		// default is Y_UP, which is also default for me
		std::string_view upAxe = up_Axis.child_value();
		if (upAxe == "Z_UP")
		{
			noramlizingMatrix = glm::rotate(-glm::half_pi<float>(), glm::vec3(1.f, .0f, .0f));
		}
		else if (upAxe == "Y_UP")
		{
			//do nothing
		}
		else
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "unknown <up_axis> value: {}", upAxe);
		}
	}

	if (auto imageLibrary = colladaNode.child("library_images").child("image").child("init_from"))
	{
		textureName = imageLibrary.child_value();
	}

	if (auto geomLibrary = colladaNode.child("library_geometries"))
	{
		for (const auto geom : geomLibrary.children("geometry"))
		{
			std::vector<glm::vec4> vertices;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texCoords;
			std::string_view id = geom.attribute("name").as_string();
			
			oMesh.m_name = geom.attribute("name").as_string();
			if (auto xmlMesh = geom.child("mesh"))
			{
				for (auto xmlSource : xmlMesh.children("source"))
				{
					S_FloatArray floats(xmlSource.child("float_array"));

					std::string_view id = xmlSource.attribute("id").as_string();

					if (id.find("positions") != id.npos)
					{
						vertices.reserve(floats.count<glm::vec3>());
						while (!floats.EndOfArray())
						{
							vertices.emplace_back(noramlizingMatrix * glm::vec4(floats.Get<glm::vec3>(), 1.f));
						}
					}
					else if (id.find("normals") != id.npos)
					{
						normals.reserve(floats.count<glm::vec3>());
						while (!floats.EndOfArray())
						{
							normals.emplace_back(floats.Get<glm::vec3>());
						}
					}
					else if (id.find("map-0") != id.npos)
					{
						texCoords.reserve(floats.count<glm::vec2>());
						while (!floats.EndOfArray())
						{
							texCoords.emplace_back(floats.Get<glm::vec2>());
						}
					}
				}

				// here it is possible to non-triangle polygons to arise
				if (auto polylist = xmlMesh.child("polylist"))
				{
					if (auto indiceList = polylist.child("p"))
					{
						std::string_view indiceListString = indiceList.child_value();
						std::stringstream indicesStream;
						indicesStream << indiceListString;
						int v, n, t, c;
						while (indicesStream >> v >> n >> t >> c)
						{
							oMesh.vertices.push_back(vertices[v]);
							oMesh.normals.emplace_back(normals[n]);
							oMesh.texcoords.emplace_back(texCoords[t]);
						}
					}
				}
				else if (auto triangles = xmlMesh.child("triangles"))
				{
					int count = 0;
					if (const auto countAttribute = triangles.attribute("count"))
					{
						count = countAttribute.as_int();
					}
					else
					{
						// it's against standard, but we can recover from this error
						// we use this information only for optimize 
						CORE_LOG(E_Level::Warning, E_Context::Core, "<triangles> element misses required attribute count.");
					}

					if (auto indiceList = triangles.child("p"))
					{
						std::string_view indiceListString = indiceList.child_value();
						std::stringstream indicesStream;
						indicesStream << indiceListString;
						int v, n, t0;
						while (indicesStream >> v >> n >> t0)
						{
							oMesh.vertices.push_back(vertices[v]);
							oMesh.normals.emplace_back(normals[n]);
							oMesh.texcoords.emplace_back(texCoords[t0]);
						}
					}

				}
			}
		}
	}

	std::map<std::string, S_Joint> joints;
	if (auto controllerLibrary = colladaNode.child("library_controllers"))
	{
		if (auto skinXML = controllerLibrary.child("controller").child("skin"))
		{
			LoadJoints(skinXML);
			LoadJointsInvMatrices(joints, skinXML, noramlizingMatrix);
		}
	}

	if (auto visualScene = colladaNode.child("library_visual_scenes").child("visual_scene"))
	{
		for (auto xmlNode : visualScene.children("node"))
		{
			// we should take id from library_controllers
			std::string_view id = xmlNode.attribute("id").as_string();
			if (id == "Armature")
			{
				// ignore rotation/scale matrices
				auto modelMatrix = ParseTranslation(xmlNode);
				if (std::distance(xmlNode.children("node").begin(), xmlNode.children("node").end()) != 1)
				{
					CORE_LOG(E_Level::Warning, E_Context::Render, "Skeleton definition should contain only 1 root joint");
				}

				auto rootNode = *(xmlNode.children("node").begin());
				std::string_view boneName = rootNode.attribute("sid").as_string();
				const auto rootJoint = joints.find(boneName.data());
				if (rootJoint == joints.end())
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "Bone '{}' referenced in file '{}' doesn't exiests.", boneName, name);
					return false;
				}
				skeleton.m_Root = std::make_unique<S_Joint>(rootJoint->second);
				skeleton.m_NumBones = m_JointNames.size();
				if (!ParseChildrenJoints(*skeleton.m_Root.get(), rootNode, joints))
				{
					CORE_LOG(E_Level::Error, E_Context::Render, "Errors in joint hierarchy definition in file: {}", name);
				}
			}
		}
	}

	if (auto animationLibrary = colladaNode.child("library_animations"))
	{
		animation = C_SkeletalAnimation(m_JointNames.size());
		for (const auto& animationXML : animationLibrary.children("animation"))
		{
			std::string_view id = animationXML.attribute("id").as_string();
			const auto underscoreIndex = id.find('_') + 1;
			std::string boneIdName(id.substr(underscoreIndex, id.find("_pose_matrix") - underscoreIndex));
			CORE_LOG(E_Level::Error, E_Context::Render, "{}", boneIdName);
			const auto boneId = GetBoneId(boneIdName);
			if (boneId < 0)
			{
				CORE_LOG(E_Level::Warning, E_Context::Render, "Referenced bone '{}' but not found", boneIdName);
				continue;
			}
			animation.SetBoneTimeline(boneId, LoadBoneTimeline(animationXML));
		}
	}

	return true;
}

//=================================================================================
void C_ColladaLoader::LoadJoints(const pugi::xml_node& skinXML)
{
	for (auto xmlSource : skinXML.children("source"))
	{
		const std::string_view id = xmlSource.attribute("id").as_string();
		if (id.find("joints") == id.npos)
		{
			continue;
		}

		if (const auto nameArray = xmlSource.child("Name_array"))
		{
			const std::size_t numBones = nameArray.attribute("count").as_int();
			m_JointNames.reserve(numBones);
			const std::string_view names = nameArray.child_value();
			std::stringstream ss;
			ss << names;
			std::string name;
			while (ss >> name)
			{
				m_JointNames.emplace_back(name);
			}
		}
	}
}

//=================================================================================
void C_ColladaLoader::LoadJointsInvMatrices(std::map<std::string, S_Joint>& joints, const pugi::xml_node& skinXML, const glm::mat4& normalizinMatrix) const
{
	for (auto xmlSource : skinXML.children("source"))
	{
		const std::string_view id = xmlSource.attribute("id").as_string();
		if (id.find("bind_poses") == id.npos)
		{
			continue;
		}

		if (const auto floatArray = xmlSource.child("float_array"))
		{
			S_FloatArray floats(floatArray);
			int i = 0;
			for (const auto& name : m_JointNames)
			{
				const auto bindMatrix = glm::transpose(normalizinMatrix * floats.Get<glm::mat4>());
				joints.insert({ name, S_Joint(i, name, bindMatrix) });
				++i;
			}
		}
	}
}

//=================================================================================
glm::mat4 C_ColladaLoader::ParseTranslation(const pugi::xml_node& node)
{
	float x, y, z;
	if (auto translation = node.child("translate"))
	{
		std::stringstream ss;
		ss << translation.child_value();
		ss >> x >> y >> z;
		return glm::translate(glm::mat4(1.f), { x,y,z });
	}
	return glm::mat4(1.f);
}

//=================================================================================
bool C_ColladaLoader::ParseChildrenJoints(S_Joint& parent, const pugi::xml_node& xmlParent, const std::map<std::string, S_Joint>& joints)
{
	for (const auto& node : xmlParent.children("node"))
	{
		std::string_view boneName = node.attribute("sid").as_string();
		const auto joint = joints.find(boneName.data());
		if (joint == joints.end())
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Bone '{}' referenced doesn't exiests.", boneName);
			continue;
			//return false;
		}
		auto& includedJoint = parent.m_Children.emplace_back(joint->second);
		if (!ParseChildrenJoints(includedJoint, node, joints))
		{
			return false;
		}
	}
	return true;
}

//=================================================================================
int C_ColladaLoader::GetBoneId(const std::string& name) const
{
	const auto it = std::find(m_JointNames.begin(), m_JointNames.end(), name);
	if (it == m_JointNames.end())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown bone id '{}'", name);
		return -1;
	}
	return std::distance(m_JointNames.begin(), it);
}

//=================================================================================
void C_ColladaLoader::Reset()
{
	m_JointNames.clear();
}

//=================================================================================
C_BoneTimeline C_ColladaLoader::LoadBoneTimeline(const pugi::xml_node& node) const
{
	std::array<S_FloatArray, 2> floatArrays;

	for (const auto& sourceXml : node.children("source"))
	{
		std::string_view id(sourceXml.attribute("id").as_string());
		if (id.find("input") != std::string::npos)
		{
			floatArrays[0] = S_FloatArray(sourceXml.child("float_array"));
		}
		else if (id.find("output") != std::string::npos)
		{
			floatArrays[1] = S_FloatArray(sourceXml.child("float_array"));
		}
	}
	C_BoneTimeline timeline(floatArrays[0].count<float>());
	std::size_t index = 0;
	while (floatArrays[0].EndOfArray() == false)
	{
		timeline.AddBoneKeyFrame(index, S_BoneKeyframe(floatArrays[1].Get<glm::mat4>(), floatArrays[0].Get<float>()));
		++index;
	}

	return timeline;
}

}