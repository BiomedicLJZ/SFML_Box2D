#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

constexpr float SCALE = 30.0f;

class Cube {
public:
    Cube(b2World &world, float x, float y, int dynamic = 0,float density = 1.0f) {
        b2BodyDef bodydef;
        bodydef.position.Set(x / SCALE, y / SCALE);
        switch (dynamic) {
            case 0:
                bodydef.type = b2_staticBody;
                break;
            case 1:
                bodydef.type = b2_dynamicBody;
                break;
            case 2:
                bodydef.type = b2_kinematicBody;
                break;
        }
        body = world.CreateBody(&bodydef);

        b2PolygonShape shape;
        shape.SetAsBox((10.f / 2) / SCALE, (10.f / 2) / SCALE);

        b2FixtureDef fixturedef;
        fixturedef.shape = &shape;
        fixturedef.density = density;
        body->CreateFixture(&fixturedef);

        box.setSize(sf::Vector2f(10.f, 10.f));
        box.setFillColor(sf::Color::Red);
        box.setOrigin(5.f, 5.f);
    }

    void update() {
        box.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
        box.setRotation(body->GetAngle() * 180 / b2_pi);
    }

    void draw(sf::RenderWindow &win) const {
        win.draw(box);
    }

    void resetPosition() {
        body->SetTransform(b2Vec2(0, 0), body->GetAngle());
    }

    void stop() {
        body->SetLinearVelocity(b2Vec2(0, 0));
    }

    b2Body *getBody() {
        return body;
    }

private:
    sf::RectangleShape box;
    b2Body *body;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Space Invaders v0.0.1!");

    b2Vec2 gravity(0.0f, 0.0f);
    b2World world(gravity);

    Cube cube(world, 400, 300,1);

    Cube cube2(world, 500, 300, 1);

    Cube cube3(world, 600, 400, 1,100000.f);

    b2PrismaticJointDef jointDef;
    jointDef.Initialize(cube.getBody(), cube3.getBody(), cube3.getBody()->GetWorldCenter(), b2Vec2(1.0f, 0.0f));
    jointDef.lowerTranslation = -10.0f;
    jointDef.upperTranslation = 10.0f;
    jointDef.enableLimit = true;

    b2PrismaticJointDef jointDef2;
    jointDef2.Initialize(cube2.getBody(), cube3.getBody(), cube3.getBody()->GetWorldCenter(), b2Vec2(0.0f, 1.0f));
    jointDef2.lowerTranslation = -10.0f;
    jointDef2.upperTranslation = 10.0f;
    jointDef2.enableLimit = true;


    Cube cube4(world, 650, 450, 1);

    b2RevoluteJointDef jointDef3;

    jointDef3.Initialize(cube4.getBody(), cube3.getBody(), cube3.getBody()->GetWorldCenter());
    jointDef3.lowerAngle = -10.0f * b2_pi / 180.0f;
    jointDef3.upperAngle = 10.0f * b2_pi / 180.0f;
    jointDef3.enableLimit = false;
    jointDef3.enableMotor = false;
    jointDef3.maxMotorTorque = 1.0f;
    jointDef3.motorSpeed = 1.0f;


    b2PrismaticJoint* joint = (b2PrismaticJoint*)world.CreateJoint(&jointDef);
    b2PrismaticJoint* joint2 = (b2PrismaticJoint*)world.CreateJoint(&jointDef2);
    b2RevoluteJoint* joint3 = (b2RevoluteJoint*)world.CreateJoint(&jointDef3);

    b2GearJointDef gearJointDef;

    gearJointDef.bodyA = cube.getBody();
    gearJointDef.bodyB = cube2.getBody();
    gearJointDef.joint1 = joint;
    gearJointDef.joint2 = joint2;
    gearJointDef.ratio = 0.2f;

    world.CreateJoint(&gearJointDef);


    //Pulley Joint

    Cube myBody1(world, 600, 600, 1,1.0f);
    Cube myBody2(world, 700, 600, 1,1.0f);

    b2Vec2 anchor1 = myBody1.getBody()->GetWorldCenter();
    b2Vec2 anchor2 = myBody2.getBody()->GetWorldCenter();

    b2Vec2 p1= myBody1.getBody()->GetPosition();
    b2Vec2 p2= myBody2.getBody()->GetPosition();

    b2Vec2 groundAnchor1(p1.x, p1.y + 50.0f);
    b2Vec2 groundAnchor2(p2.x, p2.y + 52.0f);

    float ratio = 1.0f;

    b2PulleyJointDef pulleyjointDef;
    pulleyjointDef.Initialize(myBody1.getBody(), myBody2.getBody(), groundAnchor1, groundAnchor2, anchor1, anchor2, ratio);


    world.CreateJoint(&pulleyjointDef);

    myBody1.getBody()->ApplyForce(b2Vec2(0.0f, -0.1f), myBody1.getBody()->GetWorldCenter(), true);



    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                cube.getBody()->ApplyForceToCenter(b2Vec2(0.1f, 0.0f), true);
                cube2.getBody()->ApplyForceToCenter(b2Vec2(0.1f, 0.0f), true);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                cube.getBody()->ApplyForceToCenter(b2Vec2(-0.1f, 0.0f), true);
                cube2.getBody()->ApplyForceToCenter(b2Vec2(-0.1f, 0.0f), true);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                cube.getBody()->ApplyForceToCenter(b2Vec2(0.0f, -0.1f), true);
                cube2.getBody()->ApplyForceToCenter(b2Vec2(0.0f, -0.1f), true);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                cube.getBody()->ApplyForceToCenter(b2Vec2(0.0f, 0.1f), true);
                cube2.getBody()->ApplyForceToCenter(b2Vec2(0.0f, 0.1f), true);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                cube.resetPosition();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                cube.stop();
                cube2.stop();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                window.close();
            }
        }

        world.Step(1 / 60.f, 8, 3);
        cube.update();
        cube2.update();
        cube3.update();
        cube4.update();
        myBody1.update();
        myBody2.update();

        window.clear();
        cube.draw(window);
        cube2.draw(window);
        cube3.draw(window);
        cube4.draw(window);
        myBody1.draw(window);
        myBody2.draw(window);
        window.display();
    }
    return 0;
}
