#pragma once

#include "Component.h"
#include <string>
#include <glm/glm.hpp>


class ReverbZone : public Component {
public:
    enum class Shape {
        SPHERE = 0,
        BOX = 1
    };

    enum class Preset {
        BRIGHT_CHAMBER,
        DARK_CHAMBER,
        CONCRETE_VENUE_1,
        CONCRETE_VENUE_2,
        NIGHT_CLUB,
        WAREHOUSE,
        SMALL_CHURCH,
        MEDIUM_CHURCH,
        LARGE_CHURCH,
        CATHEDRAL,
        SHORT_DARK_HALL,
        LONG_DARK_HALL,
        BRIGHT_HALL,
        SMALL_HALL,
        MEDIUM_HALL,
        LARGE_HALL,
        LECTURE_HALL,
        RECITAL_HALL,
        SMALL_PLATE,
        MEDIUM_PLATE,
        LARGE_PLATE,
        VOCAL_PLATE,
        SMALL_ROOM,
        LARGE_ROOM,
        BATHROOM,
        BEDROOM,
        TILED_ROOM,
        PHONE_BOOTH,
        POOL,
        GYM,
        GARAGE
        
    };

    ReverbZone(GameObject* owner);
    virtual ~ReverbZone();

    // point test in world space
    bool ContainsPoint(const glm::vec3& worldPoint) const;

    // Component overrides
    void Serialize(nlohmann::json& componentObj) const override;
    void Deserialize(const nlohmann::json& componentObj) override;
    void OnEditor() override;

public:
    Shape shape = Shape::SPHERE;
    Preset preset = Preset::CATHEDRAL;
    float radius = 1.0f;                // sphere radius (world units)
    glm::vec3 extents = glm::vec3(5.0f); // box half-extents (local)
    std::string auxBusName = "Reverb_AuxBus"; // Wwise Aux Bus name
    float wetLevel = 1.0f;              // 0.0 - 1.0
    int priority = 0;                   // higher priority wins when overlapping
    bool enabled = true;
};