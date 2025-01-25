#include <iostream>
#include <ratio>
#include <thread>
#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <csignal>
#include <atomic>

namespace fs = std::filesystem;

std::atomic<bool> running(true);

void signal_handler(int signal) {
  running = false;
}

class DirectoryWatcher {
  private:
    std::string path;
    std::chrono::duration<int, std::milli> delay;
    std::map<std::string, fs::file_time_type> files;

    void watch() {
      while (running) {
        auto it = files.begin();
        while (it != files.end()) {
          if (!fs::exists(it->first)) {
            std::cout << "File deleted: " << it->first << std::endl;
            it = files.erase(it);
          }
          else {
            ++it;
          }
        }

        for (const auto& file : fs::directory_iterator(path)) {
          auto current_file_last_write_time = fs::last_write_time(file);

          if (!files.contains(file.path().string())) {
            std::cout << "File created: " << file.path().string() << std::endl;
            files[file.path().string()] = current_file_last_write_time;
          }
          else {
            if (files[file.path().string()] != current_file_last_write_time) {
              std::cout << "File modified: " << file.path().string() << std::endl;
              files[file.path().string()] = current_file_last_write_time;
            }
          }
        }

        std::this_thread::sleep_for(delay);
      }
    }

  public:
    DirectoryWatcher(std::string& path, std::chrono::duration<int, std::milli> delay)
      : path(path), delay(delay)
    {
      for (const auto& file : fs::directory_iterator(path)) {
        files[file.path().string()] = fs::last_write_time(file);
      }
    }

    void start() {
      std::thread watcher_thread(&DirectoryWatcher::watch, this);
      watcher_thread.detach();
    }
};

int main(int argc, char* argv[]) {
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  if (argc != 2) {
    std::cerr << "Insufficent arguements.\nExpected: executable <path>" << std::endl;
  }
  else {
    std::string path_to_watch = std::string(argv[1]);
    std::chrono::duration<int, std::milli> delay(1000); // Check every 1 second

    DirectoryWatcher watcher(path_to_watch, delay);
    watcher.start();

    std::cout << "Directory watcher daemon started. Press Ctrl+C to exit." << std::endl;

    while (running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Directory watcher daemon stopped." << std::endl;
  }

  return 0;
}
