#pragma once

#include <Siv3D.hpp>

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
};

}  // namespace movie_to_image_sequence
