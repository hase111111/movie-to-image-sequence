#include "open_file_scene.h"

namespace movie_to_image_sequence {

void OpenFileScene::Main() {
  const int32 button_size_w{200};
  const int32 button_start_x{900};

  // ファイルを開くボタン.
  if (SimpleGUI::ButtonAt(U"Open File", Vec2{button_start_x, 100},
                          button_size_w)) {
    if (movie_file_path_ = Dialog::OpenFile({FileFilter::AllVideoFiles()})) {
      video_ = VideoTexture{*movie_file_path_};
    }
  }

  // 開いたファイルを消すボタン.
  if (SimpleGUI::ButtonAt(
          U"Reset File", Vec2{button_start_x + button_size_w * 1.2, 100},
          button_size_w, video_.has_value() && !video_->isEmpty())) {
    movie_file_path_.reset();
    video_.reset();
  }

  if (SimpleGUI::ButtonAt(U"Convert!", Vec2{1000, 600}, button_size_w,
                          video_.has_value() && !video_->isEmpty())) {
  }

  // 画面左に動画プレビュー表示.
  DrawPreview();

  // プレビューの下にファイル情報を表示.
  FontAsset(U"Font")(U"File: ").drawAt(20, 75, 480, Palette::Black);
  FontAsset(U"Font")(U"Size: ").drawAt(20, 75, 510, Palette::Black);
  if (movie_file_path_.has_value()) {
  }
}

void OpenFileScene::DrawPreview() {
  const Rect perv_rect = Rect{50, 50, 720, 405}
                             .draw(Color{80, 180})
                             .drawFrame(10.0, Palette::Black);

  FontAsset(U"Font")(U"Preview").drawAt(48, perv_rect.center(), Palette::White);

  if (video_.has_value() && !video_->isEmpty()) {
    // Rectに収まるように描画.
    const double scale_x = static_cast<double>(perv_rect.w) / video_->width();
    const double scale_y = static_cast<double>(perv_rect.h) / video_->height();
    const double scale = std::min(scale_x, scale_y);
    video_->scaled(scale).drawAt(perv_rect.center());
  }
}

}  // namespace movie_to_image_sequence
