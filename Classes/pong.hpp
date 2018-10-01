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

#ifndef __PONG_HPP__
#define __PONG_HPP__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include <array>
#include <memory>

class pong : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    CREATE_FUNC(pong);

    bool init() override;
    void update(float _delta_time) override;

private:
    static auto is_key_pressed(cocos2d::EventKeyboard::KeyCode _key_code) -> bool;

    void init_b2_world();

    std::unique_ptr<b2World> world_;
    b2Body* ball_body_;
    b2Body* l_paddle_body_;
    b2Body* r_paddle_body_;

    cocos2d::DrawNode* ball_{};
    cocos2d::DrawNode* l_paddle_{};
    cocos2d::DrawNode* r_paddle_{};

    cocos2d::Vec2 ball_dir_{};

    inline static std::array<bool, 164> key_state_{};
};

#endif // __PONG_HPP__
