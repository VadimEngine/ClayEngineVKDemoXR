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
    auto textureVertexFileData = loadFileToMemory_XR("shaders/Texture.vert.spv");
    VkShaderModule textureVertexShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_VERTEX_BIT, textureVertexFileData.data.get(), textureVertexFileData.size}
    );
    auto textureFragmentFileData = loadFileToMemory_XR("shaders/Texture.frag.spv");
    VkShaderModule textureFragmentShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_FRAGMENT_BIT, textureFragmentFileData.data.get(), textureFragmentFileData.size}
    );

    auto flatVertexFileData = loadFileToMemory_XR("shaders/Flat.vert.spv");
    VkShaderModule flatVertexShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_VERTEX_BIT, flatVertexFileData.data.get(), flatVertexFileData.size}
    );
    auto flatFragmentFileData = loadFileToMemory_XR("shaders/Flat.frag.spv");
    VkShaderModule flatFragmentShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_FRAGMENT_BIT, flatFragmentFileData.data.get(), flatFragmentFileData.size}
    );

    auto solidVertexFileData = loadFileToMemory_XR("shaders/Solid.vert.spv");
    VkShaderModule solidVertexShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_VERTEX_BIT, solidVertexFileData.data.get(), solidVertexFileData.size}
    );
    auto solidFragmentFileData = loadFileToMemory_XR("shaders/Solid.frag.spv");
    VkShaderModule solidFragmentShader = mpGraphicsContext_->createShader(
        {VK_SHADER_STAGE_FRAGMENT_BIT, solidFragmentFileData.data.get(), solidFragmentFileData.size}
    );

    {
        // load font
        auto fontData = loadFileToMemory_XR("fonts/runescape_uf.ttf");
        auto vertexData = loadFileToMemory_XR("shaders/Text.vert.spv");
        auto fragmentData = loadFileToMemory_XR("shaders/Text.frag.spv");

        mResources_.addResource<clay::Font>(
            std::make_unique<clay::Font>(
                *mpGraphicsContext_, fontData, vertexData, fragmentData, *mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_
            ),
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
        mResources_.addResource<VkSampler>(
            std::make_unique<VkSampler>(sampler), "Default"
        );
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

        mResources_.addResource<VkSampler>(
            std::make_unique<VkSampler>(linearSampler),
            "Linear"
        );
    }
    // SandboxPreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/SandboxPreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "SandboxPreview");
    }
    // SpacePreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/SpacePreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "SpacePreview");
    }
    // FarmPreview Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/FarmPreview.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "FarmPreview");
    }
    // VImage Textures
    {
        auto vImageData = loadImageFileToMemory_XR("textures/V.png");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "VTexture");
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

        auto* pSolidTexture = new clay::Texture(*mpGraphicsContext_);
        pSolidTexture->initialize(singleRGBA);
        pSolidTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pSolidTexture), "SolidTexture");
    }
    // Sun texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/sunmap.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Linear"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "Sun");
    }
    // Moon texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/2k_moon.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Linear"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "Moon");
    }
    // Earth texture
    {
        auto vImageData = loadImageFileToMemory_XR("textures/earthmap1k.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Linear"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "Earth");
    }
    // Stars
    {
        auto vImageData = loadImageFileToMemory_XR("textures/8k_stars_milky_way.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Linear"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "Stars");
    }
    // Cloudy sky
    {
        auto vImageData = loadImageFileToMemory_XR("textures/CloudSky.jpg");
        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(vImageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Linear"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "CloudySky");
    }
    // sphere mesh
    {
        mResources_.loadResource<clay::Mesh>({"models/Sphere.obj"}, "Sphere");
    }
    // cube mesh
    {
        mResources_.loadResource<clay::Mesh>({"models/Cube.obj"}, "Cube");
    }
    // plane mesh
    {
        mResources_.loadResource<clay::Mesh>({"models/Plane.obj"}, "Plane");
    }
    // hand meshes
    {
        mResources_.loadResource<clay::Mesh>({"models/GloveLeft.obj"}, "GloveLeft");
        mResources_.loadResource<clay::Mesh>({"models/GloveRight.obj"}, "GloveRight");
    }

    // pipeline (TextureDepth)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = textureVertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = textureFragmentShader,
                .pName = "main"
            }
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
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepth"
        );

    }
    // pipeline (TextureDepthStencil)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = textureVertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = textureFragmentShader,
                .pName = "main"
            }
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

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepthStencil"
        );
    }
    // pipeline (TextureNoDepth)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = textureVertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = textureFragmentShader,
                .pName = "main"
            }
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

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureNoDepth"
        );
    }
    // pipeline (flat)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = flatVertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = flatFragmentShader,
                .pName = "main"
            }
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

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "Flat"
        );
    }
    // pipeline (SolidStencil)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = solidVertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = solidFragmentShader,
                .pName = "main"
            }
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

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "SolidStencil"
        );
    }

    // material
    // V Texture
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("VTexture")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("VTexture")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTexture"
        );
    }
    // TextureDepthStencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepthStencil")
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
                .sampler = mResources_.getResource<clay::Texture>("VTexture")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("VTexture")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTextureStencil"
        );
    }
    // Single White
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("SolidTexture")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("SolidTexture")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidTexture"
        );
    }
    // Sun
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("Sun")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("Sun")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Sun"
        );
    }
    // Moon
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("Moon")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("Moon")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Moon"
        );
    }
    // Earth
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("Earth")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("Earth")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Earth"
        );
    }
    // Stars
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureNoDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("Stars")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("Stars")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Stars"
        );
    }
    // cloudy
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureNoDepth")
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
                .sampler = mResources_.getResource<clay::Texture>("CloudySky")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("CloudySky")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "CloudySky"
        );
    }
    // Flat
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("Flat")
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "Flat"
        );
    }
    // solid-stencil
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("SolidStencil")
        };

        matConfig.bufferBindings = {
            {
                .buffer = mXRSystem_->mpGraphicsContext_->mWorldLockedCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidStencil"
        );
    }
    // imgui frame
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
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
                .sampler = *mResources_.getResource<VkSampler>("Linear"),
                .imageView = imguiImageView,
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "ImguiFrame"
        );
    }

    // Models
    // solid sphere
    {
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("SolidTexture"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "SolidSphere");
    }
    // v sphere
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->addElement({
          mResources_.getResource<clay::Mesh>("Sphere"),
          mResources_.getResource<clay::Material>("VTexture"),
          glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pVSphereModel), "VSphere");
    }
    // v sphere-stencil
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);

        pVSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("VTextureStencil"),
          glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pVSphereModel), "VSphereStencil");
    }
    // v sphere solid highlight
    {
        std::unique_ptr<clay::Model> pVSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pVSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("SolidStencil"),
          glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pVSphereModel), "VSphereSolid");
    }
    // sun
    {
        std::unique_ptr<clay::Model> pModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("Sun"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pModel), "Sun");
    }
    // moon
    {
        std::unique_ptr<clay::Model> pModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("Moon"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pModel), "Moon");
    }
    // earth
    {
        std::unique_ptr<clay::Model> pModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("Earth"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pModel), "Earth");
    }
    // skybox (star)
    {
        std::unique_ptr<clay::Model> pModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("Stars"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pModel), "StarSkybox");
    }
    // skybox (cloudy)
    {
        std::unique_ptr<clay::Model> pModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("CloudySky"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pModel), "CloudySkybox");
    }
    // Hands
    {
        std::unique_ptr<clay::Model> pLeftHandModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pLeftHandModel->addElement({
            mResources_.getResource<clay::Mesh>("GloveLeft"),
            mResources_.getResource<clay::Material>("Flat"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pLeftHandModel), "GloveLeft");

        std::unique_ptr<clay::Model> pRightHandModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pRightHandModel->addElement({
            mResources_.getResource<clay::Mesh>("GloveRight"),
            mResources_.getResource<clay::Material>("Flat"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pRightHandModel), "GloveRight");
    }
    // imgui
    {
        std::unique_ptr<clay::Model> pImguiPlane = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pImguiPlane->addElement({
            mResources_.getResource<clay::Mesh>("Plane"),
            mResources_.getResource<clay::Material>("ImguiFrame"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pImguiPlane), "ImguiPlane");
    }

    mSandboxImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_.getResource<clay::Texture>("SandboxPreview")->getSampler(),
        mResources_.getResource<clay::Texture>("SandboxPreview")->getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mSpaceImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_.getResource<clay::Texture>("SpacePreview")->getSampler(),
        mResources_.getResource<clay::Texture>("SpacePreview")->getImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    mFarmImguiTextureId = ImGui_ImplVulkan_AddTexture(
        mResources_.getResource<clay::Texture>("FarmPreview")->getSampler(),
        mResources_.getResource<clay::Texture>("FarmPreview")->getImageView(),
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

    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), textureVertexShader, nullptr);
    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), textureFragmentShader, nullptr);

    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), flatVertexShader, nullptr);
    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), flatFragmentShader, nullptr);

    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), solidVertexShader, nullptr);
    vkDestroyShaderModule(mpGraphicsContext_->getDevice(), solidFragmentShader, nullptr);

    mScenes_.front()->initialize();
}