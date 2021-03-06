export class Window {
    constructor(width: number, height: number, title: string);

    shouldClose: boolean;

    destroyWindow(): void;

    getFramebufferSize(): [number, number];

    makeContextCurrent(): void;

    swapBuffers(): void;

    setKeyCallback(cb: (key: number, scancode: number, action: number, mods: number) => void | null): void;
}

export function pollEvents(): void;

export function windowHint(hint: number, value: number): void;

export const getProcAddress: any;

/*CONSTANTS*/
