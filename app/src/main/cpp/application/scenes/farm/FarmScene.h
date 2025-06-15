#pragma once
// third party
#include <glm/glm.hpp>
// clay
#include <clay/application/common/Resources.h>
#include <clay/application/common/BaseScene.h>
#include <clay/application/xr/CameraControllerXR.h>
#include <clay/gui/xr/ImGuiComponentXR.h>



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


    clay::Entity mSkyBoxEntity;
    clay::Entity mLeftHandEntity_;
    clay::Entity mRightHandEntity_;

    clay::Entity mTreeEntity_;

    clay::Entity mFloorEntity_;

    clay::CameraControllerXR mCameraController_;


    clay::Entity mPlaneEntity_; // imgui

    clay::Resources* mpResources_;
    unsigned int mSelectedSceneIdx = 0;


};
