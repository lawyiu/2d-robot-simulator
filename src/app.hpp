#ifndef APP_HPP
#define APP_HPP

#include "robot.hpp"
#include <piksel/baseapp.hpp>
#include <box2d/box2d.h>

#include <memory>

#define INIT_SCALE_FACTOR 10.0f

class App : public piksel::BaseApp {
public:
    static App& getInstance();

    int getDeltaTimeMillis() { return mDeltaTimeMillis; }
    float getDeltaTimeSeconds() { return mDeltaTimeMillis / 1000.0f; }

    void setup();
    void update();
    void draw(piksel::Graphics& g);

    Robot& getCurrentRobot();

    b2World& getWorld() { return *mWorld; }

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseWheel(int delta);
    void mouseMoved(int x, int y);
    void mousePressed(int button);
    void mouseReleased(int button);

private:
    App() : piksel::BaseApp(640, 480), mKeyPanState(PAN_NONE) {}
    App(const App& other) {}
    ~App();

    void updateDeltaTime();

    int mPrevMillis = 0;
    int mDeltaTimeMillis = 0;

    enum KeyPanState {
        PAN_NONE = 0,
        PAN_UP = 1,
        PAN_DOWN = 2,
        PAN_LEFT = 4,
        PAN_RIGHT = 8
    };

    uint8_t mKeyPanState : 4;

    enum MousePanState {
        NO_PAN,
        START_PAN,
        PANNING
    };

    MousePanState mMousePanState = NO_PAN;

    bool mPause = false;
    bool mReload = false;
    bool mRestart = false;

    piksel::Font mFont;
    const float mTextPadding = 10.0f;
    const float mTextSize = 18.0f;

    float mScaleFactor = INIT_SCALE_FACTOR; // Pixels per cm
    float mWheelFactor = 5.0f;              // Controls mouse wheel speed

    float pixelsPerSecond = 100.0f;
    float mOffsetX = 0.0f;
    float mOffsetY = 0.0f;
    float mOrigX = 0.0f;
    float mOrigY = 0.0f;

    std::unique_ptr<Robot> mRobot;

    const float mTimeStep = 1.0f / 60.0f;
    const int32 mVelocityIterations = 6;
    const int32 mPositionIterations = 2;
    std::unique_ptr<b2World> mWorld;
};

#endif /* APP_HPP */
