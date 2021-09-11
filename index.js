const {resolve} = require("path");
const {arch, platform} = require("os");
const GLFW = require(
    resolve(
        __dirname,
        "dist",
        process.env.NODE_ENV === "development" ? "Debug" : "Release",
        platform(),
        arch(),
        "node_glfw.node"
    )
);
module.exports = GLFW;
