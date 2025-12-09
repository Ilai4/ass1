#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (const auto& track_data : library_tracks) {
        AudioTrack* new_track = nullptr;
        if (track_data.type == "MP3") {
            new_track = new MP3Track(
                track_data.title,
                track_data.artists,
                track_data.duration_seconds,
                track_data.bpm,
                track_data.extra_param1,
                track_data.extra_param2
            );
        } 
        else if (track_data.type == "WAV") {
            new_track = new WAVTrack(
                track_data.title,
                track_data.artists,
                track_data.duration_seconds,
                track_data.bpm,
                track_data.extra_param1,
                track_data.extra_param2
            );
        }
        library.push_back(new_track);
    }
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    playlist = Playlist(playlist_name);
    for (unsigned int i = 0; i < track_indices.size(); i++) {
        int index = track_indices[i] - 1;
        if (index < 0 || static_cast<size_t>(index) >= library.size()) {
            std::cout << "[WARNING] Invalid track index:  " << index + 1 << std::endl;
            continue;
        }
        AudioTrack* source_track = library[index];
        AudioTrack* cloned_track = (source_track->clone()).release();
        if (cloned_track == nullptr) {
            std::cerr << "[ERROR] Failed to clone track at index  " << index << std::endl;
            continue;
        }
        cloned_track->load();
        cloned_track->analyze_beatgrid();
        playlist.add_track(cloned_track);
        //std::cout << "Added '" << cloned_track->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
    }
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" 
              << playlist.get_track_count() << " tracks)" << std::endl;
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (unsigned int i = 0; i < tracks.size(); i++) {
        titles.push_back(tracks[i]->get_title());
    }
    return titles;
}
