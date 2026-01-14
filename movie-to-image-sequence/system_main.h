#pragma once

#include <Siv3D.hpp>
#include <memory>
#include <thread>

#include "parameter.h"

namespace movie_to_image_sequence {

class SystemMain final {
 public:
  bool Initialize();
  void Main();

 private:
  enum class CurrentScene : int32 {
    OpenFile,
    OutputInProgress,
  };

  CurrentScene current_scene_{CurrentScene::OpenFile};
  std::shared_ptr<std::thread> export_thread_;
};

}  // namespace movie_to_image_sequence
