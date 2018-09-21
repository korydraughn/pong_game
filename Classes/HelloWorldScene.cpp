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

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
        return false;

    //auto visibleSize = Director::getInstance()->getVisibleSize();
    //Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Draw paddles.
    auto* left_paddle = DrawNode::create();
    if (left_paddle)
    {
        Vec2 r_origin{};
        Vec2 r_dst{100, 100};
        Color4F r_color{1, 1, 1, 1};
        left_paddle->drawSolidRect(r_origin, r_dst, r_color);
        addChild(left_paddle, 0);
    }

    // Add keyboard support.
    auto* listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [](EventKeyboard::KeyCode _key_code, Event* _event)
    {
        log("Key with keycode %d pressed", _key_code);

        auto* target = _event->getCurrentTarget();

        switch (_key_code)
        {
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                target->setPositionY(target->getPositionY() + 1);
                //target->runAction(MoveBy::create(1, {0.f, 4.f}));
                break;

            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                target->setPositionY(target->getPositionY() - 1);
                //target->runAction(MoveBy::create(1, {0.f, -4.f}));
                break;

            default:
                break;
        }
    };

    listener->onKeyReleased = [](auto _key_code, auto* _event)
    {
        log("Key with keycode %d released", _key_code);
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, left_paddle);

    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application,
     * do not use Director::getInstance()->end() and exit(0) as given above,instead trigger
     * a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
