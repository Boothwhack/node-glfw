export class Window {
    constructor(width: number, height: number, title: string);

    shouldClose: boolean;

    swapBuffers(): void;

    setKeyCallback(cb: (key: number, scancode: number, action: number, mods: number) => void | null): void;

    destroyWindow(): void;
}

export function pollEvents(): void;
