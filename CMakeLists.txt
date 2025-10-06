# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

# Activez Rechargement à chaud pour les compilateurs MSVC si cela est pris en charge.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("OpenGL_Renderer")

project (OpenGL_Renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VCPKG_MANIFEST_MODE ON CACHE BOOL "")


find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_executable(OGL_APP
    # Main entry point
    src/main.cpp
    
    # Core systems
    src/core/Window.h
    src/core/Window.cpp
    src/core/Camera.h
    src/core/Camera.cpp
    src/core/TimeManager.h
    src/core/TimeManager.cpp
    src/core/Viewport.h
    src/core/Viewport.cpp
    
    # Rendering core
    src/rendering/Renderer.h
    src/rendering/Renderer.cpp
    src/rendering/ForwardRenderer.h
    src/rendering/ForwardRenderer.cpp
    src/rendering/DeferredRenderer.h
    src/rendering/DeferredRenderer.cpp
    src/rendering/PathTraceRenderer.h
    src/rendering/PathTraceRenderer.cpp
    src/rendering/RenderPass.h
    src/rendering/RenderPass.cpp
    src/rendering/GlobalShadingModel.h
    src/rendering/GlobalShadingModel.cpp
    src/rendering/BlendManager.h
    # src/rendering/BlendManager.cpp
    src/rendering/RayTracingStructs.h
    src/rendering/RayTracingStructs.cpp
    src/rendering/IShadingModel.h
    src/rendering/IShadingModel.cpp
    
    
    # Shaders
    src/rendering/shaders/ShaderLibrary.h
    src/rendering/shaders/ShaderLibrary.cpp
    src/rendering/shaders/shaderClass.h
    src/rendering/shaders/shaderClass.cpp
    src/rendering/shaders/ComputeShader.h
    src/rendering/shaders/ComputeShader.cpp
    
    # Buffers
    src/rendering/buffers/VAO.h
    src/rendering/buffers/VAO.cpp
    src/rendering/buffers/VBO.h
    src/rendering/buffers/VBO.cpp
    src/rendering/buffers/EBO.h
    src/rendering/buffers/EBO.cpp
    src/rendering/buffers/FBO.h
    src/rendering/buffers/FBO.cpp
    src/rendering/buffers/ComputeBuffer.h
    src/rendering/buffers/ComputeBuffer.cpp
    src/rendering/buffers/ComputeImage.h
    src/rendering/buffers/ComputeImage.cpp
    src/rendering/buffers/FramebufferFactory.h
    src/rendering/buffers/FramebufferFactory.cpp
    src/rendering/buffers/IFramebuffer.h
    src/rendering/buffers/IFramebuffer.cpp
    src/rendering/buffers/MSAAFBO.h
    src/rendering/buffers/MSAAFBO.cpp
    src/rendering/buffers/SMFBO.h
    src/rendering/buffers/SMFBO.cpp
    
    # Textures
    src/rendering/textures/Texture.h
    src/rendering/textures/Texture.cpp
    src/rendering/textures/Texture3D.h
    src/rendering/textures/Texture3D.cpp
    src/rendering/textures/TextureArray.h
    src/rendering/textures/TextureArray.cpp
    src/rendering/textures/Cubemap.h
    src/rendering/textures/Cubemap.cpp
    
    # Materials
    src/rendering/materials/Material.h
    src/rendering/materials/Material.cpp
    src/rendering/materials/MaterialManager.h
    src/rendering/materials/MaterialManager.cpp
    src/rendering/materials/RayTracingMaterial.h
    src/rendering/materials/RayTracingMaterial.cpp

    src/rendering/materials/LocalShadingModel.h
    src/rendering/materials/LocalShadingModel.cpp
    
    # Lighting
    src/rendering/lighting/Light.h
    src/rendering/lighting/Light.cpp
    src/rendering/lighting/LightManager.h
    src/rendering/lighting/LightManager.cpp
    src/rendering/lighting/DirectionalLight.h
    src/rendering/lighting/DirectionalLight.cpp
    src/rendering/lighting/PointLight.h
    src/rendering/lighting/PointLight.cpp
    src/rendering/lighting/SpotLight.h
    src/rendering/lighting/SpotLight.cpp
    src/rendering/lighting/ShadowMap.h
    src/rendering/lighting/ShadowMap.cpp
    
    # Post-processing
    src/rendering/postprocessing/PostProcesser.h
    src/rendering/postprocessing/PostProcesser.cpp
    src/rendering/postprocessing/PostProcessingStack.h
    src/rendering/postprocessing/PostProcessingStack.cpp
    src/rendering/postprocessing/PostProcessEffect.h
    src/rendering/postprocessing/PostProcessEffect.cpp
    src/rendering/postprocessing/IPostProcessEffect.h
    src/rendering/postprocessing/IPostProcessEffect.cpp
    
    
    # Components
    src/components/Entity3D.h
    src/components/Entity3D.cpp
    src/components/Transform.h
    src/components/Transform.cpp
    src/components/Component.h
    src/components/Component.cpp
    src/components/TerrainComponent.h
    src/components/TerrainComponent.cpp
    src/components/Heightmap.h
    src/components/Heightmap.cpp
    src/components/TerrainStructs.h
    src/components/TerrainStructs.cpp
    src/components/TerrainBlendModes.h
    src/components/TerrainBlendModes.cpp
    src/components/CommonRessources.h
    src/components/CommonRessources.cpp
    src/components/Mesh.h
    src/components/Mesh.cpp
    src/components/MeshData.h
    src/components/MeshData.cpp
    src/components/Model.h
    src/components/Model.cpp
    
    
    
    # Scenes
    src/scenes/SceneBase.h
    src/scenes/SceneBase.cpp
    src/scenes/Scene.h
    src/scenes/Scene.cpp
    src/scenes/samplescenes/BasicRenderingScene.h
    src/scenes/samplescenes/BasicRenderingScene.cpp
    src/scenes/SceneManager.h
    src/scenes/SceneManager.cpp
)



target_include_directories(OGL_APP PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/buffers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/textures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/materials
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/lighting
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/postprocessing
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/src/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scenes
)

# Link vcpkg libraries + Vulkan
target_link_libraries(OGL_APP PRIVATE 
    glfw
    glm::glm
    glad::glad
    assimp::assimp
    imgui::imgui
)

# Copy assets to build directory
file(COPY assets/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

# Set properties for Windows
if(WIN32)
    target_compile_definitions(OGL_APP PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()