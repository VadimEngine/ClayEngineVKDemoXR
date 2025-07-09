#pragma once
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/application/xr/AppXR.h>
#include <clay/gui/xr/ImGuiComponentXR.h>
#include <clay/application/xr/CameraControllerXR.h>
#include <clay/ecs/EntityManager.h>

class SandboxScene : public clay::BaseScene {
public:
    SandboxScene(clay::BaseApp& app);

    ~SandboxScene();

    void initialize() override;

    void update(float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void assembleResources();

    void destroyResources() override;

    clay::ecs::EntityManager mEntityManager_;

    clay::ecs::Entity mCenterSphere_;

    clay::ecs::Entity mTexturedSphere_;
    clay::ecs::Entity mTexturedSphereStencil_;
    clay::ecs::Entity mTexturedSphereSolid_;

    clay::ecs::Entity mPlaneEntity_;

    // TODO make hand(s) class to encapsulate the hand logic
    clay::ecs::Entity mLeftHandEntity_;
    clay::ecs::Entity mRightHandEntity_;

    clay::ecs::Entity mTextEntity_;

    unsigned int mSelectedSceneIdx = 0;

    clay::CameraControllerXR mCameraController_;

    bool mHighLight_ = false;

    clay::Audio* mpBeepDeepAudio_ = nullptr;

};

