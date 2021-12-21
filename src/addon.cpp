#include <napi.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "utils.hpp"
#include "constants.hpp"
#include "window.hpp"

// GLFWAddon may be instantiated in multiple threads, but glfwInit should only be called once and glfwTerminate should
// be called when all instances of GLFWAddon are destroyed.
struct GLFWGlobalState {
    GLFWGlobalState() {
        glfwInit();
    }

    ~GLFWGlobalState() {
        glfwTerminate();
    }

    static std::weak_ptr<GLFWGlobalState>& weakRefGlobalInstance() {
        static std::weak_ptr<GLFWGlobalState> globalInstanceRef{};
        return globalInstanceRef;
    }

    static std::shared_ptr<GLFWGlobalState> refGlobalInstance() {
        static std::mutex mtx{};

        // ensure multiple threads are not trying to create a new instance at the same time.
        std::lock_guard guard{mtx};

        auto& weakRef{weakRefGlobalInstance()};
        if (weakRef.expired()) {
            // create new instance and keep track of it, return it as the first reference.
            auto newInstance{std::make_shared<GLFWGlobalState>()};
            weakRef = newInstance;
            return newInstance;
        } else {
            // at least one thread already has a reference to the instance, return another reference.
            return weakRef.lock();
        }
    }
};

class GLFWAddon : public Napi::Addon<GLFWAddon> {
public:
    typedef void (*glFunc) (void);
    typedef glFunc (glLoadFunc)(const char*);

    glLoadFunc* loadPtr_;
    std::shared_ptr<GLFWGlobalState> glfw_;

    GLFWAddon(Napi::Env env, Napi::Object exports)
            : loadPtr_{&glfwGetProcAddress}, glfw_{GLFWGlobalState::refGlobalInstance()} {
        InitConstants(env, exports);
        InitWindow(env, exports);

        DefineAddon(exports, {
            InstanceMethod<&GLFWAddon::pollEvents>("pollEvents"),
            InstanceMethod<&GLFWAddon::windowHint>("windowHint"),
            InstanceAccessor<&GLFWAddon::getProcAddress>("getProcAddress"),
        });
    }

    void pollEvents(const Napi::CallbackInfo& info) {
        glfwPollEvents();
    }

    void windowHint(const Napi::CallbackInfo& info)
    {
        verifyArguments(info, napi_number, napi_number);
        glfwWindowHint(info[0].ToNumber(), info[1].ToNumber());
    }

    Napi::Value getProcAddress(const Napi::CallbackInfo& info)
    {
        return Napi::External<glLoadFunc*>::New(info.Env(), &loadPtr_);
    }
};

NODE_API_ADDON(GLFWAddon);