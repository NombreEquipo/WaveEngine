#define GLM_ENABLE_EXPERIMENTAL
#include "ParticleSystem.h"
#include <glad/glad.h>
#include <algorithm>
#include <cstdlib>
#include <glm/gtx/vector_angle.hpp>

float RandomFloat(float min, float max) {
    if (max - min < 0.0001f) return min; // Avoid division by zero
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void ModuleEmitterSpawn::ResetDefaults() {
    // Reset to default paremeters
    lifetimeMin = 1.0f; lifetimeMax = 2.0f;
    speedMin = 2.0f; speedMax = 5.0f;
    shape = EmitterShape::BOX;
    emissionArea = glm::vec3(0.0f);
    emissionRadius = 1.0f;
    coneAngle = 45.0f;
    sizeStart = 0.5f; sizeEnd = 0.0f;
    colorStart = glm::vec4(1.0f);
    colorEnd = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    rotationSpeedMin = 0.0f; rotationSpeedMax = 0.0f;
}

void ModuleEmitterMovement::ResetDefaults() {
    gravity = glm::vec3(0.0f, -1.0f, 0.0f);
}

// Creation of the particle
void ModuleEmitterSpawn::Spawn(EmitterInstance* emitter, Particle* particle) {
    particle->active = true;
    float speed = RandomFloat(speedMin, speedMax);
    glm::vec3 dir = glm::vec3(0, 1, 0); // Default direction is up

    if (shape == EmitterShape::BOX) {
        // Random point inside a box
        glm::vec3 offset(
            RandomFloat(-emissionArea.x, emissionArea.x),
            RandomFloat(-emissionArea.y, emissionArea.y),
            RandomFloat(-emissionArea.z, emissionArea.z)
        );
        particle->position = spawnPosition + offset;
        // Direction up
        dir = glm::vec3(RandomFloat(-0.2f, 0.2f), 1.0f, RandomFloat(-0.2f, 0.2f));
    }
    else if (shape == EmitterShape::SPHERE) {
        // Direction on spehere is totatlly random because its made for explosions
        dir = glm::vec3(
            RandomFloat(-1.0f, 1.0f),
            RandomFloat(-1.0f, 1.0f),
            RandomFloat(-1.0f, 1.0f)
        );
        if (glm::length(dir) > 0.001f) dir = glm::normalize(dir);
        // Spawns at center
        particle->position = spawnPosition;
    }
    else if (shape == EmitterShape::CONE) {
        // Cone direction is up but always inside the cone limits
        float angle = glm::radians(RandomFloat(0.0f, coneAngle));
        float rotation = glm::radians(RandomFloat(0.0f, 360.0f));
        // Start Up
        dir = glm::vec3(0, 1, 0);
        // Pitch
        glm::vec3 rotatedDir = glm::rotate(dir, angle, glm::vec3(1, 0, 0));
        // Yaw around Y
        dir = glm::rotate(rotatedDir, rotation, glm::vec3(0, 1, 0));
        // Spawns at the top corner of the cone but is inverted so from down to up
        particle->position = spawnPosition;
    }

    particle->velocity = glm::normalize(dir) * speed;

    // Initialize Properties
    particle->lifetime = RandomFloat(lifetimeMin, lifetimeMax);
    particle->maxLifetime = particle->lifetime;

    // Start and End values
    particle->sizeStart = sizeStart;
    particle->sizeEnd = sizeEnd;
    particle->size = sizeStart;

    // Color
    particle->colorStart = colorStart;
    particle->colorEnd = colorEnd;
    particle->color = colorStart;

    // Spin
    particle->rotation = RandomFloat(0.0f, 360.0f);
    particle->angularVelocity = RandomFloat(rotationSpeedMin, rotationSpeedMax);

    // Animation
    particle->animationTime = 0.0f;
}

void ModuleEmitterMovement::Update(EmitterInstance* emitter, float dt) {
    for (auto& p : emitter->particles) {
        if (!p.active) continue;
        p.velocity += gravity * dt;
        p.position += p.velocity * dt;
    }
}

EmitterInstance::EmitterInstance() { particles.reserve(maxParticles); }

EmitterInstance::~EmitterInstance() {
    for (auto m : modules) delete m;
    modules.clear();
}

void EmitterInstance::Init() {
    if (modules.empty()) {
        modules.push_back(new ModuleEmitterSpawn());
        modules.push_back(new ModuleEmitterMovement());
    }
}

void EmitterInstance::Reset() {
    particles.clear();
    timeSinceLastEmit = 0.0f;
}

void EmitterInstance::ResetValues() {
    // Reset Global Settings
    maxParticles = 500;
    emissionRate = 20.0f;
    additiveBlending = false;
    textureRows = 1;
    textureCols = 1;
    animationSpeed = 1.0f;
    animLoop = false;
    // Reset Module Settings
    for (auto mod : modules) {
        mod->ResetDefaults();
    }
}

void EmitterInstance::Update(float dt) {
    if (!active) return;
    // Continious emission
    if (emissionRate > 0.0f) {
        timeSinceLastEmit += dt;
        float emitInterval = 1.0f / emissionRate;
        while (timeSinceLastEmit >= emitInterval) {
            if (particles.size() < maxParticles) {
                Particle p;
                for (auto mod : modules) mod->Spawn(this, &p);
                particles.push_back(p);
            }
            timeSinceLastEmit -= emitInterval;
        }
    }
    // Lifecycle and interpolation
    for (auto& p : particles) {
        if (!p.active) continue;

        p.lifetime -= dt;
        if (p.lifetime <= 0.0f) {
            p.active = false;
            continue;
        }

        float lifeRatio = 1.0f - (p.lifetime / p.maxLifetime);
        p.size = glm::mix(p.sizeStart, p.sizeEnd, lifeRatio);
        p.color = glm::mix(p.colorStart, p.colorEnd, lifeRatio);
        p.rotation += p.angularVelocity * dt;
        // Animation time
        p.animationTime = lifeRatio * animationSpeed;
    }

    for (auto mod : modules) mod->Update(this, dt);
    // Cleanup
    KillDeadParticles();
}

void EmitterInstance::KillDeadParticles() {
    // Remove all dead particles
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return !p.active; }), particles.end());
}

// Explosion effect
void EmitterInstance::Burst(int count) {
    if (!active) return;

    for (int i = 0; i < count; ++i) {
        if (particles.size() < maxParticles) {
            Particle p;
            // Force spawn
            for (auto mod : modules) mod->Spawn(this, &p);
            particles.push_back(p);
        }
    }
}

void EmitterInstance::Draw(glm::vec3 cameraPos) {
    if (particles.empty()) return;
    // Draw far to near
    for (auto& p : particles) p.distanceToCamera = glm::distance(p.position, cameraPos);
    std::sort(particles.begin(), particles.end(), [](const Particle& a, const Particle& b) {
        return a.distanceToCamera > b.distanceToCamera;
        });

    // Render state
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE); // Disable for transparent objects
    glDisable(GL_CULL_FACE);

    // Blending Mode
    if (additiveBlending) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Glow
    }
    else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard Alpha
    }

    // Texture Setup
    glActiveTexture(GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Multiply Texture * Color

    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    glBegin(GL_QUADS);

    // Animation frame sizes
    int totalFrames = textureRows * textureCols;
    float frameWidth = 1.0f / (float)textureCols;
    float frameHeight = 1.0f / (float)textureRows;

    for (const auto& p : particles) {
        // BILLBOARDING
        // Coordinate system aligned with the camera
        glm::vec3 direction = glm::normalize(cameraPos - p.position);
        glm::vec3 worldUp = (glm::abs(direction.y) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
        glm::vec3 baseRight = glm::normalize(glm::cross(worldUp, direction));
        glm::vec3 baseUp = glm::normalize(glm::cross(direction, baseRight));

        // Apply Particle Rotation
        float rad = glm::radians(p.rotation);
        float c = cos(rad);
        float s = sin(rad);

        // Rotate Vectors
        glm::vec3 right = baseRight * c + baseUp * s;
        glm::vec3 up = baseRight * -s + baseUp * c;

        float halfSize = p.size * 0.5f;

        // Vertex Positions
        glm::vec3 v1 = p.position - (right * halfSize) + (up * halfSize);
        glm::vec3 v2 = p.position + (right * halfSize) + (up * halfSize);
        glm::vec3 v3 = p.position + (right * halfSize) - (up * halfSize);
        glm::vec3 v4 = p.position - (right * halfSize) - (up * halfSize);

        // UV animation
        int currentFrame = (int)p.animationTime;
        if (animLoop) {
            currentFrame = (int)(p.animationTime * totalFrames) % totalFrames;
        }
        else {
            currentFrame = (int)(p.animationTime * totalFrames);
            if (currentFrame >= totalFrames) currentFrame = totalFrames - 1;
        }

        int col = currentFrame % textureCols;
        int row = currentFrame / textureCols;

        // Map UV coordinates based on grid position
        float uLeft = col * frameWidth;
        float uRight = uLeft + frameWidth;
        float vTop = 1.0f - (row * frameHeight);
        float vBottom = vTop - frameHeight;

        glColor4f(p.color.r, p.color.g, p.color.b, p.color.a);

        glTexCoord2f(uLeft, vTop);    glVertex3f(v1.x, v1.y, v1.z);
        glTexCoord2f(uRight, vTop);   glVertex3f(v2.x, v2.y, v2.z);
        glTexCoord2f(uRight, vBottom); glVertex3f(v3.x, v3.y, v3.z);
        glTexCoord2f(uLeft, vBottom);  glVertex3f(v4.x, v4.y, v4.z);
    }

    glEnd();

    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}