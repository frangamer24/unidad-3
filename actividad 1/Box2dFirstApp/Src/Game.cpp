#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>

const float SCALE = 30.f;

class BallContactListener : public b2ContactListener {
public:
    b2Body* ball;

    BallContactListener(b2Body* ballBody) : ball(ballBody) {}

    void BeginContact(b2Contact* contact) override {
        // Si una de las partes del contacto es la pelota
        if (contact->GetFixtureA()->GetBody() == ball || contact->GetFixtureB()->GetBody() == ball) {
            b2Vec2 velocity = ball->GetLinearVelocity();
            // Aumentamos un poco la velocidad
            float speedMultiplier = 1.1f; // 10% más rápido
            velocity *= speedMultiplier;

            // Limitar velocidad máxima para evitar que se vuelva incontrolable
            if (velocity.Length() > 50.f) velocity.Normalize(), velocity *= 50.f;

            ball->SetLinearVelocity(velocity);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pelota con rebote acelerado");

    b2Vec2 gravity(0.f, 0.f);
    b2World world(gravity);

    // Bordes de la pantalla
    b2BodyDef borderDef;
    borderDef.position.Set(0.f, 0.f);
    b2Body* borders = world.CreateBody(&borderDef);

    b2EdgeShape edge;
    edge.SetTwoSided(b2Vec2(0, 600 / SCALE), b2Vec2(800 / SCALE, 600 / SCALE));
    borders->CreateFixture(&edge, 0.f);
    edge.SetTwoSided(b2Vec2(0, 0), b2Vec2(800 / SCALE, 0));
    borders->CreateFixture(&edge, 0.f);
    edge.SetTwoSided(b2Vec2(0, 0), b2Vec2(0, 600 / SCALE));
    borders->CreateFixture(&edge, 0.f);
    edge.SetTwoSided(b2Vec2(800 / SCALE, 0), b2Vec2(800 / SCALE, 600 / SCALE));
    borders->CreateFixture(&edge, 0.f);

    // Crear la pelota
    b2BodyDef ballDef;
    ballDef.type = b2_dynamicBody;
    ballDef.position.Set(400 / SCALE, 300 / SCALE);
    b2Body* ball = world.CreateBody(&ballDef);

    b2CircleShape circle;
    circle.m_radius = 20.f / SCALE;

    b2FixtureDef ballFixture;
    ballFixture.shape = &circle;
    ballFixture.density = 1.0f;
    ballFixture.friction = 0.0f;
    ballFixture.restitution = 1.0f; // Rebote perfecto
    ball->CreateFixture(&ballFixture);

    ball->SetLinearVelocity(b2Vec2(5.f, 3.f));

    // Contact listener que acelera la pelota en los rebotes
    BallContactListener contactListener(ball);
    world.SetContactListener(&contactListener);

    // Parte visual
    sf::CircleShape graphicBall(20.f);
    graphicBall.setFillColor(sf::Color::Red);
    graphicBall.setOrigin(20.f, 20.f);

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();
        world.Step(dt, 8, 3);

        b2Vec2 pos = ball->GetPosition();
        graphicBall.setPosition(pos.x * SCALE, pos.y * SCALE);

        window.clear(sf::Color::Black);
        window.draw(graphicBall);
        window.display();
    }

    return 0;
}
