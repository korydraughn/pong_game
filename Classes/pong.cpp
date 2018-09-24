/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "pong.hpp"
#include "SimpleAudioEngine.h"

#include <random>
#include <vector>

USING_NS_CC;

namespace
{
    constexpr float ball_r = 5;
    constexpr float ball_speed = 250;
    constexpr int paddle_w = 10;
    constexpr int paddle_h = 50;
    constexpr float paddle_speed = 150;

    struct physics_tag
    {
        static constexpr int ball   = 1;
        static constexpr int paddle = 2;
    };

    // Creates a game ball sprite.
    auto create_ball(Color4F _color) -> DrawNode*
    {
        const float density = 0;
        const float restitution = 1;
        const float friction = 0;

        auto* physics_body = PhysicsBody::createCircle(ball_r, {density, restitution, friction});
        physics_body->setTag(physics_tag::ball);
        physics_body->setDynamic(true);
        physics_body->setGravityEnable(false);
        physics_body->setRotationEnable(false);
        physics_body->setContactTestBitmask(1);

        const Vec2 center{};
        const float radius = ball_r;
        const float angle = 0;
        const unsigned int segments = 10;

        auto* ball = DrawNode::create();
        ball->drawSolidCircle(center, radius, angle, segments, _color);
        ball->setPhysicsBody(physics_body);

        return ball;
    }

    // Creates a game paddle sprite.
    auto create_paddle(Color4F _color) -> DrawNode*
    {
        const float density = 1;
        const float restitution = 0;
        const float friction = 0;

        const auto phw = static_cast<float>(paddle_w) / 2;
        const auto phh = static_cast<float>(paddle_h) / 2;

        std::vector<Vec2> points{
            {0, phh},
            {phw, paddle_h},
            {paddle_w, phh},
            {phw, 0}
        };

        //auto* physics_body = PhysicsBody::createBox({paddle_w, paddle_h}, {density, restitution, friction});
        auto* physics_body = PhysicsBody::createPolygon(points.data(), points.size(), {density, restitution, friction});
        physics_body->setTag(physics_tag::paddle);
        physics_body->setDynamic(false);
        //physics_body->setPositionOffset({paddle_w / 2, paddle_h / 2});
        //physics_body->setGravityEnable(false);
        //physics_body->setRotationEnable(false);
        physics_body->setContactTestBitmask(1);

        Vec2 origin{0, 0};
        Vec2 dst{paddle_w, paddle_h};
        const auto vsize_half = Director::getInstance()->getVisibleSize().height / 2;

        auto* paddle = DrawNode::create();
        paddle->drawSolidRect(origin, dst, _color);
        paddle->setAnchorPoint({});
        paddle->setPositionY(vsize_half - (paddle_h / 2));
        paddle->setPhysicsBody(physics_body);

        return paddle;
    }

    void init_ball_direction(Vec2& _dir)
    {
        static std::mt19937 gen{std::random_device{}()};
        static std::uniform_int_distribution<int> dist{0, 1};
        _dir.x = dist(gen) > 0 ? 1 : -1;
        _dir.y = dist(gen) > 0 ? 1 : -1;
    }

    void move_ball(float _delta_time, float _ball_speed, Vec2& _ball_dir, DrawNode* _ball)
    {
        /*
        const auto speed = _ball_speed * _delta_time;
        const auto pos = _ball->getPosition();
        auto x = pos.x + (_ball_dir.x * speed);
        auto y = pos.y + (_ball_dir.y * speed);

        _ball->setPosition({x, y});
        */

        const auto vsize = Director::getInstance()->getVisibleSize();
        const auto min_x = ball_r;
        const auto min_y = ball_r;
        const auto max_x = vsize.width - ball_r;
        const auto max_y = vsize.height - ball_r;
        auto x = _ball->getPositionX();
        auto y = _ball->getPositionY();
        auto* physics_body = _ball->getPhysicsBody();
        auto vel = physics_body->getVelocity();

        // clang-format off
        if (x < min_x || x > max_x) { _ball_dir.x *= -1; physics_body->setVelocity({vel.x * -1, vel.y}); }
        if (y < min_y || y > max_y) { _ball_dir.y *= -1; physics_body->setVelocity({vel.x, vel.y * -1}); }
        // clang-format on
    }

    // Updates the position of a paddle based on the keys pressed.
    void move_paddle(float _delta_time, DrawNode* _paddle, bool _key_up, bool _key_down)
    {
        const auto speed = paddle_speed * _delta_time;
        auto y = _paddle->getPositionY();

        // clang-format off
        if      (_key_up)   _paddle->setPositionY(y + speed);
        else if (_key_down) _paddle->setPositionY(y - speed);
        // clang-format on
        
        // Keep paddle in the visible area.
        const auto vheight = Director::getInstance()->getVisibleSize().height;
        const auto max_y = vheight - paddle_h;
        y = _paddle->getPositionY();

        // clang-format off
        if      (y < 0)     _paddle->setPositionY(0);
        else if (y > max_y) _paddle->setPositionY(max_y);
        // clang-format on
    }
} // anonymous namespace

Scene* pong::createScene()
{
    return pong::create();
}

// on "init" you need to initialize your instance
auto pong::init() -> bool
{
    if (!Scene::initWithPhysics())
        return false;

    Scene::getPhysicsWorld()->setDebugDrawMask(0xffffffff);

    const auto vsize = Director::getInstance()->getVisibleSize();

    // Game ball.
    ball_ = create_ball(Color4F::WHITE);
    ball_->setPosition({vsize.width / 2, vsize.height / 2});
    init_ball_direction(ball_dir_);
    ball_->getPhysicsBody()->setVelocity({ball_speed * ball_dir_.x, ball_speed * ball_dir_.y});
    addChild(ball_);

    // Left paddle.
    l_paddle_ = create_paddle(Color4F::BLUE);
    l_paddle_->setPositionX(10);
    addChild(l_paddle_);

    // Right paddle.
    r_paddle_ = create_paddle(Color4F::GREEN);
    r_paddle_->setPositionX(vsize.width - paddle_w - 10);
    addChild(r_paddle_);

    // Add keyboard support.
    auto* keyboard_listener = EventListenerKeyboard::create();

    // clang-format off
    keyboard_listener->onKeyPressed = [](auto _key_code, auto* _event)
    {
        log("Key with keycode %d pressed", _key_code);
        key_state_[static_cast<int>(_key_code)] = true;
    };

    keyboard_listener->onKeyReleased = [](auto _key_code, auto* _event)
    {
        log("Key with keycode %d released", _key_code);
        key_state_[static_cast<int>(_key_code)] = false;
    };
    // clang-format on

    auto* event_dispatcher = Director::getInstance()->getEventDispatcher();

    event_dispatcher->addEventListenerWithSceneGraphPriority(keyboard_listener, this);

    auto* contact_listener = EventListenerPhysicsContact::create();

    contact_listener->onContactBegin = [](PhysicsContact& _contact) -> bool
    {
        auto* a = _contact.getShapeA()->getBody();
        auto* b = _contact.getShapeA()->getBody();

        for (auto* shape : {a, b})
        {
            if (shape->getTag() == physics_tag::ball)
            {
                auto vel = shape->getVelocity();
                shape->setVelocity({vel.x * -1, vel.y});
                break;
            }
        }

        return true;
    };

    event_dispatcher->addEventListenerWithSceneGraphPriority(contact_listener, this);

    scheduleUpdate();

    return true;
}

auto pong::update(float _delta_time) -> void
{
    Node::update(_delta_time);

    move_ball(_delta_time, ball_speed, ball_dir_, ball_);

    // Controls for left paddle.
    move_paddle(_delta_time,
                l_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_W),
                is_key_pressed(EventKeyboard::KeyCode::KEY_S));

    // Controls for right paddle.
    move_paddle(_delta_time,
                r_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_UP_ARROW),
                is_key_pressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW));
}

auto pong::is_key_pressed(EventKeyboard::KeyCode _key_code) -> bool
{
    return key_state_[static_cast<int>(_key_code)];
}
