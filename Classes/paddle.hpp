#ifndef PONG_PADDLE_HPP
#define PONG_PADDLE_HPP

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace game {

class paddle
{
public:
    paddle(b2World& _world, float _half_width, float _half_height);

    void set_color(cocos2d::Color3B _color) noexcept;
    void set_position(float _x, float _y) noexcept;

    auto get_node() const noexcept -> DrawNode*;
    auto get_position() const noexcept -> cocos2d::Vec2;
    auto get_color() const noexcept -> cocos2d::Color4F;
    auto get_width() const noexcept -> float;
    auto get_height() const noexcept -> float;

private:
    b2World* world_;
    b2Body* body_;
    cocos2d::DrawNode* node_;
    float half_width_;
    float half_height_;
};

} // namespace game

#endif // PONG_PADDLE_HPP
