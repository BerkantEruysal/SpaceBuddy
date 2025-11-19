#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <thread>
#include <chrono>
#include "backend.h"

#ifdef Q_OS_WIN
    #include <windows.h>
    void simulateSpaceKey() {
        INPUT inputs[2] = {};
        
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_SPACE;
        
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_SPACE;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        
        SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    }
#elif defined(Q_OS_LINUX)
    #include <X11/Xlib.h>
    #include <X11/keysym.h>
    #include <X11/extensions/XTest.h>
    
    void simulateSpaceKey() {
        Display *display = XOpenDisplay(nullptr);
        if (!display) return;
        
        unsigned int keycode = XKeysymToKeycode(display, XK_space);
        XTestFakeKeyEvent(display, keycode, True, 0);
        XTestFakeKeyEvent(display, keycode, False, 0);
        XFlush(display);
        XCloseDisplay(display);
    }
#endif

void onSpaceKeyPressed() {
    // Use QMetaObject::invokeMethod to ensure the call happens on the main thread (where Backend lives)
    // This is critical because Backend interacts with QWebSocket which is not thread-safe.
    QMetaObject::invokeMethod(Backend::create(), "onSpacePressed", Qt::QueuedConnection);
}

#ifdef Q_OS_WIN
void windowsGlobalSpaceListener() {
    bool spacePressed = false;
    while (true) {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            if (!spacePressed) {
                onSpaceKeyPressed();
                spacePressed = true;
            }
        } else {
            spacePressed = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
#elif defined(Q_OS_LINUX)

void linuxGlobalSpaceListener() {
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        qWarning() << "X11 acilimiyor";
        return;
    }

    int spaceKeyCode = XKeysymToKeycode(display, XK_space);
    bool spacePressed = false;

    while (true) {
        char keys[32];
        XQueryKeymap(display, keys);

        bool isPressed = (keys[spaceKeyCode / 8] & (1 << (spaceKeyCode % 8))) != 0;

        if (isPressed && !spacePressed) {
            onSpaceKeyPressed();
            spacePressed = true;
        } else if (!isPressed) {
            spacePressed = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    XCloseDisplay(display);
}
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("SpaceBuddyApp", "Main");

    // Global space key listener thread
    #ifdef Q_OS_WIN
        std::thread spaceListenerThread(windowsGlobalSpaceListener);
        spaceListenerThread.detach();
    #elif defined(Q_OS_LINUX)
        std::thread spaceListenerThread(linuxGlobalSpaceListener);
        spaceListenerThread.detach();
    #endif

    Backend* backend = Backend::create();
    QObject::connect(backend, &Backend::spaceReceivedRemote, &app, [](){
        simulateSpaceKey();
    });

    return app.exec();
}
