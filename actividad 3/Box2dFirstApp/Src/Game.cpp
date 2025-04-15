#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

const float SCALE = 30.f;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "actividad 3");

    b2Vec2 gravity(0.f, 0.f);
    b2World world(gravity);

    // Crear la pelota dinámica
    b2BodyDef ballDef;
    ballDef.type = b2_dynamicBody;
    ballDef.position.Set(400 / SCALE, 300 / SCALE);
    b2Body* ball = world.CreateBody(&ballDef);

    b2CircleShape circleShape;
    circleShape.m_radius = 20.f / SCALE;

    b2FixtureDef fixture;
    fixture.shape = &circleShape;
    fixture.density = 1.0f;
    fixture.restitution = 0.5f;
    ball->CreateFixture(&fixture);

    // Crear el punto fijo (objeto estático invisible)
    b2BodyDef anchorDef;
    anchorDef.position.Set(400 / SCALE, 150 / SCALE);
    b2Body* anchor = world.CreateBody(&anchorDef); // tipo por defecto es estático

    // Crear el resorte entre pelota y punto fijo
    b2DistanceJointDef jointDef;
    jointDef.Initialize(ball, anchor, ball->GetWorldCenter(), anchor->GetWorldCenter());
    jointDef.collideConnected = false;
   
    b2DistanceJoint* joint = (b2DistanceJoint*)world.CreateJoint(&jointDef);

    // Gráficos
    sf::CircleShape ballShape(20.f);
    ballShape.setOrigin(20.f, 20.f);
    ballShape.setFillColor(sf::Color::Green);

    sf::CircleShape anchorShape(5.f);
    anchorShape.setOrigin(5.f, 5.f);
    anchorShape.setFillColor(sf::Color::Red);

    // Variables de interacción
    sf::Clock clock;
    b2Body* selectedBody = nullptr;
    bool dragging = false;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                b2Vec2 mouseWorld(mousePos.x / SCALE, mousePos.y / SCALE);

                b2Vec2 pos = ball->GetPosition();
                float dx = pos.x - mouseWorld.x;
                float dy = pos.y - mouseWorld.y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < circleShape.m_radius) {
                    selectedBody = ball;
                    dragging = true;
                }
            }

            if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
                dragging = false;
                if (selectedBody) {
                    selectedBody->SetLinearVelocity(b2Vec2(0.f, 0.f));
                    selectedBody = nullptr;
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

        ballShape.setPosition(ball->GetPosition().x * SCALE, ball->GetPosition().y * SCALE);
        anchorShape.setPosition(anchor->GetPosition().x * SCALE, anchor->GetPosition().y * SCALE);

        window.clear(sf::Color::Black);
        window.draw(ballShape);
        window.draw(anchorShape);

        // Dibujar el resorte
        sf::Vertex line[] = {
            sf::Vertex(ballShape.getPosition(), sf::Color::White),
            sf::Vertex(anchorShape.getPosition(), sf::Color::White)
        };
        window.draw(line, 2, sf::Lines);

        window.display();
    }

    return 0;
}
