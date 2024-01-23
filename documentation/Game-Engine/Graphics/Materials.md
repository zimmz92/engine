# Materials


### Material

Materials are a collection of [Material Layers](#Material-Layer) which together depict what an 
object will look like when rendered. A Material Layer provides a framework around a
graphics pipeline and provides entities a component in which to specify the textures and properties
required by the graphics pipeline, and a system which implements draw functionality for the 
pipeline[^1]. A graphics pipeline is defined by various pipeline settings and shaders and is a single 
pass of operations that is performed to render an entity.



### Material Layer

A material layer is the implementation of a graphics pipeline which applies a visual effect to an object based 
on the shaders, and pipeline settings, specified for the material layer. A material layer implements an 
Entity-Component-System (ECS) [component](../ECS/Component) for entities to specify which textures correspond with 
the specific material layer. The material layer also implements an ECS [system](../ECS/System) which is responsible 
for tracking the unique models currently being rendered, and which entities use them, and for submitting indirect draw 
calls for each of the unique models.


#### Material Layer Creation
A material layer is derived from a material layer type. 

A material layer type is defined by the types of entities that a material layer of this type will be rendering
and by the number textures required by each of the pipeline's shaders. The entity type compatibility
is specified by deriving a material layer type from an Ae\*MaterialLayerType class, where \* represents
the entity type compatibility, for instance Ae3DMaterialLayerType for 3D entities. The Ae\*MaterialLayerType 
classes are template classes that, when derived, must be provided the number of textures for the vertex, 
fragment, tessellation, and geometry shaders, in that order. 

The code below is an example of deriving a 
new material type that only requires a single texture for the fragment shader.

``` cpp
class ExampleMaterialLayerType : public Ae3DMaterialLayerType<0,1,0,0>{
public:
    ExampleMaterialLayerType(AeDevice &t_aeDevice,
                             GameComponents& t_game_components,
                             VkRenderPass t_renderPass,
                             ae_ecs::AeECS& t_ecs,
                             MaterialShaderFiles& t_materialShaderFiles,
                             std::vector<VkDescriptorSetLayout>& t_descriptorSetLayouts):
            Ae3DMaterialLayerType(t_aeDevice,
                                  t_game_components,
                                  t_renderPass,
                                  t_ecs,
                                  t_materialShaderFiles,
                                  t_descriptorSetLayouts) {};
    ~ExampleMaterialLayerType()=default;
};
```

A material layer is then derived from the material type specifying which shaders, renderer, and descriptor set layouts, 
are required to implement the material.

``` cpp
MaterialShaderFiles simpleMaterialShaderFiles = {"engines/graphics/shaders/simple.vert.spv",
                                                         "engines/graphics/shaders/simple.frag.spv",
                                                         "Not Used",
                                                         "Not Used"};
ExampleMaterialLayerType m_simpleMaterial{m_device,
                                          m_game_components,
                                          m_renderPass,
                                          m_ecs,
                                          simpleMaterialShaderFiles,
                                          m_descriptorSetLayouts};
```


A material layer has the following base properties:

- Name (m_materialName) - "JON-DOE" is the default.
- Pipeline Layout (m_computePipelineLayout) - The layout of the pipeline this material utilizes.
- Pipeline (m_pipeline) - The pipeline that implements this material.
- Shader Stage Files (m_materialShaderFiles) - The shader files that define the material.
- ID (m_materialID) - The ID assigned to the material.

### References
[^1]: <a>https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Introduction</a>