#include "robot.hpp"

#include "led.hpp"

#include <dlfcn.h>
#include <iostream>

#define DEFAULT_LIB "./libcode.so"

using namespace piksel;
using namespace std;

Robot::~Robot() {
    unloadCode();

    mWorld->DestroyBody(mBody);
}

void Robot::init() {
    unique_ptr<Output> ledPtr(new Led(*this));
    mOutputs.push_back(move(ledPtr));

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(mPosition.x, mPosition.y);
    mBody = mWorld->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(mWidth / 2.0f, mHeight / 2.0f, b2Vec2_zero, mRotation);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    mBody->CreateFixture(&fixtureDef);

    loadCode();

    if (mSetup) {
        mSetup();
    }
}

void Robot::loadCode() {
    char* error;

    mLibHandle = dlopen(DEFAULT_LIB, RTLD_NOW);
    if (!mLibHandle) {
        cerr << "[Robot] Error loading the Arduino code from library: " << dlerror() << endl;
    } else {
        dlerror(); // Clear any existing error
        mSetup = reinterpret_cast<setup_t>(dlsym(mLibHandle, "setup"));

        if ((error = dlerror())) {
            cerr << "[Robot] Could not find setup function: " << error << endl;
        }

        dlerror(); // Clear any existing error
        mLoop = reinterpret_cast<loop_t>(dlsym(mLibHandle, "loop"));

        if ((error = dlerror())) {
            cerr << "[Robot] Could not find loop function: " << error << endl;
        }
    }
}

void Robot::unloadCode() {
    if (mLibHandle) {
        if (dlclose(mLibHandle)) {
            cerr << "[Robot] Could not close the library: " << dlerror() << endl;
        } else {
            mLibHandle = nullptr;
        }
    }
}

void Robot::reloadCode() {
    unloadCode();
    loadCode();

    if (mSetup) {
        mSetup();
    }
}

void Robot::update() {
    if (mLoop) {
        mLoop();
    }

    for (auto&& output : mOutputs) {
        output->update();
    }
}

void Robot::draw(Graphics& g) {
    g.push();

    g.fill(mColor);
    g.rectMode(DrawMode::CENTER);
    g.rotate(getRotation());
    g.translate(getPosition().x, getPosition().y);
    g.noStroke();
    g.rect(0.0f, 0.0f, mWidth, mHeight);

    for (auto&& output : mOutputs) {
        output->draw(g);
    }

    g.pop();
}

glm::vec2 Robot::getPosition() {
    b2Vec2 position = mBody->GetPosition();
    return glm::vec2(position.x, position.y);
}

glm::vec2 Robot::getHeading() {
    float angleRads = getRotation();
    return glm::normalize(glm::vec2(glm::cos(angleRads), glm::sin(angleRads)));
}

void Robot::setHeading(glm::vec2 heading) {
    float cosTheta = glm::dot(glm::vec2(1.0f, 0.0), heading) / glm::length(heading);
    setRotation(glm::acos(cosTheta));
}

glm::vec2 Robot::getLeftPerpendicularVec() {
    glm::vec2 headingVec = getHeading();
    glm::vec2 perpVec = glm::vec2(headingVec.y, -headingVec.x);

    return perpVec;
}

glm::vec2 Robot::getRightPerpendicularVec() {
    glm::vec2 headingVec = getHeading();
    glm::vec2 perpVec = glm::vec2(-headingVec.y, headingVec.x);

    return perpVec;
}
