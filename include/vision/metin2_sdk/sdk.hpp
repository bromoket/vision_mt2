#pragma once

#include <vision/utils/math_utils.hpp>

#include <cstdint>
#include <string>

namespace vision::metin2_sdk {

// Forward declarations for common Metin2 structures
// These would be expanded based on actual game analysis

// Basic types commonly used in Metin2
using player_id_t = std::uint32_t;
using item_id_t = std::uint32_t;
using mob_id_t = std::uint32_t;

// Common game structures (minimal examples)
struct position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    position() = default;
    position(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}
    
    // Convert to math vector
    [[nodiscard]] math::vector3 to_vector3() const {
        return {x, y, z};
    }
    
    // Distance calculation
    [[nodiscard]] float distance_to(const position& other) const {
        return to_vector3().distance_to(other.to_vector3());
    }
};

struct player_info {
    player_id_t id = 0;
    std::string name;
    position pos;
    std::uint32_t level = 0;
    std::uint32_t health = 0;
    std::uint32_t max_health = 0;
    std::uint32_t mana = 0;
    std::uint32_t max_mana = 0;
};

struct item_info {
    item_id_t id = 0;
    std::string name;
    std::uint32_t count = 0;
    std::uint32_t slot = 0;
};

struct mob_info {
    mob_id_t id = 0;
    std::string name;
    position pos;
    std::uint32_t health = 0;
    std::uint32_t max_health = 0;
    std::uint32_t level = 0;
};

// Game state enums
enum class player_state : std::uint32_t {
    idle = 0,
    moving = 1,
    attacking = 2,
    dead = 3,
    trading = 4
};

enum class item_type : std::uint32_t {
    weapon = 0,
    armor = 1,
    accessory = 2,
    consumable = 3,
    material = 4,
    quest = 5
};

// Common game constants
namespace constants {
constexpr std::uint32_t max_inventory_slots = 45;
constexpr std::uint32_t max_equipment_slots = 12;
constexpr std::uint32_t max_skill_slots = 12;
constexpr float max_interaction_distance = 1000.0f;
}

// Memory offsets and signatures (examples - would be populated during analysis)
namespace offsets {
// These would be determined through reverse engineering
constexpr std::uintptr_t player_base = 0x0;  // TBD
constexpr std::uintptr_t inventory_base = 0x0;  // TBD
constexpr std::uintptr_t mob_list_base = 0x0;  // TBD
}

namespace signatures {
// Pattern signatures for finding game functions
constexpr const char* send_packet = "E8 ?? ?? ?? ?? 83 C4 08 5F";  // Example
constexpr const char* get_player_pos = "55 8B EC 83 EC ?? 56 8B F1";  // Example
constexpr const char* attack_function = "55 8B EC 51 53 56 8B F1";  // Example
}

}  // namespace vision::metin2_sdk
