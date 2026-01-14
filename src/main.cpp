#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <variant>

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

struct Enemy {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

int main() {
    const unsigned int WINDOW_W = 800;
    const unsigned int WINDOW_H = 600;
    const sf::Vector2f CENTER(WINDOW_W / 2.f, WINDOW_H / 2.f);

    // VideoMode in SFML 3 accepts a Vector2u
    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "Turret Waves - SFML");
    window.setFramerateLimit(60);

    // Random generator
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> angleDist(0.f, 2 * 3.14159265f);

    // Turret parameters
    float turretAngleDeg = 0.f; // degrees
    const float rotationSpeedDegPerSec = 140.f; // how fast it turns when holding keys
    const float barrelLength = 46.f;
    const float barrelThickness = 12.f;
    const float turretRadius = 18.f;

    // Shooting
    const float bulletSpeed = 520.f;
    const float bulletRadius = 4.f;
    const float fireCooldown = 0.20f;
    float timeSinceLastShot = fireCooldown;

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    int wave = 1;
    bool waveActive = false;
    float nextWaveTimer = 0.f;
    const float timeBetweenWaves = 1.0f;

    sf::Font font;
    bool fontLoaded = false;
    // SFML 3: openFromFile()
    if (font.openFromFile("assets/sansation.ttf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Warning: could not open font assets/sansation.ttf. UI text will be hidden.\n";
    }

    // Text in SFML 3 requires a Font in the constructor
    sf::Text uiText(font, "", 18);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition(sf::Vector2f(8.f, 8.f));

    // Helper to spawn a wave (spawn count increases each wave)
    auto spawnWave = [&](int waveNumber) {
        int count = 4 + waveNumber * 2;
        enemies.clear();
        enemies.reserve(count);
        float spawnRadius = std::max(WINDOW_W, WINDOW_H) / 2.f + 50.f;
        for (int i = 0; i < count; ++i) {
            float a = angleDist(rng);
            sf::Vector2f pos = CENTER + sf::Vector2f(std::cos(a), std::sin(a)) * spawnRadius;
            Enemy e;
            e.shape = sf::CircleShape(14.f);
            e.shape.setOrigin(sf::Vector2f(e.shape.getRadius(), e.shape.getRadius()));
            e.shape.setPosition(pos);
            // velocity towards center
            sf::Vector2f dir = CENTER - pos;
            float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
            if (len != 0) dir /= len;
            float speed = 40.f + 8.f * waveNumber + (std::uniform_real_distribution<float>(-10.f, 10.f)(rng));
            e.velocity = dir * speed;
            e.shape.setFillColor(sf::Color(200, 60, 60));
            enemies.push_back(e);
        }
        waveActive = true;
    };

    // Start first wave immediately
    spawnWave(wave);

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        timeSinceLastShot += dt;

        // Events: pollEvent returns std::optional<Event> in SFML 3
        while (auto evOpt = window.pollEvent()) {
            const auto &ev = *evOpt;
            // use the is<T>() helper in SFML 3 to check event type
            if (ev.is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Input: rotate turret with left/right arrows
        float rotationThisFrame = 0.f;
        // SFML 3 key enum is nested under sf::Keyboard::Key
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            rotationThisFrame -= rotationSpeedDegPerSec * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            rotationThisFrame += rotationSpeedDegPerSec * dt;
        }
        turretAngleDeg += rotationThisFrame;
        // keep angle normalized
        if (turretAngleDeg > 360.f) turretAngleDeg -= 360.f;
        if (turretAngleDeg < 0.f) turretAngleDeg += 360.f;

        // Shooting: spacebar
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && timeSinceLastShot >= fireCooldown) {
            timeSinceLastShot = 0.f;
            // create bullet
            Bullet b;
            b.shape = sf::CircleShape(bulletRadius);
            b.shape.setOrigin(sf::Vector2f(bulletRadius, bulletRadius));
            b.shape.setPosition(CENTER);
            // compute direction from turretAngleDeg
            float rad = turretAngleDeg * 3.14159265f / 180.f;
            sf::Vector2f dir(std::cos(rad), std::sin(rad));
            b.velocity = dir * bulletSpeed;
            b.shape.setFillColor(sf::Color::Yellow);
            bullets.push_back(b);
        }

        // Update bullets
        for (size_t i = 0; i < bullets.size(); ) {
            Bullet &b = bullets[i];
            b.shape.move(b.velocity * dt);
            sf::Vector2f p = b.shape.getPosition();
            // remove bullet if outside window bounds (with margin)
            if (p.x < -50 || p.x > WINDOW_W + 50 || p.y < -50 || p.y > WINDOW_H + 50) {
                bullets.erase(bullets.begin() + i);
            } else ++i;
        }

        // Update enemies
        for (size_t i = 0; i < enemies.size(); ++i) {
            enemies[i].shape.move(enemies[i].velocity * dt);
        }

        // Collision detection: bullets vs enemies
        for (size_t bi = 0; bi < bullets.size(); ) {
            bool bulletRemoved = false;
            sf::Vector2f bp = bullets[bi].shape.getPosition();
            for (size_t ei = 0; ei < enemies.size(); ++ei) {
                sf::Vector2f ep = enemies[ei].shape.getPosition();
                float dx = bp.x - ep.x;
                float dy = bp.y - ep.y;
                float dist2 = dx*dx + dy*dy;
                float rsum = bullets[bi].shape.getRadius() + enemies[ei].shape.getRadius();
                if (dist2 <= rsum * rsum) {
                    // hit: remove both bullet and enemy (one-shot kill)
                    enemies.erase(enemies.begin() + ei);
                    bullets.erase(bullets.begin() + bi);
                    bulletRemoved = true;
                    break;
                }
            }
            if (!bulletRemoved) ++bi;
        }

        // Check if any enemy reached the center -> remove them (could be lives)
        for (size_t i = 0; i < enemies.size(); ) {
            sf::Vector2f ep = enemies[i].shape.getPosition();
            float dx = ep.x - CENTER.x;
            float dy = ep.y - CENTER.y;
            float dist2 = dx*dx + dy*dy;
            if (dist2 <= (turretRadius + enemies[i].shape.getRadius()) * (turretRadius + enemies[i].shape.getRadius())) {
                // enemy reached turret: remove it
                enemies.erase(enemies.begin() + i);
            } else ++i;
        }

        // Wave logic
        if (waveActive && enemies.empty()) {
            waveActive = false;
            nextWaveTimer = 0.f;
        }
        if (!waveActive) {
            nextWaveTimer += dt;
            if (nextWaveTimer >= timeBetweenWaves) {
                ++wave;
                spawnWave(wave);
            }
        }

        // Drawing
        window.clear(sf::Color(30, 30, 30));

        // Draw enemies
        for (auto &e : enemies) window.draw(e.shape);

        // Draw bullets
        for (auto &b : bullets) window.draw(b.shape);

        // Draw turret: base circle
        sf::CircleShape base(turretRadius);
        base.setOrigin(sf::Vector2f(turretRadius, turretRadius));
        base.setPosition(CENTER);
        base.setFillColor(sf::Color(120, 180, 220));
        window.draw(base);

        // Draw barrel (rectangle) rotated by turretAngleDeg
        sf::RectangleShape barrel(sf::Vector2f(barrelLength, barrelThickness));
        barrel.setOrigin(sf::Vector2f(6.f, barrelThickness / 2.f)); // small offset so barrel doesn't sink into the center
        barrel.setPosition(CENTER);
        barrel.setRotation(sf::degrees(turretAngleDeg));
        barrel.setFillColor(sf::Color(200, 200, 100));
        window.draw(barrel);

        // center dot
        sf::CircleShape centerDot(4.f);
        centerDot.setOrigin(sf::Vector2f(4.f, 4.f));
        centerDot.setPosition(CENTER);
        centerDot.setFillColor(sf::Color::White);
        window.draw(centerDot);

        // UI text
        if (fontLoaded) {
            uiText.setString("Wave: " + std::to_string(wave) +
                             "    Enemies: " + std::to_string((int)enemies.size()) +
                             "    Bullets: " + std::to_string((int)bullets.size()) +
                             "\nControls: Left/Right to rotate, Space to fire");
            window.draw(uiText);
        }

        window.display();
    }

    return 0;
}