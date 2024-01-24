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
                float amount;

                a_interface->ReadRecordData(&value, sizeof(value));
                a_interface->ReadRecordData(&amount, sizeof(amount));

                Serialization::GetSingleton()->collection[value] = amount;
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
        const auto amount = element.second;

        if (!a_interface->WriteRecordData(&value, sizeof(value))) {
            return;
        }

        if (!a_interface->WriteRecordData(&amount, sizeof(amount))) {
            return;
        }
    }
}

void Serialization::OnRevert(SKSE::SerializationInterface*)
{
    std::unique_lock lock(Serialization::GetSingleton()->lock);
    Serialization::GetSingleton()->collection.clear();
}

auto Serialization::GetActorValue(RE::ActorValue a_value) -> float
{
    return collection[a_value];
}

void Serialization::SetActorValue(RE::ActorValue a_value, std::uint32_t a_amount)
{
    collection[a_value] += static_cast<float>(a_amount);
}