#include <chrono>
#include <cstdint>
#include <thread>
#include "memory.h"

namespace offsets
{
    // client
    constexpr uintptr_t localPlayer = 0xDEB964;
    constexpr uintptr_t entityList = 0x4E01024;
    constexpr uintptr_t glowObjectManager = 0x535BAD0;

    // lp
    constexpr uintptr_t TeamNum = 0xF4;
    constexpr uintptr_t GlowIndex = 0x10488;
}

struct RGBAColor
{
    constexpr RGBAColor(float r, float g, float b, float a = 1.f) noexcept : r(r), g(g), b(b), a(a) {}

    float r, g, b, a;
};

int main()
{
    auto csgo_memory = Memory("csgo.exe");

    const auto client = csgo_memory.GetModuleAddress("client.dll");
    const auto color = RGBAColor(1.f, 0.f, 0.f);

    while (true)
    {
        const auto localPlayer = csgo_memory.Read<uintptr_t>(client + offsets::localPlayer);
        const auto glowObjectManager = csgo_memory.Read<uintptr_t>(client + offsets::glowObjectManager);

        for (auto i = 0; i < 64; ++i)
        {
            const auto entity = csgo_memory.Read<uintptr_t>(client + offsets::entityList + i * 0x10);

            const auto entityTeamNum = csgo_memory.Read<int>(entity + offsets::TeamNum);
            const auto localTeamNum = csgo_memory.Read<int>(localPlayer + offsets::TeamNum);
            if (entityTeamNum == localTeamNum) {
                continue;
            }

            const auto glowIndex = csgo_memory.Read<int>(entity + offsets::GlowIndex);

            const auto glowAddress = glowObjectManager + (glowIndex * 0x38);
            const auto glowColorAddress = glowAddress + 0x8;

            csgo_memory.Write<RGBAColor>(glowColorAddress, color);
            csgo_memory.Write<bool>(glowAddress + 0x27, true);
            csgo_memory.Write<bool>(glowAddress + 0x28, true);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
