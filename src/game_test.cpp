#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "game_main.cpp"
#include <cmath>
#include <memory>
#include <vector>

TEST_CASE("Enemy visibility") {
    Enemy e;
    e.set_visibility(true);
    CHECK(e.shape.getFillColor() == sf::Color(255, 200, 100));
    e.set_visibility(false);
    CHECK(e.shape.getFillColor() == sf::Color(200, 60, 60));
}

TEST_CASE("Enemy velocity") {
    Enemy e;
    sf::Vector2f vel(3.f, -2.f);
    e.set_velocity(vel);
    CHECK(e.get_velocity().x == doctest::Approx(3.f));
    CHECK(e.get_velocity().y == doctest::Approx(-2.f));
}

TEST_CASE("Echo hitsEnemy") {
    Enemy e;
    e.shape = sf::CircleShape(5.f);
    e.shape.setOrigin(sf::Vector2f(5.f, 5.f)); 
    e.shape.setPosition(sf::Vector2f(50.f, 50.f)); 
    
    Echo echo;
    echo.length = 20.f;
    echo.velocity = sf::Vector2f(1.f, 0.f);
    echo.shape = sf::RectangleShape(sf::Vector2f(echo.length, 6.f));
    echo.shape.setOrigin(sf::Vector2f(echo.length/2.f, 3.f)); 
    echo.shape.setPosition(sf::Vector2f(50.f, 50.f));
    echo.shape.setRotation(sf::degrees(0.f));
    
    CHECK(echo.hitsEnemy(e, 6.f) == true);
    
    // Move enemy far away
    e.shape.setPosition(sf::Vector2f(200.f, 200.f));  
    CHECK(echo.hitsEnemy(e, 6.f) == false);
}

TEST_CASE("BigEcho hitsEnemy") {
    Enemy e;
    e.shape = sf::CircleShape(5.f);
    e.shape.setOrigin(sf::Vector2f(5.f, 5.f));  
    e.shape.setPosition(sf::Vector2f(100.f, 100.f));  
    
    BigEcho be;
    be.radius = 50.f;
    be.shape = sf::CircleShape(be.radius);
    be.shape.setOrigin(sf::Vector2f(be.radius, be.radius));  
    be.shape.setPosition(sf::Vector2f(100.f, 100.f));
    
    CHECK(be.hitsEnemy(e, 0.f) == true);
    
    e.shape.setPosition(sf::Vector2f(200.f, 200.f));  
    CHECK(be.hitsEnemy(e, 0.f) == false);
}

TEST_CASE("Echo update moves and shrinks") {
    Echo echo;
    echo.length = 20.f;
    echo.velocity = sf::Vector2f(1.f, 0.f);
    echo.shape = sf::RectangleShape(sf::Vector2f(echo.length, 6.f));
    echo.shape.setOrigin(sf::Vector2f(echo.length/2.f, 3.f));  
    
    sf::Vector2f center(0.f, 0.f);
    echo.update(1.f, 2.f, 10.f, 6.f, center);
    
    CHECK(echo.length == doctest::Approx(18.f));
    CHECK(echo.shape.getPosition().x == doctest::Approx(10.f));
    CHECK(echo.shape.getPosition().y == doctest::Approx(0.f));
}

TEST_CASE("BigEcho update shrinks") {
    BigEcho be;
    be.radius = 40.f;
    be.shape = sf::CircleShape(be.radius);
    be.shape.setOrigin(sf::Vector2f(be.radius, be.radius)); 
    
    sf::Vector2f center(0.f, 0.f);
    be.update(1.f, 5.f, 0.f, 0.f, center);
    
    CHECK(be.radius == doctest::Approx(35.f));
    CHECK(be.shape.getPosition().x == doctest::Approx(0.f));
    CHECK(be.shape.getPosition().y == doctest::Approx(0.f));
}