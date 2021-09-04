#include <napi.h>
#include <GLFW/glfw3.h>

#include "constants.hpp"
#include "window.hpp"

class GLFWAddon : public Napi::Addon<GLFWAddon> {
public:
    GLFWAddon(Napi::Env env, Napi::Object exports)
    {
        glfwInit();

        InitConstants(env, exports);
        InitWindow(env, exports);

        DefineAddon(exports, {
            InstanceMethod<&GLFWAddon::pollEvents>("pollEvents"),
        });
    }

    ~GLFWAddon()
    {
        glfwTerminate();
    }

    void pollEvents(const Napi::CallbackInfo& info)
    {
        glfwPollEvents();
    }
};

NODE_API_ADDON(GLFWAddon);