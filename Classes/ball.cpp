#include "ball.hpp"

namespace game {

ball::ball(b2World& _world, float _radius)
    : world_{&_world}
    , body_{}
    , node_{cocos2d::DrawNode::create()}
    , radius_{_radius}
{
    const Vec2 center{};
    const float angle{};
    const unsigned int segments = 10;
    node_->drawSolidCircle(center, radius_, angle, segments, cocos2d::Color4F::WHITE);

    {
        b2BodyDef def;
        def.type = b2_dynamicBody;
        def.bullet = true;
        body_ = world_.CreateBody(&def);
    }

    {
        b2CircleShape shape;
        shape.m_p.Set(0, 0);
        shape.m_radius = 1.f;

        b2FixtureDef def;
        def.density = 1.f;
        def.friction = 0.f;
        def.restitution = 1.f;
        def.shape = &shape;
        body_->CreateFixture(&def);
    }
}

void ball::set_color(cocos2d::Color3B _color) noexcept
{
    node_->setColor(_color);
}

void ball::set_radius(float _radius) noexcept
{
    radius_ = _radius;
}

void ball::set_position(float _x, float _y) noexcept
{
    node_->setPositionX(_x);
    node_->setPositionY(_y);
}

auto ball::get_node() const noexcept -> DrawNode*
{
    return node_;
}

auto ball::get_position() const noexcept -> cocos2d::Vec2
{
    return node_->getPosition();
}

auto ball::get_color() const noexcept -> cocos2d::Color4F
{
    auto c = node_->getColor();
    auto r = static_cast<float>(c.r);
    auto g = static_cast<float>(c.g);
    auto b = static_cast<float>(c.b);
    auto a = static_cast<float>(node_->getOpacity()) / 255;
    return {r, g, b, a};
}

auto ball::get_radius() const noexcept -> float
{
    return radius_;
}
    
} // namespace game
