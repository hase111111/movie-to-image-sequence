#pragma once

#include <Siv3D.hpp>

#include "parameter.h"

namespace movie_to_image_sequence {

class OpenFileScene final {
 public:
  Optional<ExportParameter> Main();

 private:
  void UpdateBar();
  void DrawPreview() const;
  void DrawOutputOverview() const;
  void DrawMovieDetails() const;

  int32 GetTotalImageCount() const;
  double GetMarginMax() const;
  Size GetTileSize() const;
  bool CanConvert() const;

  Optional<FilePath> movie_file_path_;
  Optional<VideoTexture> video_;
  Optional<VideoReader> video_reader_;

  double margin_{20};
  double columns_{1};
  double frame_step_{1};
  double export_scale_{1.0};
};

}  // namespace movie_to_image_sequence
