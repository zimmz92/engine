#include "ae_ecs.hpp"
#include "testComponent.hpp"

namespace ae {
	class ObjectEntity : public AeEntity<ObjectEntity> {

	public:
		// Function to create an entity
		ObjectEntity(AeComponentManager& t_componentManager, AeEntityManager& t_entityManager, testComponentStructure t_testComponentDefinition);

		// Function to destroy an entity
		~ObjectEntity() {};


	private:


	protected:


	};

};