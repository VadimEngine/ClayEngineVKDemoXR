// clay
#include <clay/entity/render/ModelRenderable.h>
// project
#include "application/DemoAppXR.h"
// class
#include "FarmScene.h"

FarmScene::FarmScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mCameraController_(mpFocusCamera_) {}

FarmScene::~FarmScene() {}

void FarmScene::initialize() {
    assembleResources();
    {
        // left
        auto* handLeftRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("GloveLeft")
        );
        handLeftRenderable->setScale({0.2f, 0.2f, 0.2f});
        handLeftRenderable->setColor({.95f, .674f, .411f, 1.0f});
        mLeftHandEntity_.addRenderable(handLeftRenderable);

        // right
        auto* handRightRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("GloveRight")
        );
        handRightRenderable->setScale({0.2f, 0.2f, 0.2f});
        handRightRenderable->setColor({.95f, .674f, .411f, 1.0f});
        mRightHandEntity_.addRenderable(handRightRenderable);
    }
    // imgui plane
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("ImguiPlane")
        );
        mPlaneEntity_.addRenderable(modelRenderable);
        mPlaneEntity_.setPosition({2,0,0});
        mPlaneEntity_.getOrientation() *= glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mPlaneEntity_.getOrientation() *= glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    // tree
    {
        auto* treeTopRenderable = new clay::ModelRenderable(
            mpResources_->getResource<clay::Model>("TreeTop")
        );
        treeTopRenderable->setPosition({0,1,0});
        treeTopRenderable->setColor({0.0f, 1.0f, 0.0f, 1.0f});

        auto* treeTrunkRenderable = new clay::ModelRenderable(
            mpResources_->getResource<clay::Model>("TreeTrunk")
        );
        treeTrunkRenderable->setScale({.1, 2, .1});
        treeTrunkRenderable->setColor({0.239, 0.141, 0.071, 1.0f});

        mTreeEntity_.addRenderable(treeTopRenderable);
        mTreeEntity_.addRenderable(treeTrunkRenderable);

        mTreeEntity_.setPosition({0,0,0});
        mTreeEntity_.setScale({1,1,1});
    }
    // floor
    {
        auto* renderable = new clay::ModelRenderable(
            mpResources_->getResource<clay::Model>("GrassFloor")
        );
        renderable->setColor({0.0f, 1.0f, 0.0f, 1.0f});
        mFloorEntity_.addRenderable(renderable);
        mFloorEntity_.setPosition({0,-1,0});
        mFloorEntity_.setScale({50,50,50});
    }
    // skybox
    {
        auto* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("CloudySkybox")
        );
        mSkyBoxEntity.addRenderable(modelRenderable);
        mSkyBoxEntity.setPosition({0,0,0});
        mSkyBoxEntity.setScale({1,1,1});
    }
}

void FarmScene::update(float dt) {
    const auto joyDirLeft = ((clay::AppXR&)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::LEFT);
    const auto joyDirRight = ((clay::AppXR&)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::RIGHT);

    const auto& rightHandPose = ((clay::AppXR &) mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    const glm::quat rightHandOrientation(rightHandPose.orientation.w, rightHandPose.orientation.x, rightHandPose.orientation.y, rightHandPose.orientation.z);
    glm::vec3 rightHandPosition = glm::vec3(rightHandPose.position.x,rightHandPose.position.y,rightHandPose.position.z);

    const auto& leftHandPose = ((clay::AppXR&)mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::LEFT);
    const glm::quat leftHandOrientation(leftHandPose.orientation.w, leftHandPose.orientation.x, leftHandPose.orientation.y, leftHandPose.orientation.z);
    glm::vec3 leftHandPosition = glm::vec3(leftHandPose.position.x, leftHandPose.position.y, leftHandPose.position.z);
    const auto headPose = ((clay::AppXR&)mApp_).getInputHandler().getHeadPose();

    // update camera with input
    mCameraController_.updateWithJoystickInput(
        {joyDirLeft.x, joyDirLeft.y},
        {joyDirRight.x, joyDirRight.y},
        0.01f * 2.0f,
        1.0f/2.0f * 2.0f,
        headPose
    );
    mSkyBoxEntity.setOrientation(glm::conjugate(mpFocusCamera_->getOrientation()));

    const glm::vec3 cameraPosition = mpFocusCamera_->getPosition();
    const glm::quat cameraOrientation = mpFocusCamera_->getOrientation();

    // Rotate hands to match camera
    const glm::vec3 rotatedRight = cameraOrientation * rightHandPosition;
    const glm::vec3 rotatedLeft = cameraOrientation * leftHandPosition;

    rightHandPosition = cameraPosition + rotatedRight;
    leftHandPosition = cameraPosition  + rotatedLeft;
    const glm::vec3 rightHandForward = glm::normalize(rightHandOrientation * glm::conjugate(mpFocusCamera_->getOrientation()) * glm::vec3{0, 0, -1});

    {
        // update left hand
        mLeftHandEntity_.setOrientation(cameraOrientation * leftHandOrientation);
        mLeftHandEntity_.setPosition(leftHandPosition);
    }
    {
        // update right hand
        mRightHandEntity_.setOrientation(cameraOrientation * rightHandOrientation);
        mRightHandEntity_.setPosition(rightHandPosition);
    }
}

void FarmScene::render(VkCommandBuffer cmdBuffer) {
    mSkyBoxEntity.render(cmdBuffer);
    mTreeEntity_.render(cmdBuffer);
    mFloorEntity_.render(cmdBuffer);

    mPlaneEntity_.render(cmdBuffer); // imgui

    mLeftHandEntity_.render(cmdBuffer);
    mRightHandEntity_.render(cmdBuffer);
}

void FarmScene::renderGUI(VkCommandBuffer cmdBuffer) {
    const uint32_t imguiWidth = 4128;
    const uint32_t imguiHeight = 2208;

    // todo see if pointing at plane
    const auto& rightHandPose = ((clay::AppXR &) mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    const glm::quat rightHandOrientation(rightHandPose.orientation.w, rightHandPose.orientation.x, rightHandPose.orientation.y, rightHandPose.orientation.z);
    glm::vec3 rightHandPosition = glm::vec3(rightHandPose.position.x,rightHandPose.position.y,rightHandPose.position.z);

    const glm::vec3 cameraPosition = mpFocusCamera_->getPosition();
    const glm::quat cameraOrientation = mpFocusCamera_->getOrientation();

    const glm::vec3 rotatedRight = cameraOrientation * rightHandPosition;

    rightHandPosition = cameraPosition + rotatedRight;
    const glm::vec3 rightHandForward = glm::normalize(rightHandOrientation * cameraOrientation * glm::vec3{0, 0, -1});

    glm::vec3 rayOrigin = rightHandPosition;
    glm::vec3 rayDir = rightHandForward;

    glm::vec3 normal = glm::normalize(mPlaneEntity_.getOrientation() * glm::vec3{0, 1, 0});

    float denominator = glm::dot(normal, rayDir);

    ImVec2 calMousePos = {0,0};

    if (glm::abs(denominator) > 1e-6f) {
        glm::vec3 planeToRay = mPlaneEntity_.getPosition() - rayOrigin;
        float t = glm::dot(planeToRay, normal) / denominator;

        if (t > 0.0f) {
            glm::vec3 intersectPoint = rayOrigin + t * rayDir;

            glm::mat4 rotationMatrix = glm::mat4_cast(mPlaneEntity_.getOrientation());
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), mPlaneEntity_.getPosition());


            float planeMinX = -0.5f, planeMaxX = 0.5f; // Plane bounds in local space
            float planeMinY = -0.5f, planeMaxY = 0.5f;

            glm::vec4 localPoint = glm::inverse(translationMatrix * rotationMatrix) * glm::vec4(intersectPoint, 1.0f);

            float u = (localPoint.x - planeMinX) / (planeMaxX - planeMinX);
            float v = (localPoint.z - planeMinY) / (planeMaxY - planeMinY);
            int pixelX = static_cast<int>(u * imguiWidth);
            int pixelY = static_cast<int>(v * imguiHeight);

            calMousePos = ImVec2(pixelX, pixelY);
        }
    }

    const float rightTriggerState = ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::RIGHT);

    // draw imgui
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(imguiWidth, imguiHeight), ImGuiCond_FirstUseEver);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.MousePos = calMousePos;

    if (rightTriggerState > .1f) {
        io.AddMouseButtonEvent(0, true);  // Simulate mouse down
    }
    if (rightTriggerState < .1f) {
        io.AddMouseButtonEvent(0, false);  // Simulate mouse up
    }

    ImGui::Begin("Plane");
    ImGui::SetWindowFontScale(6.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Separator();
    ImGui::Text("Grab: %f, %f", ((clay::AppXR &)mApp_).getInputHandler().getGrabState(clay::InputHandlerXR::Hand::LEFT), ((clay::AppXR &)mApp_).getInputHandler().getGrabState(clay::InputHandlerXR::Hand::RIGHT));
    ImGui::Text("Trigger: %f, %f", ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::LEFT), ((clay::AppXR &)mApp_).getInputHandler().getTriggerState(clay::InputHandlerXR::Hand::RIGHT));

    ImGui::Text("Buttons: Y: %i, X: %i, B: %i, A: %i",
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::Y),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::X),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::B),
        ((clay::AppXR &)mApp_).getInputHandler().getButtonDown(clay::InputHandlerXR::Button::A)
    );
    const auto& leftGripPose = ((clay::AppXR &)mApp_).getInputHandler().getGripPose(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Grip Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        leftGripPose.orientation.x,
        leftGripPose.orientation.y,
        leftGripPose.orientation.z,
        leftGripPose.orientation.w,
        leftGripPose.position.x,
        leftGripPose.position.y,
        leftGripPose.position.z
    );
    const auto& rightGripPose = ((clay::AppXR &)mApp_).getInputHandler().getGripPose(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Grip Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        rightGripPose.orientation.x,
        rightGripPose.orientation.y,
        rightGripPose.orientation.z,
        rightGripPose.orientation.w,
        rightGripPose.position.x,
        rightGripPose.position.y,
        rightGripPose.position.z
    );
    const auto& leftAimPose = ((clay::AppXR &)mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Aim Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        leftAimPose.orientation.x,
        leftAimPose.orientation.y,
        leftAimPose.orientation.z,
        leftAimPose.orientation.w,
        leftAimPose.position.x,
        leftAimPose.position.y,
        leftAimPose.position.z
    );
    const auto& rightAimPose = ((clay::AppXR &)mApp_).getInputHandler().getAimPose(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Aim Pose: (%f, %f, %f, %f) (%f, %f, %f)",
        rightAimPose.orientation.x,
        rightAimPose.orientation.y,
        rightAimPose.orientation.z,
        rightAimPose.orientation.w,
        rightAimPose.position.x,
        rightAimPose.position.y,
        rightAimPose.position.z
    );
    const auto& leftJoystickDir = ((clay::AppXR &)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::LEFT);
    ImGui::Text("Left Joystick dir: (%f, %f) (%f, %f)",
        leftJoystickDir.x,
        leftJoystickDir.y,
        leftJoystickDir.x,
        leftJoystickDir.y
    );
    const auto& rightJoystickDir = ((clay::AppXR &)mApp_).getInputHandler().getJoystickDirection(clay::InputHandlerXR::Hand::RIGHT);
    ImGui::Text("Right Joystick dir: (%f, %f) (%f, %f)",
        rightJoystickDir.x,
        rightJoystickDir.y,
        rightJoystickDir.x,
        rightJoystickDir.y
    );
    const auto& headPose = ((clay::AppXR &)mApp_).getInputHandler().getHeadPose();
    ImGui::Text("Headpose (%f, %f %f, %f) (%f %f %f)",
        headPose.orientation.x,
        headPose.orientation.y,
        headPose.orientation.z,
        headPose.orientation.w,
        headPose.position.x,
        headPose.position.y,
        headPose.position.z
    );

    ImGui::Separator();

    ImGui::BeginGroup();
    if (ImGui::BeginListBox("##Scenes")) {
        for (unsigned int i = 0; i < ((DemoAppXR&)mApp_).mSceneDetails_.size(); ++i) {
            std::string elementName = "Entity " + ((DemoAppXR &)mApp_).mSceneDetails_[i].mName_;
            if (ImGui::Selectable(elementName.c_str(), i == mSelectedSceneIdx)) {
                mSelectedSceneIdx = i;
            }
        }
        ImGui::EndListBox();
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    if (mSelectedSceneIdx < ((DemoAppXR&)mApp_).mSceneDetails_.size()) {
        DemoAppXR::SceneDetail& displayScene = ((DemoAppXR &)mApp_).mSceneDetails_[mSelectedSceneIdx];
        ImGui::Text("%s", displayScene.mName_.c_str());
        ImGui::Image(
            (ImTextureID)displayScene.mPreviewTexture_,
            ImVec2(935, 500 ),
            ImVec2(0, 0),
            ImVec2(1, 1)
        );
        if (ImGui::Button("Start")) {
            displayScene.mLoadScene_();
        }
    }
    ImGui::EndGroup();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void FarmScene::assembleResources() {
    mpResources_ = new clay::Resources(mApp_.getGraphicsContext());

    // create leaves model
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(mApp_.getGraphicsContext());
        pRenderable->addElement({
            mApp_.getResources().getResource<clay::Mesh>("Sphere"),
            mApp_.getResources().getResource<clay::Material>("Flat"),
        });
        mpResources_->addResource<clay::Model>(std::move(pRenderable), "TreeTop");
    }
    // create trunk model
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(mApp_.getGraphicsContext());
        pRenderable->addElement({
            mApp_.getResources().getResource<clay::Mesh>("Cube"),
            mApp_.getResources().getResource<clay::Material>("Flat"),
        });

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "TreeTrunk");
    }
    // floor model
    {
        std::unique_ptr<clay::Model> pRenderable = std::make_unique<clay::Model>(mApp_.getGraphicsContext());
        pRenderable->addElement({
            mApp_.getResources().getResource<clay::Mesh>("Plane"),
            mApp_.getResources().getResource<clay::Material>("Flat"),
        });

        mpResources_->addResource<clay::Model>(std::move(pRenderable), "GrassFloor");
    }
}

void FarmScene::destroyResources() {

}