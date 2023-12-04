The Entity Component System (ECS) primary purpose is to simplify 
creating things in the game, entities, and giving them 
properties, or components, and creating systems that interact with entities 
through their components.

Functionally the ECS comprises following elements:
- Entity
- Entity Manager
- Component
- Component Manger
- System
- System Manager

Each of these elements play a fundamental role in implementing the ECS.
An Entity represents something in the game, a camera, a player, a rock, 
to name a few. An entity is defined by its components. Components are 
collections of like information, with the assumption being that each entry
in a component has the same type of information but potentially different values. 
Examples of components are coordinates for the location of the entity, 
an entities model file location, collision sound effect, physical properties. 
Systems then are common actions that are applied to components when needed such as 
gravity, rendering, movement, or AI. The ECS is designed specifically to ensure 
uniformity and hopefully eliminate duplication of work so that a single gravity 
system can be created to work on all entities that have components that gravity 
should be applied to.


***Strategy for initilizing entities is add a extra bit to the component signiture field to inicate that an entity has been initilized and is not just "alive" but "living". To see if an entity has been fully initilized it's 
component siniture in the component manager will be compared to the entities local component signiture to ensure they match.... some additional thought is needed here.


The interactions between Entities, Components, and systems are handled by their 
respective managers. Maximum number of entities and components should be based off 
of hardware capablities.

Entity
- Unique ID for each entity
- ID for the type of entity (base class)
- List defaults for a component
- Allow custom values for required components

Entity Manager
- Ensures that too many entities are not created
- Entities have unique IDs.
- Map of entities to their IDs?

Component Manager
- Ensure each component gets a unique id
- Limit the total number of allowed components
- Provide initilization for component arrays
- Allow entity data to be added to a component array
- Allow entity data to be removed from the component array
- Destry component arrays
- Interface entites to components
- Retrive information from component array
- Modify component information upon request
- Tracks the component(s) used by an entity and communicates to the system manager when an entity changes the components it uses.

System
- Field to indicate if it will read or write a specific component for parallelization/prioritization?
- System unique ID
- Specifies to the system manager component(s) required for the system to operate on an entity
- List of entities that a system should be operating on (This should improve performance )

System Manager
- Ensures each system has a unique id
- Tracks the "required component" signature for each system
- 

- List used component(s) for each entity

Open questions and things to look into:
- How to implement nested elements?
- Asyncronous Reprojection
- Foviated Rendering
- Voxel entity reduction by merging like voxels and only considering the merged object during rendering and non-modifying interactions.
- How/when to do parallel entity/component management, like creation, destruction, or modification of a large number of component entries simulaneously (like gravity).
- There does not seem there to be a need for systems to have to act upon entities, only to know what components to interact with and where a specific entities data is within the component.





an entity exists purely as an entity ID tracked by the entity
and component managers and by the component data for the entity and the entities
component
signature.
