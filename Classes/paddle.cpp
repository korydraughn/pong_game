#include "paddle.hpp"

namespace game {

paddle::paddle(b2World& _world, float _half_width, float _half_height)
    : world_{&_world}
    , body_{}
    , node_{cocos2d::DrawNode::create()}
    , half_width_{_half_width}
    , half_height_{_half_height}
    , color_{cocos2d::Color4F::WHITE}
{
    Vec2 origin{0, 0};
    Vec2 dst{paddle_w, paddle_h};
    const auto vsize_half = Director::getInstance()->getVisibleSize().height / 2;

    node_->drawSolidRect(origin, dst, _color);
    node_->setAnchorPoint({});
    node_->setPositionY(vsize_half - (paddle_h / 2));

    {
        b2BodyDef def;
        def.type = b2_kinematicBody;
        body_ = _world.CreateBody(&def);
    }

    {
        b2PolygonShape shape;
        shape.SetAsBox(half_width_, half_height_);

        b2FixtureDef def;
        def.density = 1.f;
        def.friction = 0.f;
        def.restitution = 1.f;
        def.shape = &shape;
        body_->CreateFixture(&def);
    }
}

void paddle::set_color(cocos2d::Color3B _color) noexcept
{
    node_->setColor(_color);
}

void paddle::set_radius(float _radius) noexcept
{
    radius_ = _radius;
}

void paddle::set_position(float _x, float _y) noexcept
{
    node_->setPositionX(_x);
    node_->setPositionY(_y);
}

auto paddle::get_node() const noexcept -> DrawNode*
{
    return node_;
}

auto paddle::get_position() const noexcept -> cocos2d::Vec2
{
    return node_->getPosition();
}

auto paddle::get_color() const noexcept -> cocos2d::Color4F
{
    auto c = node_->getColor();
    auto r = static_cast<float>(c.r);
    auto g = static_cast<float>(c.g);
    auto b = static_cast<float>(c.b);
    auto a = static_cast<float>(node_->getOpacity()) / 255;
    return {r, g, b, a};
}

auto paddle::get_radius() const noexcept -> float
{
    return radius_;
}
    
} // namespace game
