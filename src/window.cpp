#include "window.hpp"

#include <GLFW/glfw3.h>
#include <napi.h>

using Napi::CallbackInfo;
using Napi::Number;

struct Window : public Napi::ObjectWrap<Window> {
    explicit Window(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Window>(info)
    {
        if (info.Length() < 3) {
            throw Napi::TypeError::New(info.Env(), "Window constructor expects exactly 3 arguments.");
        }

        int width{info[0].ToNumber()};
        int height{info[1].ToNumber()};
        std::string title{info[2].ToString().Utf8Value()};

        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(handle, this);
    }

    Window(Window&&) = delete;

    Window(const Window&) = delete;

    ~Window() override
    {
        destroy();
    }

    static void Init(Napi::Env env, Napi::Object& exports)
    {
        exports.Set("Window", DefineClass(env, "Window", {
            InstanceAccessor<&Window::getShouldClose, &Window::setShouldClose>("shouldClose"),
            InstanceMethod<&Window::destroyWindow>("destroyWindow"),
            InstanceMethod<&Window::getFramebufferSize>("getFramebufferSize"),
            InstanceMethod<&Window::makeContextCurrent>("makeContextCurrent"),
            InstanceMethod<&Window::setKeyCallback>("setKeyCallback"),
            InstanceMethod<&Window::swapBuffers>("swapBuffers"),
        }));
    }

    Napi::Value getFramebufferSize(const Napi::CallbackInfo& info)
    {
        int width, height;
        glfwGetFramebufferSize(handle, &width, &height);

        auto arr{Napi::Array::New(info.Env(), 2)};
        arr.Set(0u, width);
        arr.Set(1u, height);

        return arr;
    }

    void makeContextCurrent(const Napi::CallbackInfo&)
    {
        glfwMakeContextCurrent(handle);
    }

    Napi::Value getShouldClose(const Napi::CallbackInfo& info)
    {
        bool shouldClose{glfwWindowShouldClose(handle) == GLFW_TRUE};
        return Napi::Boolean::New(info.Env(), shouldClose);
    }

    void setShouldClose(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        bool shouldClose{value.ToBoolean()};
        glfwSetWindowShouldClose(handle, shouldClose ? GLFW_TRUE : GLFW_FALSE);
    }

    void swapBuffers(const Napi::CallbackInfo&)
    {
        glfwSwapBuffers(handle);
    }

    Napi::ThreadSafeFunction keyCallbackJs{nullptr};

    void setKeyCallback(const Napi::CallbackInfo& info)
    {
        if (info[0].IsNull() || info[0].IsUndefined()) {
            glfwSetKeyCallback(handle, nullptr);
            keyCallbackJs.Unref(info.Env());
            keyCallbackJs = nullptr;
        }
        else {
            keyCallbackJs = Napi::ThreadSafeFunction::New(
                info.Env(),
                info[0].As<Napi::Function>(),
                "Window key callback",
                0,
                1
            );

            // Set key callback every time in case it has previously been removed.
            glfwSetKeyCallback(handle, &Window::keyCallback);
        }
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window& w{*static_cast<Window*>(glfwGetWindowUserPointer(window))};

        if (w.keyCallbackJs != nullptr) {
            w.keyCallbackJs.NonBlockingCall([key, scancode, action, mods](Napi::Env env, Napi::Function cb) {
                cb.Call({
                    Number::New(env, double(key)),
                    Number::New(env, double(scancode)),
                    Number::New(env, double(action)),
                    Number::New(env, double(mods)),
                });
            });
        }
    }

    void destroy()
    {
        if (handle != nullptr) {
            glfwDestroyWindow(handle);
            handle = nullptr;
        }
    }

    void destroyWindow(const Napi::CallbackInfo& info)
    {
        destroy();

        keyCallbackJs.Unref(info.Env());
        keyCallbackJs = nullptr;
    }

    GLFWwindow* handle{nullptr};
};

void InitWindow(Napi::Env env, Napi::Object exports)
{
    Window::Init(env, exports);
}
