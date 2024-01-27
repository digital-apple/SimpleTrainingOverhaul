#pragma once

class Serialization
{
public:
    enum : std::uint32_t
    {
        kModID = 'STOL',
        kVersion = 1,
        kCollection = 'COLL'
    };

    static auto GetSingleton() -> Serialization*;

    static void OnGameLoaded(SKSE::SerializationInterface* a_interface);
    static void OnGameSaved(SKSE::SerializationInterface*a_interface);
    static void OnRevert(SKSE::SerializationInterface*);

    void CreateGlobalVariables();
    auto GetCurrentActorValue() -> RE::ActorValue;
    auto GetGlobalValue(RE::ActorValue a_value) -> RE::TESGlobal*;
    void SetCurrentActorValue(RE::ActorValue a_value);
    void SetGlobalValue(RE::ActorValue a_value, std::uint32_t a_amount);
private:
    Serialization() = default;
    Serialization(const Serialization&) = delete;
    Serialization(Serialization&&) = delete;

    ~Serialization() = default;

    Serialization& operator=(const Serialization&) = delete;
    Serialization& operator=(Serialization&&) = delete;

    mutable std::mutex lock;
    std::unordered_map<RE::ActorValue, RE::TESGlobal*> collection;
    RE::ActorValue current;
};