#include "DemoAppXR.h"
#include "application/scenes/sandbox/SandboxScene.h"
#include "application/scenes/space/SpaceScene.h"
#include "application/scenes/farm/FarmScene.h"


DemoAppXR::DemoAppXR(clay::XRSystem* pXRSystem) : AppXR(pXRSystem) {

}

void DemoAppXR::initialize() {

}

void DemoAppXR::CreateResources() {
    AppXR::CreateResources();

    // shaders
    clay::ShaderModule textureVertShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_VERTEX_BIT,
        loadFileToMemory_XR("shaders/Texture.vert.spv")
    );
    clay::ShaderModule textureFragShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        loadFileToMemory_XR("shaders/Texture.frag.spv")
    );
    // Flat
    clay::ShaderModule flatVertShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_VERTEX_BIT,
        loadFileToMemory_XR("shaders/Flat.vert.spv")
    );
    clay::ShaderModule flatFragShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        loadFileToMemory_XR("shaders/Flat.frag.spv")
    );
    // Solid
    clay::ShaderModule solidVertShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_VERTEX_BIT,
        loadFileToMemory_XR("shaders/Solid.vert.spv")
    );
    clay::ShaderModule solidFragShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        loadFileToMemory_XR("shaders/Solid.frag.spv")
    );

    clay::Resources::Handle<VkSampler> samplerHandle_Default;
    clay::Resources::Handle<VkSampler> samplerHandle_Linear;

    clay::Resources::Handle<clay::Texture> textureHandle_SandboxPreview;
    clay::Resources::Handle<clay::Texture> textureHandle_SpacePreview;
    clay::Resources::Handle<clay::Texture> textureHandle_FarmPreview;
    clay::Resources::Handle<clay::Texture> textureHandle_VTexture;
    clay::Resources::Handle<clay::Texture> textureHandle_Solid;
    clay::Resources::Handle<clay::Texture> textureHandle_Sun;
    clay::Resources::Handle<clay::Texture> textureHandle_Moon;
    clay::Resources::Handle<clay::Texture> textureHandle_Earth;
    clay::Resources::Handle<clay::Texture> textureHandle_Stars;
    clay::Resources::Handle<clay::Texture> textureHandle_Clouds;

    // todo replace with handle (no r)
    clay::Resources::Handle<clay::Mesh> meshHandle_Sphere;
    clay::Resources::Handle<clay::Mesh> meshHandle_Cube;
    clay::Resources::Handle<clay::Mesh> meshHandle_Plane;
    clay::Resources::Handle<clay::Mesh> meshHandle_GloveLeft;
    clay::Resources::Handle<clay::Mesh> meshHandle_GloveRight;

    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureDepth;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureDepthStencil;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureNoDepth;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_Flat;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_SolidStencil;

    clay::Resources::Handle<clay::Material> materialHandle_VTexture;
    clay::Resources::Handle<clay::Material> materialHandle_VTextureStencil;
    clay::Resources::Handle<clay::Material> materialHandle_Solid;
    clay::Resources::Handle<clay::Material> materialHandle_Sun;
    clay::Resources::Handle<clay::Material> materialHandle_Moon;
    clay::Resources::Handle<clay::Material> materialHandle_Earth;
    clay::Resources::Handle<clay::Material> materialHandle_Stars;
    clay::Resources::Handle<clay::Material> materialHandle_CloudySky;
    clay::Resources::Handle<clay::Material> materialHandle_Flat;
    clay::Resources::Handle<clay::Material> materialHandle_SolidStencil;
    clay::Resources::Handle<clay::Material> materialHandle_Imgui;


    {
        // load font
        auto fontData = loadFileToMemory_XR("fonts/runescape_uf.ttf");

        clay::ShaderModule fontVertShader(
            mpGraphicsContext_->getDevice(),
            VK_SHADER_STAGE_VERTEX_BIT,
            loadFileToMemory_XR("shaders/Text.vert.spv")
        );
        clay::ShaderModule fontFragShader(
            mpGraphicsContext_->getDevice(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            loadFileToMemory_XR("shaders/Text.frag.spv")
        );

        mResources_.addResource<clay::Font>(
            clay::Font(*mpGraphicsContext_, fontData, fontVertShader, fontFragShader, *mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_),
            "Runescape"
        );
    }

    mResources_.loadResource<clay::Audio>({"audio/beep_deep_1.wav"}, "DeepBeep");

    {
        // default sampler
        VkSampler sampler;
        VkSamplerCreateInfo samplerInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };

        if (vkCreateSampler(mpGraphicsContext_->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
        samplerHandle_Default = mResources_.addResource(std::move(sampler), "Default");
    }
    {
        // linear sampler
        VkSampler linearSampler;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(mpGraphicsContext_->mPhysicalDevice_, &properties);
        VkSamplerCreateInfo samplerInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = VK_LOD_CLAMP_NONE,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };

        if (vkCreateSampler( mpGraphicsContext_->getDevice(), &samplerInfo, nullptr, &linearSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        samplerHandle_Linear = mResources_.addResource<VkSampler>(std::move(linearSampler), "Linear");
    }
    // SandboxPreview Textures
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/SandboxPreview.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_SandboxPreview = mResources_.addResource(std::move(texture), "SandboxPreview");
    }
    // SpacePreview Textures
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/SpacePreview.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_SpacePreview= mResources_.addResource(std::move(texture), "SpacePreview");
    }
    // FarmPreview Textures
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/FarmPreview.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_FarmPreview = mResources_.addResource(std::move(texture), "FarmPreview");
    }
    // VImage Textures
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/V.png");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_VTexture = mResources_.addResource(std::move(texture), "VTexture");
    }
    // solid texture
    {
        // solid image
        clay::utils::ImageData singleRGBA;
        singleRGBA.width = 1;
        singleRGBA.height = 1;
        singleRGBA.channels = 4; // RGBA
        singleRGBA.pixels = std::make_unique<unsigned char[]>(4);

        singleRGBA.pixels[0] = 255;
        singleRGBA.pixels[1] = 255;
        singleRGBA.pixels[2] = 255;
        singleRGBA.pixels[3] = 255;

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(singleRGBA);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Solid = mResources_.addResource(std::move(texture), "SolidTexture");
    }
    // Sun texture
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/sunmap.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Linear]);

        textureHandle_Sun = mResources_.addResource(std::move(texture), "Sun");
    }
    // Moon texture
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/2k_moon.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Linear]);

        textureHandle_Moon = mResources_.addResource(std::move(texture), "Moon");
    }
    // Earth texture
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/earthmap1k.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Linear]);

        textureHandle_Earth= mResources_.addResource(std::move(texture), "Earth");
    }
    // Stars
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/8k_stars_milky_way.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Linear]);

        textureHandle_Stars = mResources_.addResource(std::move(texture), "Stars");
    }
    // Cloudy sky
    {
        clay::utils::ImageData imageData = loadImageFileToMemory_XR("textures/CloudSky.jpg");
        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Linear]);

        textureHandle_Clouds = mResources_.addResource(std::move(texture), "CloudySky");
    }
    // sphere mesh
    {
        meshHandle_Sphere = mResources_.loadResource<clay::Mesh>({"models/Sphere.obj"}, "Sphere");
    }
    // cube mesh
    {
        meshHandle_Cube = mResources_.loadResource<clay::Mesh>({"models/Cube.obj"}, "Cube");
    }
    // plane mesh
    {
        meshHandle_Plane = mResources_.loadResource<clay::Mesh>({"models/Plane.obj"}, "Plane");
    }
    // hand meshes
    {
        meshHandle_GloveLeft = mResources_.loadResource<clay::Mesh>({"models/GloveLeft.obj"}, "GloveLeft");
        meshHandle_GloveRight = mResources_.loadResource<clay::Mesh>({"models/GloveRight.obj"}, "GloveRight");
    }

    // pipeline (TextureDepth)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &textureVertShader, &textureFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        pipelineHandle_TextureDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureDepth"
        );
    }
    // pipeline (TextureDepthStencil)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &textureVertShader, &textureFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        VkStencilOpState stencilState = {};
        stencilState.failOp = VK_STENCIL_OP_KEEP;
        stencilState.passOp = VK_STENCIL_OP_REPLACE;
        stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
        stencilState.compareOp = VK_COMPARE_OP_ALWAYS;  // Equivalent to glStencilFunc(ALWAYS, ref, mask)
        stencilState.compareMask = 0xFF;
        stencilState.writeMask = 0xFF;
        stencilState.reference = 0xFF; // Set at draw time via vkCmdSetStencilReference if dynamic

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_TRUE,
            .front = stencilState,
            .back = stencilState
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        pipelineHandle_TextureDepthStencil = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureDepthStencil"
        );
    }
    // pipeline (TextureNoDepth)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &textureVertShader, &textureFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        pipelineHandle_TextureNoDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureNoDepth"
        );
    }
    // pipeline (flat)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &flatVertShader, &flatFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
        };

        pipelineHandle_Flat = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "Flat"
        );
    }
    // pipeline (SolidStencil)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &solidVertShader, &solidFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        VkStencilOpState stencilState = {};
        stencilState.failOp = VK_STENCIL_OP_KEEP;
        stencilState.passOp = VK_STENCIL_OP_KEEP;
        stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
        stencilState.compareOp = VK_COMPARE_OP_NOT_EQUAL;
        stencilState.compareMask = 0xFF;
        stencilState.writeMask = 0x00;
        stencilState.reference = 0xFF;

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_TRUE,
            .front = stencilState,
            .back = stencilState
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
        };

        pipelineHandle_SolidStencil = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "SolidStencil"
        );
    }

    // material
    // V Texture
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_VTexture].getSampler(),
                .imageView = mResources_[textureHandle_VTexture].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };
        materialHandle_VTexture = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "VTexture"
        );
    }
    // TextureDepthStencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepthStencil]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_VTexture].getSampler(),
                .imageView = mResources_[textureHandle_VTexture].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_VTextureStencil = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "VTextureStencil"
        );
    }
    // Single White
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Solid].getSampler(),
                .imageView = mResources_[textureHandle_Solid].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Solid = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "SolidTexture"
        );
    }
    // Sun
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Sun].getSampler(),
                .imageView = mResources_[textureHandle_Sun].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Sun = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Sun"
        );
    }
    // Moon
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Moon].getSampler(),
                .imageView = mResources_[textureHandle_Moon].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Moon = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Moon"
        );
    }
    // Earth
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Earth].getSampler(),
                .imageView = mResources_[textureHandle_Earth].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Earth = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Earth"
        );
    }
    // Stars
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureNoDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mHeadLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Stars].getSampler(),
                .imageView = mResources_[textureHandle_Stars].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Stars = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Stars"
        );
    }
    // cloudy
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureNoDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mHeadLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Clouds].getSampler(),
                .imageView = mResources_[textureHandle_Clouds].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_CloudySky = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "CloudySky"
        );
    }
    // Flat
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_Flat]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };

        materialHandle_Flat = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Flat"
        );
    }
    // solid-stencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_SolidStencil]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };

        materialHandle_SolidStencil = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "SolidStencil"
        );
    }
    // imgui frame
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[samplerHandle_Linear],
                .imageView = imguiImageView,
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Imgui = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "ImguiFrame"
        );
    }

    // Models
    // solid sphere
    {
       clay::Model solidSphereModel(*mpGraphicsContext_);
        solidSphereModel.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(solidSphereModel), "SolidSphere");
    }
    // v sphere
    {
       clay::Model vSphereModel(*mpGraphicsContext_);
        vSphereModel.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_VTexture],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(vSphereModel), "VSphere");
    }
    // v sphere-stencil
    {
        clay::Model vSphereModel(*mpGraphicsContext_);

        vSphereModel.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_VTextureStencil],
          glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(vSphereModel), "VSphereStencil");
    }
    // v sphere solid highlight
    {
        clay::Model vSphereModel(*mpGraphicsContext_);
        vSphereModel.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_SolidStencil],
          glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(vSphereModel), "VSphereSolid");
    }
    // sun
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Sun],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(model), "Sun");
    }
    // moon
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Moon],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(model), "Moon");
    }
    // earth
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Earth],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(model), "Earth");
    }
    // skybox (star)
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_Stars],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(model), "StarSkybox");
    }
    // skybox (cloudy)
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Sphere],
            &mResources_[materialHandle_CloudySky],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(model), "CloudySkybox");
    }
    // Hands
    {
        clay::Model leftHandModel(*mpGraphicsContext_);
        leftHandModel.addElement({
            &mResources_[meshHandle_GloveLeft],
            &mResources_[materialHandle_Flat],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(leftHandModel), "GloveLeft");

       clay::Model rightHandModel(*mpGraphicsContext_);
        rightHandModel.addElement({
            &mResources_[meshHandle_GloveRight],
            &mResources_[materialHandle_Flat],
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(rightHandModel), "GloveRight");
    }
    // create leaves model
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
             &mResources_[meshHandle_Sphere],
             &mResources_[materialHandle_Flat],
         });
        mResources_.addResource<clay::Model>(std::move(model), "TreeTop");
    }
    // create trunk model
    {

        clay::Model model(*mpGraphicsContext_);
        model.addElement({
             &mResources_[meshHandle_Cube],
             &mResources_[materialHandle_Flat],
         });
        mResources_.addResource<clay::Model>(std::move(model), "TreeTrunk");
    }
    // floor model
    {
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandle_Plane],
            &mResources_[materialHandle_Flat],
         });
        mResources_.addResource<clay::Model>(std::move(model), "GrassFloor");
    }
    // imgui
    {
        clay::Model imguiPlane(*mpGraphicsContext_);
        imguiPlane.addElement({
            &mResources_[meshHandle_Plane],
            &mResources_[materialHandle_Imgui],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(imguiPlane), "ImguiPlane");
    }

    mSandboxImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_[textureHandle_SandboxPreview].getSampler(),
        mResources_[textureHandle_SandboxPreview].getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mSpaceImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_[textureHandle_SpacePreview].getSampler(),
        mResources_[textureHandle_SpacePreview].getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mFarmImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_[textureHandle_FarmPreview].getSampler(),
        mResources_[textureHandle_FarmPreview].getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    // scene details
    {
        mSceneDetails_ = {
            {
                "Sandbox Scene",
                "Scene for experimenting features.",
                mSandboxImguiTextureId,
                [this]{
                    setScene(new SandboxScene(*this));
                }
            },
            {
                "Space Scene",
                "Space simulation with an orbiting solar system.",
                mSpaceImguiTextureId,
                [this]{
                    setScene(new SpaceScene(*this));
                }
            },
            {
                "Farm Scene",
                "Simple Nature Scene with future plans to include farming.",
                mFarmImguiTextureId,
                [this]{
                    setScene(new FarmScene(*this));
                }
            }
        };
    }

    mScenes_.front()->initialize();
}