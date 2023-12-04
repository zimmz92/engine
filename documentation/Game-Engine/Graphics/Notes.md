## Objects/Materials Handling
Trying to figure out how to handle objects with models/materials efficiently. Need to
group objects first by their models, then replicate those groups, or portions of, 
for each of the pipelines(materials+texture) that are required for the different
properties an entity using that model might have. For instance a ghost entity using 
the model of a human may require some transparency so would have to go through a 
different/additional pipeline that a "solid" entity using the human model would not. 
Ideally lists would be made for each render system that would only be updated when a
change has occurred to the model an entity is using, the materials for an entity 
changed, or entities were created or destroyed.

For a render system (pipeline): 
- List of unique models to be rendered (vertex and index buffers)
- List of indexes into object shader storage buffers (pointer to model matrix, normal 
matrix, texture index)

1) Use map for storing a list of entities that use a particular model for each shader with 
their
2) Need to force the use of the "updateData" to ensure I can inform the component manager 
that a entities data has updated
3) Have components track which of their entities have updated
4) Component manager track which of their entities have been deleted, this needs to be
cleared after each execution of all systems
6) Component manager will need a function so systems can get a list of entities that have
been deleted if it matters to the way they operate. Render systems are going to keep a 
map of each of the unique models to be rendered, see above.

Entity with model has updated
models need to track a list of entities that use them
- check if entity's model has changed or is already in the buffer
  - If in the buffer already do nothing
  - If not add the entities model to the list of models to be drawn