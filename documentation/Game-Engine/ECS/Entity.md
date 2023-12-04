# Entity
## Purpose
Provides a standard framework for defining a set of properties that 
describes one or more portions of the game.

## Description
An entity is a set of defined properties which describe something in, used by, 
or required for the game. These properties are organized into, and values are 
stored within, components based on their usage for, or within, the game. For 
instance the x,y,z world position of an object in game may be grouped into one 
component. See the [component page](./Component.md) for more details on components. 
The entity framework requires entity types to be defined specifying the set of 
components which will describe an entity of that type. When an entity of a specific 
entity type is created the values for each of the components describing the entity 
are populated. Since the data for the entity is stored and tracked within applicable
components and since systems act upon a set of components which contain entity data 
an entity as a standalone object is only required to assist with entity 
initialization. As a result an entity manager is used to allocate and track entities,
[entity management](#management) section for more details.

## Lifecycle
The lifecycle of an entity consists of declaration, initialization, usage and 
modification, then destruction. Declaration informs the ECS that a new entity, 
of a specific entity type, is to be created by creating an object of that entity 
type. To assist with entity initialization the components of the entity will be
referenced within the entity object. The usage and modification of an entity is 
handled by ECS systems allowing the entity object used for initialization to be 
deleted. The destruction of an entity is handled by the ECS upon request.

## Properties and Usage
An entity only exists in an object form at creation and during initialization. 
A base entity only has an ID property and a reference to the entity manager 
that assigns the entity an ID and is responsible for coordinating destroying 
an entity upon request. When an entity type is derived from the base entity 
required components are defined and references created to ease the entity 
instantiation process. When and entity of a specific type is created the entity 
manager for the entity assigns it an entity ID. Since the properties of an entity
are actually [components](./Component.md) the entity ID, and component references, 
are the only properties of an entity object. After initialization of the entity the
object can be discarded since the entity manager will manager and track the entity 
by ID which will be used as a way of indexing and identifying component data for 
that entity.

## Entity Management
The primary roles of the entity manager is to allocate entity IDs, enable/disable 
entities, and facilitate entity destruction. Allocating entity IDs occurs by popping
an entity ID off the entity ID stack whenever an entity created. Enabling an entity 
allows [systems](./Systems.md) to act upon, access, or respond to component data 
related to that entity, where disabling an entity prevents this. Entities are 
disabled by default to ensure an entity can be fully initialized prior to 
[systems](./Systems.md) utilizing them. Destruction of entities consist of disabling
an entity, releasing their component data, then, finally, deallocating the entity
ID putting it back on the stack for a new entity to use.
