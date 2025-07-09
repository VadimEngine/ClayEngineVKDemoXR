#pragma once
// third party
#include <glm/glm.hpp>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/application/xr/CameraControllerXR.h>
#include <clay/gui/xr/ImGuiComponentXR.h>
#include <clay/graphics/common/SkyBox.h>
#include <clay/ecs/EntityManager.h>

class FarmScene : public clay::BaseScene {
public:
    FarmScene(clay::BaseApp& app);

    ~FarmScene();

    void initialize() override;

    void update(float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void assembleResources();

    void destroyResources() override;

    clay::ecs::EntityManager mEntityManager_;
    clay::SkyBox mSkyBox_;

    clay::ecs::Entity mSkyBoxEntity;
    clay::ecs::Entity mLeftHandEntity_;
    clay::ecs::Entity mRightHandEntity_;

    clay::ecs::Entity mTreeEntityTop_;
    clay::ecs::Entity mTreeEntityTrunk_;

    clay::ecs::Entity mFloorEntity_;

    clay::ecs::Entity mPlaneEntity_; // imgui

    clay::CameraControllerXR mCameraController_;

    unsigned int mSelectedSceneIdx = 0;
};
