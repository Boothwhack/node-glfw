const FS = require("fs/promises");

if (process.argv.length < 5) throw new Error("Expected 3 arguments.");

const [glfwHeader, srcFile, generatedFile] = process.argv.slice(2);

if (!srcFile.endsWith(".in")) throw new Error("Source file must end with '.in'.");

const DefineConstant = /^#define\s+GLFW_(\w+)/gm;

(async () => {
    const header = (await FS.readFile(glfwHeader)).toString();

    const constantDescriptors = Array.from(header.matchAll(DefineConstant))
        .map(([_, name]) => `ConstantDescriptor(${name})`)
        .join(',\n');

    const src = (await FS.readFile(srcFile)).toString();
    const generatedSrc = src.replace("{{CONSTANTS}}", constantDescriptors);

    await FS.writeFile(generatedFile, generatedSrc);
})().catch(console.error);
