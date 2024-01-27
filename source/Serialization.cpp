#include "Serialization.h"

auto Serialization::GetSingleton() -> Serialization*
{
    static Serialization singleton;
    return std::addressof(singleton);
}

void Serialization::OnGameLoaded(SKSE::SerializationInterface* a_interface)
{
    std::uint32_t type;
    std::uint32_t version;
    std::uint32_t length;

    while (a_interface->GetNextRecordInfo(type, version, length)) {
        if (version != kVersion) {
            logs::critical("Serialization::OnGameLoaded :: Record data version mismatch! '{}' -> '{}'", version, static_cast<std::uint32_t>(kVersion));
            continue;
        }

        switch (type) {
        case kCollection:
            std::size_t count;
            a_interface->ReadRecordData(&count, sizeof(count));

            for (std::size_t i = 0; i < count; i++) {
                RE::ActorValue value;
                float global;

                a_interface->ReadRecordData(&value, sizeof(value));
                a_interface->ReadRecordData(&global, sizeof(global));

                Serialization::GetSingleton()->collection[value]->value = global;
            }
            break; 
        }
    }
}

void Serialization::OnGameSaved(SKSE::SerializationInterface* a_interface)
{
    if (!a_interface->OpenRecord(kCollection, kVersion)) {
        return;
    }

    std::unique_lock lock(Serialization::GetSingleton()->lock);

    const size_t size = Serialization::GetSingleton()->collection.size();

    if (!a_interface->WriteRecordData(&size, sizeof(size))) {
        return;
    }

    for (const auto& element : Serialization::GetSingleton()->collection) {
        const auto value = element.first;
        const auto global = element.second->value;

        if (!a_interface->WriteRecordData(&value, sizeof(value))) {
            return;
        }

        if (!a_interface->WriteRecordData(&global, sizeof(global))) {
            return;
        }
    }
}

void Serialization::OnRevert(SKSE::SerializationInterface*)
{
    std::unique_lock lock(Serialization::GetSingleton()->lock);

    for (const auto& value : Serialization::GetSingleton()->collection) {
        value.second->value = 0.0F;
    }
}

auto Serialization::GetGlobalValue(RE::ActorValue a_value) -> RE::TESGlobal*
{
    return collection[a_value];
}

void Serialization::SetGlobalValue(RE::ActorValue a_value, std::uint32_t a_amount)
{
    collection[a_value]->value += static_cast<float>(a_amount);
}

void Serialization::CreateGlobalVariables()
{
    const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::TESGlobal>();

    if (factory && collection.empty()) {
        collection.try_emplace(RE::ActorValue::kAlchemy, factory->Create());
        collection.try_emplace(RE::ActorValue::kAlteration, factory->Create());
        collection.try_emplace(RE::ActorValue::kArchery, factory->Create());
        collection.try_emplace(RE::ActorValue::kBlock, factory->Create());
        collection.try_emplace(RE::ActorValue::kConjuration, factory->Create());
        collection.try_emplace(RE::ActorValue::kDestruction, factory->Create());
        collection.try_emplace(RE::ActorValue::kEnchanting, factory->Create());
        collection.try_emplace(RE::ActorValue::kHeavyArmor, factory->Create());
        collection.try_emplace(RE::ActorValue::kIllusion, factory->Create());
        collection.try_emplace(RE::ActorValue::kLightArmor, factory->Create());
        collection.try_emplace(RE::ActorValue::kLockpicking, factory->Create());
        collection.try_emplace(RE::ActorValue::kOneHanded, factory->Create());
        collection.try_emplace(RE::ActorValue::kPickpocket, factory->Create());
        collection.try_emplace(RE::ActorValue::kRestoration, factory->Create());
        collection.try_emplace(RE::ActorValue::kSmithing, factory->Create());
        collection.try_emplace(RE::ActorValue::kSneak, factory->Create());
        collection.try_emplace(RE::ActorValue::kSpeech, factory->Create());
        collection.try_emplace(RE::ActorValue::kTwoHanded, factory->Create());
    }

    for (const auto& value : collection) {
        value.second->formFlags |= RE::TESForm::RecordFlags::kTemporary;
    }

    logs::info("Serialization::CreateGlobalVariables :: Collection size: '{}'", collection.size());
}

void Serialization::SetCurrentActorValue(RE::ActorValue a_value)
{
    current = a_value;
}

auto Serialization::GetCurrentActorValue() -> RE::ActorValue
{
    return current;
}