// clay
#include <clay/utils/common/Logger.h>
#include <clay/application/xr/XRSystem.h>
// application
#include "application/DemoAppXR.h"
#include "application/scenes/sandbox/SandboxScene.h"
#include "application/scenes/space/SpaceScene.h"
#include "application/scenes/farm/FarmScene.h"


void android_main(struct android_app* androidApp) {
    JNIEnv *env;
    androidApp->activity->vm->AttachCurrentThread(&env, nullptr);

    XrInstance m_xrInstance = XR_NULL_HANDLE;  // Dummy XrInstance variable for OPENXR_CHECK macro.
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR = nullptr;
    OPENXR_CHECK(
        xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&xrInitializeLoaderKHR),
        "Failed to get InstanceProcAddr for xrInitializeLoaderKHR."
    );
    if (!xrInitializeLoaderKHR) {
        return;
    }

    // Fill out an XrLoaderInitInfoAndroidKHR structure and initialize the loader for Android.
    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitializeInfoAndroid.applicationVM = androidApp->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = androidApp->activity->clazz;
    OPENXR_CHECK(
        xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid),
        "Failed to initialize Loader for Android."
    )
    LOG_I("Starting Clay VK VR Demo");

    DebugOutput debugOutput;  // This redirects std::cerr and std::cout to the IDE's output or Android Studio's logcat.

    clay::XRSystem xrSystem(androidApp);
    xrSystem.initialize();

    DemoAppXR demoAppXr(&xrSystem);
    clay::Resources::setFileLoader(
        [&demoAppXr](const std::string& path) {
            return demoAppXr.loadFileToMemory_XR(path);
        }
    );
    demoAppXr.setScene(new SandboxScene(demoAppXr));
    demoAppXr.Run();
}
