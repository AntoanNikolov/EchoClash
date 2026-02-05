#include <memory>
#include <filesystem>
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

class Enemy {
private:
    bool visible = false;
    sf::Vector2f velocity;
public:
    sf::CircleShape shape;
    float visibilityTimer = 0.f; // seconds remaining the enemy stays "visible"

    void set_visibility(bool v);
    auto set_velocity(const sf::Vector2f& vel) {
        velocity = vel;
        return velocity;
    }
    sf::Vector2f get_velocity() const {
        return velocity;
    }
};

void Enemy::set_visibility(bool v) {
    visible = v;
    // Optional: change visual appearance immediately when visibility toggles
    if (visible) {
        // e.g. brighter color to indicate "hit"
        shape.setFillColor(sf::Color(255, 200, 100));
    } else {
        // restore normal color when visibility ends
        shape.setFillColor(sf::Color(200, 60, 60));
    }
}

class EchoBase {
public:
    float elapsedTime = 0.f;
    sf::Vector2f velocity;
    
    //Virtual destructor ensures correct cleanup when deleting Echo or BigEcho through a base pointer
    virtual ~EchoBase() = default;
    virtual void update(float dt, float shrinkRate, float speed, float thickness, const sf::Vector2f& center) = 0;
    virtual bool hitsEnemy(const Enemy& enemy, float thickness) const = 0;

};

struct Echo : public EchoBase {
    sf::RectangleShape shape;
    float length;
    void update(float dt, float shrinkRate, float speed, float thickness, const sf::Vector2f& center) override {
        elapsedTime += dt;
        
        // Rectangle shrinks at constant rate
        length = (length - dt * shrinkRate);
        
        // Update rectangle size according to the above change
        shape.setSize(sf::Vector2f(length, thickness));
        shape.setOrigin(sf::Vector2f(length / 2.f, thickness / 2.f)); // recenter as it shrinks
        
        // Move outward from center
        sf::Vector2f offset = velocity * speed * elapsedTime;
        // ^ The above line caluclates how far an echo has traveled by multiplying its direction by speed and total time alive
        shape.setPosition(center + offset);
    }

    bool hitsEnemy(const Enemy& enemy, float thickness) const override {
    if (length <= 0.f) return false;

    sf::Vector2f enemyPos = enemy.shape.getPosition();
    float enemyR = enemy.shape.getRadius();

    sf::Transform inv = shape.getTransform().getInverse();
    sf::Vector2f local = inv.transformPoint(enemyPos);

    float halfW = length / 2.f;
    float halfH = thickness / 2.f;

    float cx = std::max(-halfW, std::min(local.x, halfW));
    float cy = std::max(-halfH, std::min(local.y, halfH));

    float dx = local.x - cx;
    float dy = local.y - cy;

    return (dx*dx + dy*dy) <= enemyR * enemyR;
    }

};

struct BigEcho : public EchoBase {
    sf::CircleShape shape;
    float radius;
    void update(float dt, float shrinkRate, float speed, float thickness, const sf::Vector2f& center) override {
        elapsedTime += dt;
        
        // Circle shrinks at constant rate
        radius = (radius - dt * shrinkRate);
        
        // Update circle size according to the above change
        shape.setRadius(radius);
        shape.setOrigin(sf::Vector2f(radius, radius)); // recenter as it shrinks
        shape.setPosition(center);
    }
    bool hitsEnemy(const Enemy& enemy, float) const override {
    if (radius <= 0.f) return false;

    sf::Vector2f ep = enemy.shape.getPosition();
    sf::Vector2f cp = shape.getPosition();
    float er = enemy.shape.getRadius();

    float dx = ep.x - cp.x;
    float dy = ep.y - cp.y;
    float r = radius + er;

    return (dx*dx + dy*dy) <= r * r;
}

};
#ifndef TESTING // wrapping to avoid conflicts with test suite's main()
int main() {
    const unsigned int WINDOW_W = 800;
    const unsigned int WINDOW_H = 600;
    const sf::Vector2f CENTER(WINDOW_W / 2.f, WINDOW_H / 2.f);

    // VideoMode in SFML 3 accepts a Vector2u
    sf::RenderWindow window(sf::VideoMode({WINDOW_W, WINDOW_H}), "EchoClash");
    window.requestFocus();
    window.setFramerateLimit(60);

    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    sf::Font font;
    bool fontLoaded = false;
    // SFML 3: openFromFile()
    if (font.openFromFile("assets/ARIAL.ttf")) {
    fontLoaded = true;
    } else {
        std::cerr << "Warning: could not open font. UI text will be hidden.\n";
    }

    // Text in SFML 3 requires a Font in the constructor
    sf::Text uiText(font, "", 18);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition(sf::Vector2f(350.f, 8.f));

    // pause menu text
    sf::Text pauseText(font, "PAUSED", 40);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(sf::Vector2f(300.f, 150.f));
    sf::Text resumeText(font, "Resume", 32);
    resumeText.setFillColor(sf::Color::White);
    resumeText.setPosition(sf::Vector2f(310.f, 215.f));
    sf::Text quitText(font, "Quit", 32);
    quitText.setFillColor(sf::Color::White);
    quitText.setPosition(sf::Vector2f(310.f, 305.f));

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

    // Charge bar
    const float barWidth = 20.f;

    // Make Charge Bar
    sf::RectangleShape chargeBarBackground(sf::Vector2f(26.f, 157.f));
    sf::RectangleShape chargeBar(sf::Vector2f(barWidth, 0.f));
    chargeBar.setOrigin(sf::Vector2f(-10.f, -590.f));
    chargeBar.setFillColor(sf::Color::Cyan);
    chargeBarBackground.setOrigin(sf::Vector2f(-7.5, -436.f));
    chargeBarBackground.setFillColor(sf::Color(100, 100, 100));

    // Make Big Wave Charge Bar (on the right side)
    sf::RectangleShape bigWaveChargeBarBackground(sf::Vector2f(26.f, 207.f)); // increased 2nd value to accomodate the big wave but it still shrinks from 0-157. 
    sf::RectangleShape bigWaveChargeBar(sf::Vector2f(barWidth, 0.f));
    bigWaveChargeBar.setOrigin(sf::Vector2f(-770.f, -590.f));
    bigWaveChargeBar.setFillColor(sf::Color::Magenta);
    bigWaveChargeBarBackground.setOrigin(sf::Vector2f(-767.5, -436.f)); 
    bigWaveChargeBarBackground.setFillColor(sf::Color(100, 100, 100));

    // Make Pause Menu
    sf::RectangleShape resumeButton(sf::Vector2f(200.f, 80.f));
    sf::RectangleShape quitButton(sf::Vector2f(200.f, 80.f));
    resumeButton.setOrigin(sf::Vector2f(-300.f, -200.f));
    quitButton.setOrigin(sf::Vector2f(-300.f, -290.f));
    resumeButton.setFillColor(sf::Color(100, 100, 100, 0));
    quitButton.setFillColor(sf::Color(100, 100, 100, 0));

    // Shooting
    const float bulletSpeed = 520.f;
    const float bulletRadius = 4.f;
    const float fireCooldown = 0.20f;
    float timeSinceLastShot = fireCooldown;

    // Lose Life Flash
    int flashTimer = 0;
    sf::RectangleShape loseLifeFlash(sf::Vector2f(800.f, 600.f));
    loseLifeFlash.setPosition(sf::Vector2f(0.f, 0.f));
    loseLifeFlash.setFillColor(sf::Color(255, 0, 0, 0));

    // Echolocation (soundwave)
    float echoAngleDeg = 0.f; // degrees
    const float echoThickness = 6.f;
    const float echoSpeed = 520.f;
    const float echoMaxCharge = 150.f; // Max LENGTH when fully charged
    const float echoChargeRate = 200.f; // how fast length increases per second when W is held
    const float echoShrinkRate = 100.f;  // How fast echo shrinks per second
    float echoCharge = 0.f; // Current charge amount
    bool wasWHeld = false; // track if W was held last frame
    // 360 Wave (Big Wave)
    bool wasEHeld = false; // track if E was held last frame
    float bigWaveCharge = 0.f; // Current big wave charge
    const float bigWaveMaxCharge = 200.f; // Max radius when fully charged
    const float bigWaveChargeRate = 250.f; // how fast radius increases per second when E is held
    const float bigWaveShrinkRate = 150.f; // How fast big wave shrinks per second

    bool wasEscapeHeld = false; // track if Escape was held last frame for pause menu

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Echo> echos;
    std::vector<BigEcho> BigEchos;
    // vector for all echos: 
    // std::vector<std::variant<Echo, BigEcho>> allEchos; // std variant is needed to hold both types, as it dynamically tracks which type is stored
    std::vector<std::unique_ptr<EchoBase>> allEchos; //polymorphic approach


    int wave = 1;
    int lives = 10;
    int hearts = lives;
    bool waveActive = false;
    float nextWaveTimer = 0.f;
    const float timeBetweenWaves = 1.0f;
    bool isPaused = false;

    // Make Hearts
    sf::Texture heartTexture("assets/heart.png");
    sf::Sprite heartSprite(heartTexture);
    heartSprite.setTextureRect(sf::IntRect({0, 0}, {8000 * lives, 7000}));
    heartSprite.setPosition(sf::Vector2f(10.f, 10.f));
    heartSprite.setScale(sf::Vector2f(0.05f, 0.05f));
    heartSprite.scale(sf::Vector2f(0.04f, 0.04f));
    heartTexture.setRepeated(true);
    heartTexture.setSmooth(true);
    window.draw(heartSprite);

    // Helper to spawn a wave (spawn count increases each wave)
    float total_intensity = 0.f;
    auto spawnWave = [&](int waveNumber) {
    int count = 1 + total_intensity/40; // might not be dividing by the right number *****
    if (count > 5) count = 5; // cap max enemies to 5 for now, to make the game easier

        
        // int count = 1 + waveNumber * 2;
        enemies.clear();
        // Reset echos when a new wave spawns, rather than when their radius/length reaches zero
        // Which is a dumb solution but it works
        echos.clear();
        BigEchos.clear();
        allEchos.clear();
        enemies.reserve(count); // we reserve enough to store more enemies and avoid reallocations
        float spawnRadius = std::max(WINDOW_W, WINDOW_H) / 2.f + 50.f;
        for (int i = 0; i < count; ++i) {
            float a = angleDist(rng); // random angle
            sf::Vector2f pos = CENTER + sf::Vector2f(std::cos(a), std::sin(a)) * spawnRadius;
            Enemy e; //make an enemy
            e.shape = sf::CircleShape(14.f); // give it a circle shape 
            e.shape.setOrigin(sf::Vector2f(e.shape.getRadius(), e.shape.getRadius())); // by default origin is top-left, we are centering it
            e.shape.setPosition(pos); // move he ORIGIN we made in the line above
             // ... velocity setup ...
            e.set_visibility(false);         // ensure default
            e.visibilityTimer = 0.f;    // ensure default
            // velocity towards center
            sf::Vector2f dir = CENTER - pos; //vector pointing from the enemy position to the center. Center - enemy position
            float len = std::sqrt(dir.x*dir.x + dir.y*dir.y); // find distance
            if (len != 0) dir /= len; //prevent division by zero when enemy reaches center
            // speed increases with wave number + some random variation
            float speed = 40.f + 8.f * waveNumber + (std::uniform_real_distribution<float>(-10.f, 10.f)(rng));
            e.set_velocity(dir * speed); //this makes the enemy actually move
            e.shape.setFillColor(sf::Color(0, 0, 0, 0));
            enemies.push_back(e); // adds the enemy
        }
        waveActive = true;
    };

    // Start first wave immediately
    spawnWave(wave);

    sf::Clock clock;
    while (window.isOpen()) { //actual game loop, runs until window is closed
        // dt is the time since the last frame
        float dt = clock.restart().asSeconds(); // we use time so the movement speed is not dependent on framerate
        timeSinceLastShot += dt; //control shooting cooldown

        while (auto evOpt = window.pollEvent()) { //checks if something happens in the window (like closing it)
            const auto &ev = *evOpt;
            // use the is<T>() helper in SFML 3 to check event type
            if (ev.is<sf::Event::Closed>()) {
                window.close(); //if the window is closed, we close it. woah.
            }
        }
        
        if(!isPaused) {
            // initially no rotation this frame
            float rotationThisFrame = 0.f;
            // SFML 3 key enum is nested under sf::Keyboard::Key
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                rotationThisFrame -= rotationSpeedDegPerSec * dt;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                rotationThisFrame += rotationSpeedDegPerSec * dt;
            }
            turretAngleDeg += rotationThisFrame;
            // stick whithin 0-360 range
            if (turretAngleDeg > 360.f) turretAngleDeg -= 360.f;
            if (turretAngleDeg < 0.f) turretAngleDeg += 360.f;

            // Shooting: spacebar
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && timeSinceLastShot >= fireCooldown) {
                timeSinceLastShot = 0.f;
                // create bullet
                Bullet b;
                b.shape = sf::CircleShape(bulletRadius);
                b.shape.setOrigin(sf::Vector2f(bulletRadius, bulletRadius)); // this centers the circle shape
                b.shape.setPosition(CENTER); //shooting from center of turret
                // compute direction from turretAngleDeg
                float rad = turretAngleDeg * 3.14159265f / 180.f; // degrees to radians
                sf::Vector2f dir(std::cos(rad), std::sin(rad)); // direction vector
                b.velocity = dir * bulletSpeed; // velocity (makes the bullet move)
                b.shape.setFillColor(sf::Color::Yellow);
                bullets.push_back(b); // add to bullets list
            }

            // Echolocation: Up Arrow key (charge and release)
            bool isWHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
            if (isWHeld) {
                // Charge the echo
                if (echoCharge <= echoMaxCharge) {
                    echoCharge = echoCharge + dt * echoChargeRate;
                    chargeBar.setSize(sf::Vector2f(barWidth, -(echoCharge)));
                } else {
                    echoCharge = echoMaxCharge;
                    chargeBar.setSize(sf::Vector2f(barWidth, -(echoCharge)));
                }
            } else if (wasWHeld && echoCharge > 0.f) {
                // if W was released - spawn the echo with the accumulated charge
                chargeBar.setSize(sf::Vector2f(barWidth, 0.f));
                Echo ec;
                ec.length = echoCharge * 2.f;
                total_intensity += ec.length/4;
                ec.elapsedTime = 0.f; // just spawned
                float rad = turretAngleDeg * 3.14159265f / 180.f; // line must be perpendicular to turret direction
                ec.velocity = sf::Vector2f(std::cos(rad), std::sin(rad));
                // Create rectangle perpendicular to direction (width = thickness, length = charge)
                ec.shape = sf::RectangleShape(sf::Vector2f(ec.length, echoThickness));
                ec.shape.setOrigin(sf::Vector2f(ec.length / 2.f, echoThickness / 2.f)); // center origin so it shrinks from both sides
                ec.shape.setPosition(CENTER);
                ec.shape.setRotation(sf::degrees(turretAngleDeg + 90.f)); // perpendicular to turret direction
                ec.shape.setFillColor(sf::Color::Cyan);
                echos.push_back(ec);
                allEchos.push_back(std::make_unique<Echo>(ec));
                echoCharge = 0.f;  // Reset charge
            }
            wasWHeld = isWHeld;

            // Big Wave: E key (charge and release)
            bool isEHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
            if (isEHeld) {
                // Charge the big wave
                if (bigWaveCharge <= bigWaveMaxCharge) {
                    bigWaveCharge = bigWaveCharge + dt * bigWaveChargeRate;
                    bigWaveChargeBar.setSize(sf::Vector2f(barWidth, -(bigWaveCharge)));
                } else {
                    bigWaveCharge = bigWaveMaxCharge;
                    bigWaveChargeBar.setSize(sf::Vector2f(barWidth, -(bigWaveCharge)));
                }
            } else if (wasEHeld && bigWaveCharge > 0.f) {
                // if E was released - spawn the big wave with the accumulated charge
                bigWaveChargeBar.setSize(sf::Vector2f(barWidth, 0.f));
                BigEcho bec;
                bec.radius = bigWaveCharge * 2.f;
                total_intensity += bec.radius; // big wave intensity is much higher
                bec.elapsedTime = 0.f; // just spawned
                // Create circle that expands from center
                bec.shape = sf::CircleShape(bec.radius);
                bec.shape.setOrigin(sf::Vector2f(bec.radius, bec.radius)); // center origin
                bec.shape.setPosition(CENTER);
                bec.shape.setFillColor(sf::Color::Transparent);
                bec.shape.setOutlineThickness(3.f);
                bec.shape.setOutlineColor(sf::Color::Magenta);
                BigEchos.push_back(bec);
                allEchos.push_back(std::make_unique<BigEcho>(bec));
                bigWaveCharge = 0.f;  // Reset charge
            }
            wasEHeld = isEHeld;

            // Update echos (shrinking rectangles that fly outward)
            for (size_t i = 0; i < echos.size(); ) {
                echos[i].update(dt, echoShrinkRate, echoSpeed, echoThickness, CENTER);
                // Remove when length is 0
                if (echos[i].length <= 0.f) {
                    echos.erase(echos.begin() + i);
                } else {
                    ++i;
                }
            }

            // Update big waves (expanding circles)
            for (size_t i = 0; i < BigEchos.size(); ) {
                BigEchos[i].update(dt, bigWaveShrinkRate, 0.f, 0.f, CENTER);
                // Remove when radius is 0
                if (BigEchos[i].radius <= 0.f) {
                    BigEchos.erase(BigEchos.begin() + i);
                } else {
                    ++i;
                }
            }

            if (!lives){
                return 0;
            }

            // For each enemy, check against each echo using the echo rectangle's inverse transform.
            // Transform the enemy center into the echo's local space (where the rectangle is axis-aligned and centered),
            // clamp to the rectangle extents to find the closest point, then test circle-vs-point distance.
            // On hit: set enemy visible and start a 4.0s timer (do NOT erase the enemy).
            for (auto& enemy : enemies) {
                for (auto& echo : allEchos) {
                    if (echo->hitsEnemy(enemy, echoThickness)) {
                        enemy.set_visibility(true);
                        enemy.visibilityTimer = 4.f;
                        break;
                    }
                }
            }


            // Per-frame: update enemy visibility timers
            for (size_t i = 0; i < enemies.size(); ++i) {
                if (enemies[i].visibilityTimer > 0.f) {
                    enemies[i].visibilityTimer -= dt;
                    if (enemies[i].visibilityTimer <= 0.f) {
                        enemies[i].visibilityTimer = 0.f;
                        enemies[i].set_visibility(false);
                    }
                }
            }

            // Update bullets
            for (size_t i = 0; i < bullets.size(); ) {
                Bullet &b = bullets[i]; //take current bullet
                b.shape.move(b.velocity * dt); //move it according to its velocity
                sf::Vector2f p = b.shape.getPosition(); // current position
                // remove bullet if outside window bounds (with margin)
                if (p.x < -50 || p.x > WINDOW_W + 50 || p.y < -50 || p.y > WINDOW_H + 50) { //erase if out of bounds
                    bullets.erase(bullets.begin() + i);
                } else ++i;
            }

            // Keep updating all enemies
            for (size_t i = 0; i < enemies.size(); ++i) {
                enemies[i].shape.move(enemies[i].get_velocity() * dt);
            }

            // Collision detection: bullets vs enemies
            for (size_t bi = 0; bi < bullets.size(); ) {
                bool bulletRemoved = false;
                sf::Vector2f bp = bullets[bi].shape.getPosition(); // bullet position
                for (size_t ei = 0; ei < enemies.size(); ++ei) { // check against all enemies
                    sf::Vector2f ep = enemies[ei].shape.getPosition(); // enemy position
                    float dx = bp.x - ep.x; // difference in x
                    float dy = bp.y - ep.y; // difference in y
                    float dist2 = dx*dx + dy*dy;
                    float rsum = bullets[bi].shape.getRadius() + enemies[ei].shape.getRadius(); // if distance squared is less than radius sum squared, we have a collision
                    if (dist2 <= rsum * rsum) {
                        // hit: remove both bullet and enemy (one-shot kill)
                        enemies.erase(enemies.begin() + ei);
                        bulletRemoved = false;
                        break;
                    }
                }
                if (!bulletRemoved) ++bi;
            }

            // Check if any enemy reached the center -> remove them (as if they hit the turret)
            for (size_t i = 0; i < enemies.size(); ) {
                sf::Vector2f ep = enemies[i].shape.getPosition(); // enemy position
                float dx = ep.x - CENTER.x;
                float dy = ep.y - CENTER.y;
                float dist2 = dx*dx + dy*dy; // distance squared to center
                if (dist2 <= (turretRadius + enemies[i].shape.getRadius()) * (turretRadius + enemies[i].shape.getRadius())) {
                    // enemy reached turret: remove it
                    enemies.erase(enemies.begin() + i);
                    flashTimer = 15;
                    lives--;
                    hearts--;
                    if (hearts == 0) {
                        hearts++;
                        heartSprite.setTextureRect(sf::IntRect({0, 0}, {0, 7000}));
                        return 0;
                    } else {
                        heartSprite.setTextureRect(sf::IntRect({0, 0}, {hearts * 8000, 7000}));
                    }
                } else ++i;
            }

            // sets up death flash
            flashTimer--;
            if (flashTimer > 0){
                loseLifeFlash.setFillColor(sf::Color(255, 0, 0, 100));
            } else {
                loseLifeFlash.setFillColor(sf::Color(255, 0, 0, 0));
                flashTimer = 0;
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
        }

        // Pause menu toggle (moved this as well for cleanliness)
        bool isEscapeHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
        if (isEscapeHeld && !wasEscapeHeld) {
            isPaused = !isPaused; // toggle pause on/off
            if (isPaused) {
                resumeButton.setFillColor(sf::Color(100, 100, 100, 255));
                quitButton.setFillColor(sf::Color(100, 100, 100, 255));
            } else {
                resumeButton.setFillColor(sf::Color(100, 100, 100, 0));
                quitButton.setFillColor(sf::Color(100, 100, 100, 0));
            }
        }
        wasEscapeHeld = isEscapeHeld;

        // Pause menu button clicks (outside isPaused check so it works while paused)
        if (isPaused && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            std::cout << "mouse clicked!" << std::endl;
            sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
            if(resumeButton.getGlobalBounds().contains(mousePosition)) {
                std::cout << "resuming game!" << std::endl;
                isPaused = false;
                resumeButton.setFillColor(sf::Color(100, 100, 100, 0));
                quitButton.setFillColor(sf::Color(100, 100, 100, 0));
            } else if (quitButton.getGlobalBounds().contains(mousePosition)) {
                return 0;
            }
        }

        // Drawing
        window.clear(sf::Color(30, 30, 30));

        // Draw enemies
        for (auto &e : enemies) window.draw(e.shape);

        // Draw bullets
        for (auto &b : bullets) window.draw(b.shape);
        
        // Draw echos
        for (auto &ec : echos) {
            window.draw(ec.shape);
        }

        // Draw big waves
        for (auto &bec : BigEchos) {
            window.draw(bec.shape);
        }
        
        if(!isPaused){
            total_intensity -= 1.f; // might not be subtracting by the right number *****
            if (total_intensity < 0.f) total_intensity = 0.f;
        }

        // Draw hearts
        window.draw(heartSprite);

        // Draw Charge Bar
        window.draw(chargeBarBackground);
        window.draw(chargeBar);

        // Draw Big Wave Charge Bar
        window.draw(bigWaveChargeBarBackground);
        window.draw(bigWaveChargeBar);

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

        // Draw loseLifeFlash
        window.draw(loseLifeFlash);

        // Draw pause menu
        window.draw(resumeButton);
        window.draw(quitButton);
        
        // UI text
        if (fontLoaded) {
            uiText.setString("Wave: " + std::to_string(wave) +
                             "    Enemies: " + std::to_string((int)enemies.size()) +
                             "    Bullets: " + std::to_string((int)bullets.size()) +
                             "    Intensity: " + std::to_string((int)total_intensity) +
                             "\nControls: Left/Right to rotate, Space to fire, Esc to pause,"
                            "\n Up to charge echo, E to charge big echo");
            window.draw(uiText);
            if (isPaused) {
                window.draw(pauseText);
                window.draw(resumeText);
                window.draw(quitText);
            }
        }

        window.display();
    }
    return 0;
}
#endif