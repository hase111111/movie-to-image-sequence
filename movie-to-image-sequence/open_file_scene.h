#pragma once

#include <Siv3D.hpp>

namespace movie_to_image_sequence {

class OpenFileScene final {
 public:
  void Main();

 private:
  void DrawPreview();

  Optional<FilePath> movie_file_path_;
  Optional<VideoTexture> video_;
};

}  // namespace movie_to_image_sequence
