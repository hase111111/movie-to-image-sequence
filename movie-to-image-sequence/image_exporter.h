#pragma once

#include <Siv3D.hpp>

#include "parameter.h"

namespace movie_to_image_sequence {

class ImageExporter final {
 public:
  void Export(const ExportParameter& param);

 private:
  Image MakeLabelImage(const Font& font, const String& text);
};

}  // namespace movie_to_image_sequence
