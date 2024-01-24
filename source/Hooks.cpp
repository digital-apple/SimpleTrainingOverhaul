#include "Hooks.h"
#include "Serialization.h"

namespace Addresses
{
    float GetActorValue([[maybe_unused]]RE::ActorValueOwner* a_owner, RE::ActorValue a_value)
    {
        return Serialization::GetSingleton()->GetActorValue(a_value);
    }

    namespace TrainingMenu::Train::GetActorValue
    {
        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(51793, 52667), REL::Relocate(0x60, 0x67) };

            struct Patch : Xbyak::CodeGenerator
            {
                Patch(std::uintptr_t a_function, std::uintptr_t a_address)
                {
                    Xbyak::Label functionLabel;
                    Xbyak::Label returnLabel;

                    if SKYRIM_REL_CONSTEXPR (REL::Module::IsAE()) {
                        mov(edx, ptr[rbx + 0x40]);
                    }
                
                    call(ptr[rip + functionLabel]);
                    jmp(ptr[rip + returnLabel]);

                    L(functionLabel);
                    dq(a_function);

                    L(returnLabel);
                    dq(a_address + 0x6);
                }
            };

            Patch patch(reinterpret_cast<uintptr_t>(Addresses::GetActorValue), target.address());
            patch.ready();

            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(45);
            trampoline.write_branch<5>(target.address(), trampoline.allocate(patch));

            REL::safe_fill(target.address() + 0x5, REL::NOP, 0x1);
        }
    }

    namespace TrainingMenu::Train::IncreaseActorValue
    {
        struct IncreaseActorValue
        {
            static void thunk(RE::Actor* a_actor, RE::ActorValue a_value, std::uint32_t a_amount)
            {
                if (a_actor) {
                    Serialization::GetSingleton()->SetActorValue(a_value, a_amount);
                    a_actor->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kPermanent, a_value, static_cast<float>(a_amount));
                }
            }
            static inline REL::Relocation<decltype(thunk)> function;
        };

        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(51793, 52667), REL::Relocate(0xCE, 0xD8) };
            stl::write_thunk_call<IncreaseActorValue>(target.address());
        }
    }

    namespace TrainingMenu::UpdateText::FormatText
    {
        struct FormatText
        {
            static void thunk(const char* a_buffer, std::size_t a_size, const char* a_format, const char* a_first, const char* a_second, ...)
            {
                const auto player = RE::PlayerCharacter::GetSingleton();

                if (player) {
                    const auto list = RE::ActorValueList::GetSingleton();

                    if (list) {
                        const auto value = list->LookupActorValueByName(a_first);
                        a_format = "%s %.0f (+%.0f)";

                        return function(a_buffer, a_size, a_format, a_first, a_second, Serialization::GetSingleton()->GetActorValue(value));
                    }
                }
                return function(a_buffer, a_size, a_format, a_first, a_second);
            }
            static inline REL::Relocation<decltype(thunk)> function;
        };

        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(51794, 52668), 0x9B };
            stl::write_thunk_call<FormatText>(target.address());
        }
    }

    namespace TrainingMenu::UpdateText::GetActorValue
    {
        void Hook()
        {
            REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(51794, 52668), 0x283 };

            struct Patch : Xbyak::CodeGenerator
            {
                Patch(std::uintptr_t a_function, std::uintptr_t a_address)
                {
                    Xbyak::Label functionLabel;
                    Xbyak::Label returnLabel;
                
                    mov(edx, ptr[rbx + 0x40]);
                    call(ptr[rip + functionLabel]);
                    jmp(ptr[rip + returnLabel]);

                    L(functionLabel);
                    dq(a_function);

                    L(returnLabel);
                    dq(a_address + 0x9);
                }
            };

            Patch patch(reinterpret_cast<uintptr_t>(Addresses::GetActorValue), target.address());
            patch.ready();

            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(45);
            trampoline.write_branch<5>(target.address(), trampoline.allocate(patch));

            REL::safe_fill(target.address() + 0x5, REL::NOP, 0x4);
        }
    }
    
    void Hook()
    {
        TrainingMenu::Train::GetActorValue::Hook();
        TrainingMenu::Train::IncreaseActorValue::Hook();
        TrainingMenu::UpdateText::FormatText::Hook();
        TrainingMenu::UpdateText::GetActorValue::Hook();
    }
}