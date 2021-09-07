#include <napi.h>
#include <GLFW/glfw3.h>

#include "utils.hpp"
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
            InstanceMethod<&GLFWAddon::windowHint>("windowHint"),
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

    void windowHint(const Napi::CallbackInfo& info)
    {
        verifyArguments(info, napi_number, napi_number);
        glfwWindowHint(info[0].ToNumber(), info[1].ToNumber());
    }
};

NODE_API_ADDON(GLFWAddon);