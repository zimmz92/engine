Component
- Unique ID for each component
- Values unique to that component that an entity may need to describe it
- Allow entities to use a component
  - There needs to be a way to add multiple components at one time, this would improve performance when an entity is initially created and adding multiple components, should not be designating what systems may operate on an entites components until they are all defined.
- Allow entities to remove a component
  - There needs to be a way to remvoe multiple components at one time, this would improve performance when an entity is destroyed or evolving created and removing multiple components, should not be designating what systems may operate on an entites components until they are all defined.
- Provide initilization/memory managment for component arrays
  - Have functions that talk with memory management system depending on the memory strategy used for a specific component
- Allow entity data to be added to a component array
- Allow entity data to be removed from the component array
- Destroy component arrays
- Retrive information from component array
- Modify component information upon request

Currently components are arrays of structures with the size of each component array 
being the maximum number of allow entities. Entity IDs are the index into the component 
arrays that specify where their specific data is. Each entity currently has a signature 
that indicate which components are used by that entity type. There is a need to know 
what entities are currently active and valid to allow rapid identification of valid 
component data for a specific or set of entities.

There is potentially a desire to use different memory management depending on the type 
of data that is being stored or accessed. For instance a component where almost all 
entities will have that type of data doing memory allocation for all possible entites 
would be advantagous. However, if there is a component that only a few entities may use 
dynamic memory allocation maybe a better strategy. Perhaps the default is static and the 
possibility to change the type of memory allocation can be given if desired. Optional 
component constructor class.