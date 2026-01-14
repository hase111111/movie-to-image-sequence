#pragma once

#include <Siv3D.hpp>

namespace movie_to_image_sequence {

struct ExportParameter final {
  FilePath video_path{};
  int32 frame_step{};
  Size tile_size{};
  int32 columns{};
  int32 margin{};
};

}  // namespace movie_to_image_sequence
