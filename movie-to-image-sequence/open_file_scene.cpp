#include "open_file_scene.h"

namespace movie_to_image_sequence {

static String TrimWithTilde(const String& text, size_t maxLength = 55) {
  if (text.size() <= maxLength) {
    return text;
  }

  // "~" の分だけ 1 文字引く
  return U"~" + text.substr(text.size() - (maxLength - 1));
}

void OpenFileScene::Main() {
  const int32 button_size_w{200};
  const int32 button_start_x{900};

  // ファイルを開くボタン.
  if (SimpleGUI::ButtonAt(U"Open File", Vec2{button_start_x, 75},
                          button_size_w)) {
    if (movie_file_path_ = Dialog::OpenFile({FileFilter::AllVideoFiles()})) {
      video_ = VideoTexture{*movie_file_path_};
      video_reader_ = VideoReader{*movie_file_path_};
    }
  }

  // 開いたファイルを消すボタン.
  if (SimpleGUI::ButtonAt(
          U"Reset File", Vec2{button_start_x + button_size_w * 1.2, 75},
          button_size_w, video_.has_value() && !video_->isEmpty())) {
    movie_file_path_.reset();
    video_.reset();
    video_reader_.reset();
  }

  if (SimpleGUI::ButtonAt(U"Convert!", Vec2{1000, 600}, button_size_w,
                          video_.has_value() && !video_->isEmpty())) {
  }

  // 変換後の画像を変更するバー.
  UpdateBar();

  // 画面左に動画プレビュー表示.
  DrawPreview();

  // プレビューの下にファイル情報を表示.
  FontAsset(U"Font")(U"File").draw(20, 55, 470, Palette::Black);
  FontAsset(U"Font")(U"Size").draw(20, 55, 500, Palette::Black);
  FontAsset(U"Font")(U"Frame").draw(20, 55, 530, Palette::Black);

  if (!movie_file_path_.has_value()) {
    FontAsset(U"Font")(U"動画ファイルを開いてください．")
        .draw(20, 130, 470, Palette::Red);
  } else if (video_.has_value() && video_->isEmpty()) {
    FontAsset(U"Font")(U"動画ファイルを開くのに失敗しました．")
        .draw(20, 130, 470, Palette::Red);
  } else {
    FontAsset(U"Font")(TrimWithTilde(*movie_file_path_))
        .draw(20, 130, 470, Palette::Black);
    FontAsset(U"Font")(U"{} x {}"_fmt(video_->width(), video_->height()))
        .draw(20, 130, 500, Palette::Black);
    FontAsset(U"Font")(U"{}"_fmt(video_reader_->getFrameCount()))
        .draw(20, 130, 530, Palette::Black);
  }
}

void OpenFileScene::UpdateBar() {
  const double start_x{800.0};

  // スライダーでmarginの値を変更.
  const double margin_max =
      video_.has_value() ? (std::min)(video_->width(), video_->height()) / 10.0
                         : 100.0;
  SimpleGUI::Slider(U"Margin : {}"_fmt(static_cast<int32>(margin_)), margin_, 0,
                    margin_max, Vec2{start_x, 125}, 160.0, 200);
  const double frame_step_max =
      video_reader_.has_value()
          ? (std::max)(1.0, video_reader_->getFrameCount() / 5.0)
          : 20;
  SimpleGUI::Slider(U"Frame Step : {}"_fmt(static_cast<int32>(frame_step_)),
                    frame_step_, 1, frame_step_max, Vec2{start_x, 175}, 160.0,
                    200);
  const int32 total_image_count =
      video_reader_.has_value()
          ? static_cast<int32>(video_reader_->getFrameCount() / frame_step_)
          : 1;
  FontAsset(U"Font")(U"Total Images : {}"_fmt(total_image_count))
      .draw(20, start_x + 20, 225, Palette::Black);

  SimpleGUI::Slider(U"Columns : {}"_fmt(static_cast<int32>(columns_)), columns_,
                    1, 20, Vec2{start_x, 275}, 160.0, 200);
  const int32 h_image_num =
      (total_image_count + static_cast<int32>(columns_) - 1) /
      static_cast<int32>(columns_);
  FontAsset(U"Font")(U"Rows : {}"_fmt(h_image_num))
      .draw(20, start_x + 20, 325, Palette::Black);

  SimpleGUI::Slider(U"Scale : {:.2f}x"_fmt(export_scale_), export_scale_, 0.01,
                    1.0, Vec2{start_x, 400}, 160.0, 200);
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
