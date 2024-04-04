#include "main.hpp"
#include "UnityEngine/ScriptableObject.hpp"
#include "GlobalNamespace/NoteCutHapticEffect.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "GlobalNamespace/SaberTypeExtensions.hpp"
#include "GlobalNamespace/HapticFeedbackManager.hpp"
#include "Libraries/HM/HMLib/VR/HapticPresetSO.hpp"
#include "GlobalNamespace/ArcAndObstacleHapticEffectManager.hpp"
#include "GlobalNamespace/SliderInteractionManager.hpp"
#include "GlobalNamespace/SliderController.hpp"
#include "GlobalNamespace/IVRPlatformHelper.hpp"
#include "GlobalNamespace/RumbleHapticFeedbackPlayer.hpp"
#include "UnityEngine/XR/XRNode.hpp"

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Configuration &getConfig() {
  static Configuration config(modInfo);
  return config;
}

MAKE_HOOK_MATCH(NoteCutHapticEffect_HitNote, &GlobalNamespace::NoteCutHapticEffect::HitNote,
    void, GlobalNamespace::NoteCutHapticEffect* self, GlobalNamespace::SaberType saberType, GlobalNamespace::NoteCutHapticEffect::Type type) {
    //if (type == GlobalNamespace::NoteCutHapticEffect::Type::ArcHead || type == GlobalNamespace::NoteCutHapticEffect::Type::ArcHeadAndTail || type == GlobalNamespace::NoteCutHapticEffect::Type::ArcTail)
    //  return; //WE HATE ARCS HERE
    //duration(0.13), strength(1), freq(0.5)
    auto hapticPresetSO = UnityEngine::ScriptableObject::CreateInstance<Libraries::HM::HMLib::VR::HapticPresetSO*>();
    hapticPresetSO->_duration = 0.13f;
    hapticPresetSO->_strength = 1.0f;
    hapticPresetSO->_frequency = 0.5f;
    hapticPresetSO->_continuous = false;
    hapticPresetSO->_useAdvancedHapticsOnSupportedPlatforms = false;
    auto node = GlobalNamespace::SaberTypeExtensions::Node(saberType);
    self->_hapticFeedbackManager->_rumbleHapticFeedbackPlayer->_vrPlatformHelper->StopHaptics(node);
    self->_hapticFeedbackManager->PlayHapticFeedback(node, hapticPresetSO);
}



MAKE_HOOK_MATCH(ArcAndObstacleHapticEffectManager_AddActiveSlider, &GlobalNamespace::SliderInteractionManager::AddActiveSlider,
    void, GlobalNamespace::SliderInteractionManager* self, GlobalNamespace::SliderController* newSliderController) {
    ArcAndObstacleHapticEffectManager_AddActiveSlider(self, newSliderController);
}

MOD_EXPORT void setup(CModInfo *info) noexcept {
    *info = modInfo.to_c();

    getConfig().Load();

    Paper::Logger::RegisterFileContextId(PaperLogger.tag);

    PaperLogger.info("Completed setup!");
}

MOD_EXPORT void late_load() noexcept {
    il2cpp_functions::Init();

    PaperLogger.info("Installing hooks...");

    INSTALL_HOOK(PaperLogger, NoteCutHapticEffect_HitNote);
    //INSTALL_HOOK(PaperLogger, ArcAndObstacleHapticEffectManager_AddActiveSlider);

    PaperLogger.info("Installed all hooks!");
}