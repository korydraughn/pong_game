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

USING_NS_CC;

namespace
{
    constexpr int paddle_w = 10;
    constexpr int paddle_h = 50;
    constexpr float paddle_speed = 150;

    // Creates a game paddle sprite.
    DrawNode* create_paddle(Color4F _color)
    {
        Vec2 origin{0, 0};
        Vec2 dst{paddle_w, paddle_h};

        auto* paddle = DrawNode::create();
        paddle->drawSolidRect(origin, dst, _color);
        paddle->setAnchorPoint({});

        const auto vsize_half = Director::getInstance()->getVisibleSize().height / 2;
        paddle->setPositionY(vsize_half - (paddle_h / 2));

        return paddle;
    }

    // Updates the position of a paddle based on the keys pressed.
    void move_paddle(DrawNode* _paddle,
                     bool _key_left,
                     bool _key_right,
                     bool _key_up,
                     bool _key_down,
                     float _delta_time)
    {
        const auto speed = paddle_speed * _delta_time;
        const auto pos = _paddle->getPosition();

        if (_key_left)
        {
            _paddle->setPositionX(pos.x - speed);
        }
        else if (_key_right)
        {
            _paddle->setPositionX(pos.x + speed);
        }

        if (_key_up)
        {
            _paddle->setPositionY(pos.y + speed);
        }
        else if (_key_down)
        {
            _paddle->setPositionY(pos.y - speed);
        }
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

    // Left paddle.
    l_paddle_ = create_paddle(Color4F::RED);
    l_paddle_->setPositionX(10);
    addChild(l_paddle_);

    // Right paddle.
    const auto visible_size = Director::getInstance()->getVisibleSize();
    r_paddle_ = create_paddle(Color4F::GREEN);
    r_paddle_->setPositionX(visible_size.width - paddle_w - 10);
    addChild(r_paddle_);

    // Add keyboard support.
    auto* listener = EventListenerKeyboard::create();

    // clang-format off
    listener->onKeyPressed = [](auto _key_code, auto* _event)
    {
        log("Key with keycode %d pressed", _key_code);
        key_state_[static_cast<int>(_key_code)] = true;
    };

    listener->onKeyReleased = [](auto _key_code, auto* _event)
    {
        log("Key with keycode %d released", _key_code);
        key_state_[static_cast<int>(_key_code)] = false;
    };
    // clang-format on

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    scheduleUpdate();

    return true;
}

auto pong::update(float _delta_time) -> void
{
    Node::update(_delta_time);

    // Controls for left paddle.
    move_paddle(l_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_A),
                is_key_pressed(EventKeyboard::KeyCode::KEY_D),
                is_key_pressed(EventKeyboard::KeyCode::KEY_W),
                is_key_pressed(EventKeyboard::KeyCode::KEY_S),
                _delta_time);

    // Controls for right paddle.
    move_paddle(r_paddle_,
                is_key_pressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW),
                is_key_pressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW),
                is_key_pressed(EventKeyboard::KeyCode::KEY_UP_ARROW),
                is_key_pressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW),
                _delta_time);
}

auto pong::is_key_pressed(EventKeyboard::KeyCode _key_code) -> bool
{
    return key_state_[static_cast<int>(_key_code)];
}
