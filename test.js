const GLFW = require("./cmake-build-release/node_glfw");
const Worker = require("worker_threads");

(async () => {
    /*console.log("Opening window.");
    const window = new GLFW.Window(600, 500, "Title");
    await new Promise(resolve => setTimeout(resolve, 2000));
    if (Worker.isMainThread) {
        const worker = new Worker.Worker(__filename);
        await new Promise(resolve => worker.on("exit", resolve));
    }
    await new Promise(resolve => setTimeout(resolve, 2000));
    console.log("Closing window");
    window.destroyWindow();*/

    const window = new GLFW.Window(600, 500, "Title");

    window.setKeyCallback(key => {
        console.log("Key",key,GLFW.KEY_ESCAPE);
        if (key === GLFW.KEY_ESCAPE) window.shouldClose = true;
    })

    const intervalHandle = setInterval(() => {
        if (window.shouldClose) {
            window.destroyWindow();
            clearInterval(intervalHandle);
            return;
        }

        GLFW.pollEvents();
    }, 1000 / 60);
})().catch(console.error);
