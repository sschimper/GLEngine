#pragma once

#include <Core/CoreMacros.h>

#include <Entity/IEntity.h>

#include <vector>
#include <memory>
#include <string>

namespace GLEngine {

// forward declaration
namespace Physics {
namespace Primitives {
class C_Frustum;
}
}

namespace Entity {

class API_EXPORT C_World {
public:
	C_World();
	~C_World();
	std::vector<std::shared_ptr<I_Entity>> GetEntities(Physics::Primitives::C_Frustum frust);
	void AddEntity(std::shared_ptr<I_Entity> entity);

private:
	std::vector<std::shared_ptr<I_Entity>>* m_Entities;
};

}
}