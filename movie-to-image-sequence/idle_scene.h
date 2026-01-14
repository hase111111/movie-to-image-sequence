#pragma once

#include <Siv3D.hpp>

#include "image_exporter.h"

namespace movie_to_image_sequence {

class IdleScene final {
 public:
  void Main(const ImageExporter& exporter);

 private:
};

}  // namespace movie_to_image_sequence
