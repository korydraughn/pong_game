#ifndef PONG_BALL_HPP
#define PONG_BALL_HPP

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace game {

class ball
{
public:
    ball(b2World& _world, float _radius);

    void set_color(cocos2d::Color4F _color) noexcept;
    void set_radius(float _radius) noexcept;
    void set_position(float _x, float _y) noexcept;

    auto get_node() const noexcept -> DrawNode*;
    auto get_position() const noexcept -> cocos2d::Vec2;
    auto get_color() const noexcept -> cocos2d::Color4F;
    auto get_radius() const noexcept -> float;

private:
    b2World* world_;
    b2Body* body_;
    cocos2d::DrawNode* node_;
    float radius_;
    cocos2d::Color4F color_;
};

} // namespace game

#endif // PONG_BALL_HPP
