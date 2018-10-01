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
#include <tuple>

USING_NS_CC;

namespace
{
    constexpr float scale = 1.5f;
    constexpr float ball_r = 3;
    constexpr float ball_speed = 75.f;
    constexpr int paddle_w = 10;
    constexpr int paddle_h = 50;
    constexpr float paddle_speed = 150;

    /*
    struct physics_tag
    {
        static constexpr int ball   = 1;
        static constexpr int paddle = 2;
    };
    */

    template <typename T>
    T ptm(float _x, float _y) noexcept
    {
        //auto size = Director::getInstance()->getVisibleSize();
        return {_x / scale, _y / scale};
    }

    template <typename T>
    T mtp(float _x, float _y) noexcept
    {
        //auto size = Director::getInstance()->getVisibleSize();
        return {_x * scale, _y * scale};
    }

    // Creates a game ball sprite.
    auto create_ball(b2World& _world, Color4F _color) -> std::tuple<DrawNode*, b2Body*>
    {
        const Vec2 center{};
        const float radius = ball_r;
        const float angle = 0;
        const unsigned int segments = 10;
        auto vsize = Director::getInstance()->getVisibleSize();

        auto* ball = DrawNode::create();
        ball->drawSolidCircle(center, radius, angle, segments, _color);
        ball->setPosition({static_cast<float>(vsize.width) / 2, static_cast<float>(vsize.height) / 2});

        // Setup physics.
        // Create box2d body.
        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        body_def.bullet = true;
        auto* body = _world.CreateBody(&body_def);
        body->SetGravityScale(0);

        // Create shape.
        auto pos = ptm<Vec2>(ball->getPositionX(), ball->getPositionY());
        b2CircleShape shape;
        shape.m_p.Set(0.f, 0.f);
        shape.m_radius = ptm<b2Vec2>(ball_r, 0.f).x;

        // Create box2d fixture.
        b2FixtureDef fixture_def;
        fixture_def.density = 1.f;
        fixture_def.friction = 0.f;
        fixture_def.restitution = 1.f;
        fixture_def.shape = &shape;
        body->CreateFixture(&fixture_def);

        return {ball, body};
    }

    // Creates a game paddle sprite.
    auto create_paddle(b2World& _world, Color4F _color) -> std::tuple<DrawNode*, b2Body*>
    {
        Vec2 origin{0, 0};
        Vec2 dst{paddle_w, paddle_h};
        const auto vsize_half = Director::getInstance()->getVisibleSize().height / 2;

        auto* paddle = DrawNode::create();
        paddle->drawSolidRect(origin, dst, _color);
        paddle->setPositionY(vsize_half - (paddle_h / 2));

        // Setup physics.
        // Create box2d body.
        b2BodyDef body_def;
        body_def.type = b2_kinematicBody;
        auto pos = ptm<b2Vec2>(0, paddle->getPositionY());
        body_def.position.Set(pos.x, pos.y);
        auto* body = _world.CreateBody(&body_def);
        body->SetGravityScale(0);

        // Create shape.
        const auto phw = static_cast<float>(paddle_w) / 2;
        const auto phh = static_cast<float>(paddle_h) / 2;
        auto size = ptm<b2Vec2>(phw, phh);
        b2PolygonShape shape;
        shape.SetAsBox(size.x, size.y, {phw, -phh}, 0.f); // TODO Should probably be scaled.

        // Create box2d fixture.
        b2FixtureDef fixture_def;
        fixture_def.density = 1.f;
        fixture_def.friction = 0.f;
        fixture_def.restitution = 0.f;
        fixture_def.shape = &shape;
        body->CreateFixture(&fixture_def);

        return {paddle, body};
    }

    void init_ball_direction(Vec2& _dir)
    {
        static std::mt19937 gen{std::random_device{}()};
        static std::uniform_int_distribution<int> dist{0, 1};
        _dir.x = dist(gen) > 0 ? 1 : -1;
        _dir.y = dist(gen) > 0 ? 1 : -1;
    }

    void update_node_position(b2Body& _body, DrawNode& _node)
    {
        auto v = _body.GetLinearVelocity();
        auto pos = _body.GetPosition();
        _node.setPosition(mtp<Vec2>(pos.x, pos.y));
        log("linear velocity = {%f, %f}", v.x, v.y);
        log("ball body pos   = {%f, %f}", pos.x, pos.y);
        log("ball node pos   = {%f, %f}", _node.getPositionX(), _node.getPositionY());
    }

    // Updates the position of a paddle based on the keys pressed.
    void move_paddle(b2Body& _body, DrawNode& _paddle, bool _key_up, bool _key_down)
    {
        _body.SetLinearVelocity({});

        // clang-format off
        if      (_key_up)   _body.SetLinearVelocity(ptm<b2Vec2>(0.f,  paddle_speed));
        else if (_key_down) _body.SetLinearVelocity(ptm<b2Vec2>(0.f, -paddle_speed));
        // clang-format on

        auto p = _body.GetPosition();
        _paddle.setPosition({_paddle.getPositionX(), mtp<Vec2>(0, p.y).y});
    }
} // anonymous namespace

Scene* pong::createScene()
{
    return pong::create();
}

// on "init" you need to initialize your instance
auto pong::init() -> bool
{
    if (!Scene::init())
        return false;

    init_b2_world();

    const auto vsize = Director::getInstance()->getVisibleSize();

    // Game ball.
    std::tie(ball_, ball_body_) = create_ball(*world_, Color4F::WHITE);
    init_ball_direction(ball_dir_);
    ball_body_->SetTransform(ptm<b2Vec2>(ball_->getPositionX(), ball_->getPositionY()), 0.f);
    ball_body_->SetLinearVelocity(ptm<b2Vec2>(ball_speed, 0.f));
    //ball_body_->SetLinearVelocity(ptm<b2Vec2>(ball_speed, ball_speed));
    addChild(ball_);

    // Left paddle.
    std::tie(l_paddle_, l_paddle_body_) = create_paddle(*world_, Color4F::BLUE);
    l_paddle_->setPositionX(10);
    l_paddle_body_->SetTransform(ptm<b2Vec2>(l_paddle_->getPositionX(), l_paddle_->getPositionY()), 0.f);
    addChild(l_paddle_);

    // Right paddle.
    std::tie(r_paddle_, r_paddle_body_) = create_paddle(*world_, Color4F::GREEN);
    r_paddle_->setPositionX(vsize.width - paddle_w - 10);
    r_paddle_body_->SetTransform(ptm<b2Vec2>(r_paddle_->getPositionX(), r_paddle_->getPositionY()), 0.f);
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

    scheduleUpdate();

    return true;
}

auto pong::update(float _delta_time) -> void
{
    Node::update(_delta_time);

    // Controls for left paddle.
    move_paddle(*l_paddle_body_,
                *l_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_W),
                is_key_pressed(EventKeyboard::KeyCode::KEY_S));

    // Controls for right paddle.
    move_paddle(*r_paddle_body_,
                *r_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_UP_ARROW),
                is_key_pressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW));

    constexpr auto time_step = 1.f / 60.f;
    constexpr auto velocity_interations = 6;
    constexpr auto position_interations = 2;

    world_->Step(time_step, velocity_interations, position_interations);

    update_node_position(*ball_body_, *ball_);
}

auto pong::init_b2_world() -> void
{
    world_ = std::make_unique<b2World>(b2Vec2{0.f, -9.8f});
    //world_->SetContinuousPhysics(true);
    //world_->SetAllowSleeping(true);
}

auto pong::is_key_pressed(EventKeyboard::KeyCode _key_code) -> bool
{
    return key_state_[static_cast<int>(_key_code)];
}

