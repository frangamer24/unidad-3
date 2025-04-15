#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

const float SCALE = 30.f;
const int WIDTH = 800;
const int HEIGHT = 600;

void createWall(b2World& world, float x, float y, float width, float height) {
    b2BodyDef wallDef;
    wallDef.position.Set(x / SCALE, y / SCALE);
    b2Body* wall = world.CreateBody(&wallDef);

    b2PolygonShape wallShape;
    wallShape.SetAsBox(width / SCALE, height / SCALE);

    b2FixtureDef fixture;
    fixture.shape = &wallShape;
    fixture.density = 0.f;
    fixture.friction = 0.5f;

    wall->CreateFixture(&fixture);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cuadrado con Paredes");

    b2Vec2 gravity(0.f, 0.f); // sin gravedad
    b2World world(gravity);

    // Cuadrado dinámico
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(WIDTH / 2.f / SCALE, HEIGHT / 2.f / SCALE);
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(25.f / SCALE, 25.f / SCALE);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);

    // Crear paredes
    float wallThickness = 10.f;
    createWall(world, WIDTH / 2.f, wallThickness / 2.f, WIDTH / 2.f, wallThickness / 2.f); // Arriba
    createWall(world, WIDTH / 2.f, HEIGHT - wallThickness / 2.f, WIDTH / 2.f, wallThickness / 2.f); // Abajo
    createWall(world, wallThickness / 2.f, HEIGHT / 2.f, wallThickness / 2.f, HEIGHT / 2.f); // Izquierda
    createWall(world, WIDTH - wallThickness / 2.f, HEIGHT / 2.f, wallThickness / 2.f, HEIGHT / 2.f); // Derecha

    sf::RectangleShape square(sf::Vector2f(50.f, 50.f));
    square.setOrigin(25.f, 25.f);
    square.setFillColor(sf::Color::Cyan);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        b2Vec2 force(0.f, 0.f);
        float fuerza = 20.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  force.x = -fuerza;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) force.x = fuerza;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    force.y = -fuerza;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  force.y = fuerza;

        body->ApplyForceToCenter(force, true);

        float dt = clock.restart().asSeconds();
        world.Step(dt, 8, 3);

        b2Vec2 pos = body->GetPosition();
        square.setPosition(pos.x * SCALE, pos.y * SCALE);
        square.setRotation(body->GetAngle() * 180 / b2_pi);

        window.clear(sf::Color::Black);
        window.draw(square);
        window.display();
    }

    return 0;
}
