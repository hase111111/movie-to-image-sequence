#pragma once

#include <Siv3D.hpp>
#include <thread>

#include "parameter.h"

namespace movie_to_image_sequence {

class ImageExporter final {
 public:
  void Export(const ExportParameter& param);

  void Reset();
  Image GetData() const;
  int32 GetProgress() const;

 private:
  Image MakeLabelImage(const int32 size, const Font& font, const String& text);

  void SetData(const Image& image);
  void SetProgress(int32 percent);

  mutable std::mutex mutex_;
  Image data_{};
  int32 progress_percent_{0};
};

}  // namespace movie_to_image_sequence
