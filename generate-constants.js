const FS = require("fs/promises");

if (process.argv.length < 5) throw new Error("Expected 5 arguments.");

const [glfwHeader, cppSrcFile, generatedCppFile, tsSrcFile, generatedTsFile] = process.argv.slice(2);

if (!cppSrcFile.endsWith(".in")) throw new Error("Source file must end with '.in'.");

const DefineConstant = /^#define\s+GLFW_(\w+)/gm;

(async () => {
    const header = (await FS.readFile(glfwHeader)).toString();

    const constants = Array.from(header.matchAll(DefineConstant));

    // Generate C++ constants
    const constantDescriptors = constants
        .map(([_, name]) => `ConstantDescriptor(${name})`)
        .join(",\n");

    const cppSrc = (await FS.readFile(cppSrcFile)).toString();
    const generatedCppSrc = cppSrc.replace("{{CONSTANTS}}", constantDescriptors);

    await FS.writeFile(generatedCppFile, generatedCppSrc);

    // Generate TypeScript constant definitions
    const constantDefinitions = constants
        .map(([_,name]) => `export const ${name}: number;`)
        .join('\n');

    const tsSrc = (await FS.readFile(tsSrcFile)).toString();
    const generatedTsSrc = tsSrc.replace("{{CONSTANTS}}", constantDefinitions);

    await FS.writeFile(generatedTsFile, generatedTsSrc);
})().catch(console.error);
