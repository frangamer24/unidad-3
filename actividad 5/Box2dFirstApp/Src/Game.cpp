#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>

const float SCALE = 30.f;
const int WIDTH = 800;
const int HEIGHT = 600;

// Función para crear una caja
b2Body* createBox(b2World& world, float x, float y, float width, float height, bool dynamic = true) {
    b2BodyDef bodyDef;
    bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position.Set(x / SCALE, y / SCALE);
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox((width / 2) / SCALE, (height / 2) / SCALE);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);

    return body;
}

// Función para crear joints con límites
void createJoint(b2World& world, b2Body* bodyA, b2Body* bodyB, b2Vec2 anchor, float lowerAngle, float upperAngle) {
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = bodyA;
    jointDef.bodyB = bodyB;
    jointDef.localAnchorA = bodyA->GetLocalPoint(anchor);
    jointDef.localAnchorB = bodyB->GetLocalPoint(anchor);
    jointDef.collideConnected = false;
    jointDef.enableLimit = true;
    jointDef.lowerAngle = lowerAngle;
    jointDef.upperAngle = upperAngle;
    world.CreateJoint(&jointDef);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "actividad 5");

    b2Vec2 gravity(0.f, 9.8f);
    b2World world(gravity);

    // Crear paredes
    createBox(world, WIDTH / 2, HEIGHT, WIDTH, 20, false); // piso
    createBox(world, WIDTH / 2, 0, WIDTH, 20, false);       // techo
    createBox(world, 0, HEIGHT / 2, 20, HEIGHT, false);     // izquierda
    createBox(world, WIDTH, HEIGHT / 2, 20, HEIGHT, false); // derecha

    // Crear cuerpo del ragdoll
    b2Body* torso = createBox(world, 400, 300, 40, 60);
    b2Body* head = createBox(world, 400, 240, 30, 30);
    b2Body* leftArm = createBox(world, 360, 300, 15, 50);
    b2Body* rightArm = createBox(world, 440, 300, 15, 50);
    b2Body* leftLeg = createBox(world, 385, 370, 15, 50);
    b2Body* rightLeg = createBox(world, 415, 370, 15, 50);

    // Joints para unir partes
    createJoint(world, torso, head, head->GetWorldCenter(), -0.25f, 0.25f);
    createJoint(world, torso, leftArm, leftArm->GetWorldCenter(), -1.0f, 1.0f);
    createJoint(world, torso, rightArm, rightArm->GetWorldCenter(), -1.0f, 1.0f);
    createJoint(world, torso, leftLeg, leftLeg->GetWorldCenter(), -0.5f, 0.5f);
    createJoint(world, torso, rightLeg, rightLeg->GetWorldCenter(), -0.5f, 0.5f);

    sf::Clock clock;
    std::vector<b2Body*> parts = { torso, head, leftArm, rightArm, leftLeg, rightLeg };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Movimiento con flechas (aplica fuerza al torso)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            torso->ApplyForceToCenter(b2Vec2(-100.f, 0.f), true);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            torso->ApplyForceToCenter(b2Vec2(100.f, 0.f), true);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            torso->ApplyForceToCenter(b2Vec2(0.f, -150.f), true);
        }

        world.Step(clock.restart().asSeconds(), 8, 3);

        window.clear(sf::Color::Black);

        // Dibujar cada parte del ragdoll
        for (b2Body* body : parts) {
            for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
                if (f->GetShape()->GetType() == b2Shape::e_polygon) {
                    b2PolygonShape* shape = (b2PolygonShape*)f->GetShape();
                    sf::ConvexShape poly;
                    poly.setPointCount(shape->m_count);

                    for (int i = 0; i < shape->m_count; ++i) {
                        b2Vec2 point = body->GetWorldPoint(shape->m_vertices[i]);
                        poly.setPoint(i, sf::Vector2f(point.x * SCALE, point.y * SCALE));
                    }

                    poly.setFillColor(sf::Color::White);
                    window.draw(poly);
                }
            }
        }

        window.display();
    }

    return 0;
}

