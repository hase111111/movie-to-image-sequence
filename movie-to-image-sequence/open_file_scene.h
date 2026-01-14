#pragma once

#include <Siv3D.hpp>

namespace movie_to_image_sequence {

class OpenFileScene final {
 public:
  void Main();

 private:
  void UpdateBar();
  void DrawPreview();

  Optional<FilePath> movie_file_path_;
  Optional<VideoTexture> video_;
  Optional<VideoReader> video_reader_;

  double margin_{20};
  double columns_{1};
  double frame_step_{1};
};

}  // namespace movie_to_image_sequence
