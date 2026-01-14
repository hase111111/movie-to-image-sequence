#include "image_exporter.h"

namespace movie_to_image_sequence {

void ImageExporter::Export(const ExportParameter& param) {
  VideoReader reader(param.video_path);

  if (!reader) {
    Print << U"[Error] Failed to open video";
    return;
  }

  const int32 totalFrames = static_cast<int32>(reader.getFrameCount());

  const int32 label_size = param.tile_size.y / 8;
  Font font{static_cast<int32>(label_size * 0.6), Typeface::Bold};
  Array<Image> frames;
  Image frame;

  for (int32 i{0}, index{1}; i < totalFrames; i += param.frame_step, ++index) {
    // frameStep-1フレーム分スキップ
    for (int32 j{0}; j < param.frame_step - 1; ++j) {
      if (!reader.readFrame(frame)) {
        break;
      }
    }

    if (!reader.readFrame(frame)) {
      break;
    }

    const auto scaleX = static_cast<double>(param.tile_size.x) / frame.width();
    const auto scaleY = static_cast<double>(param.tile_size.y) / frame.height();
    const auto scale = std::min(scaleX, scaleY);
    auto scaled_frame = frame.scaled(scale);

    // --- 番号Image生成 ---
    const String label = Format(index);
    Image labelImg = MakeLabelImage(label_size, font, label);

    // --- 右上に配置 ---
    const Point labelPos(param.tile_size.x - labelImg.width() - 6, 6);

    labelImg.overwrite(scaled_frame, labelPos);

    frames << scaled_frame;

    // 進捗更新
    const int32 progress =
        static_cast<int32>((static_cast<double>(i) / totalFrames) * 50.0);
    SetProgress(progress);
  }
  SetProgress(50);

  if (frames.isEmpty()) {
    Print << U"[Error] No frames extracted";
    return;
  }

  // ====== タイル合成 ======
  const int32 rows =
      (static_cast<int32>(frames.size()) + param.columns - 1) / param.columns;

  const int32 width =
      param.margin + param.columns * (param.tile_size.x + param.margin);
  const int32 height = param.margin + rows * (param.tile_size.y + param.margin);

  Image sheet(width, height, Color{0});  // 白背景

  for (int32 i{0}; i < frames.size(); ++i) {
    const int32 r = i / param.columns;
    const int32 c = i % param.columns;

    const Point pos(param.margin + c * (param.tile_size.x + param.margin),
                    param.margin + r * (param.tile_size.y + param.margin));

    frames[i].overwrite(sheet, pos);

    // 進捗更新
    const int32 progress =
        50 +
        static_cast<int32>((static_cast<double>(i) / frames.size()) * 50.0);
    SetProgress(progress);
  }

  SetData(sheet);
  SetProgress(101);
}

Image ImageExporter::MakeLabelImage(const int32 size, const Font& font,
                                    const String& text) {
  //  const RectF region = font(text).region();
  const RectF region{0, 0, size, size};
  Image img{region.size.asPoint(), Color{0, 0}};

  // fontをoverwriteで描画
  font(text).overwriteAt(img, region.center());

  return img.scaled(1.0);
}

void ImageExporter::SetData(const Image& image) {
  std::lock_guard<std::mutex> lock(mutex_);
  data_ = image;
}

Image ImageExporter::GetData() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return data_;
}

void ImageExporter::SetProgress(const int32 percent) {
  std::lock_guard<std::mutex> lock(mutex_);
  progress_percent_ = percent;
}

int32 ImageExporter::GetProgress() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return progress_percent_;
}

void ImageExporter::Reset() {
  std::lock_guard<std::mutex> lock(mutex_);
  data_ = Image{};
  progress_percent_ = 0;
}

}  // namespace movie_to_image_sequence
