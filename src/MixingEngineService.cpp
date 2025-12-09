#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    for (int i = 0; i < 2; i++) {
        if (decks[i] != nullptr) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> new_track = track.clone();
    if (!new_track) {
        std::cerr << "[ERROR] Track:  \"" << track.get_title() 
                  << "\" failed to clone" << std::endl;
        return -1;
    }
    int target_deck = 1;
    if (active_deck == 1) {
        target_deck = 0;
    }
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;
    if (decks[target_deck] != nullptr) {
        delete decks[target_deck];
        decks[target_deck] = nullptr;
    }
    new_track->load();
    new_track->analyze_beatgrid();
    if (decks[active_deck] != nullptr && auto_sync) {
        if (!can_mix_tracks(new_track)) {
            sync_bpm(new_track);
        }
    }
    std::string title = new_track->get_title();
    decks[target_deck] = new_track.release();
    std::cout << "[Load Complete] '" << title << "' is now loaded on deck " 
              << target_deck << std::endl;
    /*
              if (decks[active_deck] != nullptr) {
        std::cout << "[Unload] Unloading previous deck " << active_deck 
                  << " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
        */
    active_deck = target_deck;
    std::cout << "[Active Deck] Switched to deck " << active_deck << std::endl;
    return active_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] == nullptr || !track) {
        return false;
    }
    int active_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();
    int diff = active_bpm - new_bpm;
    if (diff < 0) {
        diff = -diff;
    }
    return diff <= bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] == nullptr || !track) {
        return;
    }
    int original_bpm = track->get_bpm();
    int active_bpm = decks[active_deck]->get_bpm();
    int average_bpm = (active_bpm + original_bpm) / 2;
    track->set_bpm(average_bpm); 
    std::cout << "[Sync BPM] Syncing BPM from " << original_bpm 
              << " to " << average_bpm << std::endl;
}
