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

Scene* pong::createScene()
{
    return pong::create();
}

// on "init" you need to initialize your instance
auto pong::init() -> bool
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
        return false;

    // auto visibleSize = Director::getInstance()->getVisibleSize();
    // Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Draw paddles.
    l_paddle_ = DrawNode::create();
    if (l_paddle_)
    {
        Vec2 r_origin{};
        Vec2 r_dst{100, 100};
        Color4F r_color{1, 1, 1, 1};
        l_paddle_->drawSolidRect(r_origin, r_dst, r_color);
        addChild(l_paddle_, 0);
    }

    // Add keyboard support.
    auto* listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [](EventKeyboard::KeyCode _key_code, Event* _event) {
        log("Key with keycode %d pressed", _key_code);
        key_state_[static_cast<int>(_key_code)] = true;
    };

    listener->onKeyReleased = [](auto _key_code, auto* _event) {
        log("Key with keycode %d released", _key_code);
        key_state_[static_cast<int>(_key_code)] = false;
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    scheduleUpdate();

    return true;
}

auto pong::menuCloseCallback(Ref* pSender) -> void
{
    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application,
     * do not use Director::getInstance()->end() and exit(0) as given above,instead trigger
     * a custom event created in RootViewController.mm as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

auto pong::update(float _delta_time) -> void
{
    Node::update(_delta_time);

    if (!l_paddle_)
        return;

    auto speed = 50.f * _delta_time;
    auto pos = l_paddle_->getPosition();

    if (is_key_pressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    {
        l_paddle_->setPositionX(pos.x - speed);
    }
    else if (is_key_pressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    {
        l_paddle_->setPositionX(pos.x + speed);
    }

    if (is_key_pressed(EventKeyboard::KeyCode::KEY_UP_ARROW))
    {
        l_paddle_->setPositionY(pos.y + speed);
    }
    else if (is_key_pressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    {
        l_paddle_->setPositionY(pos.y - speed);
    }
}

auto pong::is_key_pressed(EventKeyboard::KeyCode _key_code) -> bool
{
    return key_state_[static_cast<int>(_key_code)];
}
