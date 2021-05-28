#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <box2d/box2d.h>
#include <memory>
#include <piksel/baseapp.hpp>

class Robot;

class Level {
public:
    Level();
    ~Level();

    void init();
    void update();
    void draw(piksel::Graphics& g);

    void createFrictionJoint(b2Body* body);

    b2World& getWorld() { return *mWorld; }
    Robot& getRobot() { return *mRobot; }

protected:
    void createLevel();

    const float mTimeStep = 1.0f / 60.0f;
    const int32 mVelocityIterations = 6;
    const int32 mPositionIterations = 2;

    std::unique_ptr<b2World> mWorld;
    b2Body* mLevelBody;
    b2FrictionJoint* mFrictionJoint;

    std::unique_ptr<Robot> mRobot;
};

#endif /* LEVEL_HPP */