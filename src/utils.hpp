#pragma once

#include <array>
#include <napi.h>
#include <string>
#include <tuple>
#include <utility>

namespace detail {
    template<typename... Args, size_t... Indices>
    void verifyArguments(const Napi::CallbackInfo& info, std::tuple<Args...> args, std::index_sequence<Indices...>)
    {
        static_assert(sizeof...(Args) == sizeof...(Indices), "Incorrect number of argument types.");
        (verifyArgument<Indices>(info, std::get<Indices, Args...>(args)), ...);
    }

    template<unsigned... Digits>
    struct toChars {
        static constexpr char value[]{('0' + Digits)..., 0};
    };

    template<unsigned Rem, unsigned... Digits>
    struct explode : explode<Rem / 10, Rem % 10, Digits...> {
    };

    template<unsigned... Digits>
    struct explode<0, Digits...> : toChars<Digits...> {
    };

    template<size_t Num>
    const std::string numberAsString{explode<Num>::value};

    template<>
    const std::string numberAsString<0>{"0"};

    const std::string& typeName(napi_valuetype type)
    {
        static std::array names{
            std::string("undefined"),
            std::string("null"),
            std::string("boolean"),
            std::string("number"),
            std::string("string"),
            std::string("symbol"),
            std::string("object"),
            std::string("function"),
            std::string("external"),
            std::string("bigint"),
        };
        return names[type];
    }

    const std::string& typedArrayTypeName(napi_typedarray_type type)
    {
        static std::array names{
            std::string("Int8Array"),
            std::string("Uint8Array"),
            std::string("Uint8ClampedArray"),
            std::string("Int16Array"),
            std::string("Uint16Array"),
            std::string("Int32Array"),
            std::string("Uint32Array"),
            std::string("Float32Array"),
            std::string("Float64Array"),
            std::string("BigInt64Array"),
            std::string("BigUint64Array"),
        };
        return names[type];
    }

    template<size_t Idx>
    void verifyArgument(const Napi::CallbackInfo& info, napi_valuetype type)
    {
        auto t{info[Idx].Type()};
        if (t != type)
            throw Napi::TypeError::New(
                info.Env(),
                "Wrong argument type at index " + numberAsString<Idx> + ". Expected " + typeName(type) + ", got "
                    + typeName(t) + "."
            );
    }

    template<size_t Idx>
    void verifyArgument(const Napi::CallbackInfo& info, napi_typedarray_type type)
    {
        auto arg{info[Idx]};
        if (!arg.IsTypedArray()) {
            throw Napi::TypeError::New(
                info.Env(),
                "Wrong argument type at index" +
                    numberAsString<Idx> +
                    ". Expected " +
                    typedArrayTypeName(type) +
                    ", got " +
                    typeName(arg.Type()) +
                    "."
            );
        }
        auto typedArrayArgType{arg.As<Napi::TypedArray>().TypedArrayType()};
        if (typedArrayArgType != type) {
            throw Napi::TypeError::New(
                info.Env(),
                "Wrong argument type at index" +
                    numberAsString<Idx> +
                    ". Expected " +
                    typedArrayTypeName(type) +
                    ", got " +
                    typedArrayTypeName(typedArrayArgType) +
                    "."
            );
        }
    }
}

template<typename... Args>
void verifyArguments(const Napi::CallbackInfo& info, Args... args)
{
    detail::verifyArguments(info, std::forward_as_tuple(args...), std::make_index_sequence<sizeof...(Args)>());
}
