#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

const float SCALE = 30.f;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "actividad 2");

    b2Vec2 gravity(0.f, 0.f);
    b2World world(gravity);

    b2BodyDef ballDef1, ballDef2;
    ballDef1.type = b2_dynamicBody;
    ballDef1.position.Set(300 / SCALE, 300 / SCALE);
    b2Body* ball1 = world.CreateBody(&ballDef1);

    b2CircleShape circleShape;
    circleShape.m_radius = 20.f / SCALE;

    b2FixtureDef fixture;
    fixture.shape = &circleShape;
    fixture.density = 1.0f;
    fixture.restitution = 0.5f;

    ball1->CreateFixture(&fixture);

    ballDef2.type = b2_dynamicBody;
    ballDef2.position.Set(500 / SCALE, 300 / SCALE);
    b2Body* ball2 = world.CreateBody(&ballDef2);
    ball2->CreateFixture(&fixture);

    // Crear el resorte
    b2DistanceJointDef jointDef;
    jointDef.Initialize(ball1, ball2, ball1->GetWorldCenter(), ball2->GetWorldCenter());
    jointDef.collideConnected = true;
    b2DistanceJoint* distanceJoint = (b2DistanceJoint*)world.CreateJoint(&jointDef);

    sf::CircleShape shape1(20.f), shape2(20.f);
    shape1.setOrigin(20.f, 20.f);
    shape2.setOrigin(20.f, 20.f);
    shape1.setFillColor(sf::Color::Blue);
    shape2.setFillColor(sf::Color::Green);

    sf::Clock clock;
    b2Body* selectedBody = nullptr;
    bool dragging = false;
    bool jointEnabled = true;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                b2Vec2 mouseWorld(mousePos.x / SCALE, mousePos.y / SCALE);

                for (b2Body* body : { ball1, ball2 }) {
                    b2Vec2 pos = body->GetPosition();
                    float dx = pos.x - mouseWorld.x;
                    float dy = pos.y - mouseWorld.y;
                    float dist = std::sqrt(dx * dx + dy * dy);

                    if (dist < circleShape.m_radius) {
                        selectedBody = body;
                        dragging = true;

                        // Desactivar temporalmente el resorte
                        if (distanceJoint && jointEnabled) {
                            world.DestroyJoint(distanceJoint);
                            distanceJoint = nullptr;
                            jointEnabled = false;
                        }

                        break;
                    }
                }
            }

            if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
                if (selectedBody) {
                    selectedBody->SetLinearVelocity(b2Vec2(0.f, 0.f));
                    selectedBody = nullptr;

                    // Volver a activar el resorte
                    if (!jointEnabled) {
                        b2DistanceJointDef jointDef;
                        jointDef.Initialize(ball1, ball2, ball1->GetWorldCenter(), ball2->GetWorldCenter());
                        jointDef.collideConnected = true;
                        distanceJoint = (b2DistanceJoint*)world.CreateJoint(&jointDef);
                        jointEnabled = true;
                    }
                }
            }
        }

        if (dragging && selectedBody) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            b2Vec2 mouseWorld(mousePos.x / SCALE, mousePos.y / SCALE);

            b2Vec2 pos = selectedBody->GetPosition();
            b2Vec2 force = mouseWorld - pos;
            force *= 50.0f;

            selectedBody->ApplyForceToCenter(force, true);
        }

        float dt = clock.restart().asSeconds();
        world.Step(dt, 8, 3);

        shape1.setPosition(ball1->GetPosition().x * SCALE, ball1->GetPosition().y * SCALE);
        shape2.setPosition(ball2->GetPosition().x * SCALE, ball2->GetPosition().y * SCALE);

        window.clear(sf::Color::Black);
        window.draw(shape1);
        window.draw(shape2);

        if (distanceJoint) {
            sf::Vertex line[] = {
                sf::Vertex(shape1.getPosition(), sf::Color::White),
                sf::Vertex(shape2.getPosition(), sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        window.display();
    }

    return 0;
}
