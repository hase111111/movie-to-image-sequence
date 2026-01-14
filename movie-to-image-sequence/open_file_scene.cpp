#include "open_file_scene.h"

namespace movie_to_image_sequence {

static String TrimWithTilde(const String& text, size_t maxLength = 55) {
  if (text.size() <= maxLength) {
    return text;
  }

  // "~" の分だけ 1 文字引く
  return U"~" + text.substr(text.size() - (maxLength - 1));
}

Optional<ExportParameter> OpenFileScene::Main() {
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

  if (SimpleGUI::ButtonAt(
          U"Convert!", Vec2{button_start_x + button_size_w * 1.2, 680},
          button_size_w,
          video_.has_value() && !video_->isEmpty() && CanConvert())) {
    ExportParameter param;
    param.video_path = *movie_file_path_;
    param.frame_step = static_cast<int32>(frame_step_);
    param.tile_size = GetTileSize();
    param.columns = static_cast<int32>(columns_);
    param.margin = static_cast<int32>(margin_);
    return param;
  }

  if (!video_.has_value()) {
    FontAsset(U"Font")(U"動画を開かないとコンバートできません．")
        .draw(20, Vec2{800, 570}, Palette::Red);
  } else if (video_->isEmpty()) {
    FontAsset(U"Font")(U"動画の読み込みに失敗しました．")
        .draw(20, Vec2{800, 570}, Palette::Red);
  } else if (!CanConvert()) {
    FontAsset(U"Font")(
        U"出力される画像が大きすぎます．\n(一辺20000以下にしてください)")
        .draw(20, Vec2{800, 570}, Palette::Red);
  } else {
    FontAsset(U"Font")(U"コンバート可能です！")
        .draw(20, Vec2{800, 570}, Palette::Green);
  }

  // 変換後の画像を変更するバー.
  UpdateBar();

  // 画面左に動画プレビュー表示.
  DrawPreview();

  // 画面右下に出力画像の概要表示.
  DrawOutputOverview();

  // プレビューの下にファイル情報を表示.
  DrawMovieDetails();

  return none;
}

void OpenFileScene::UpdateBar() {
  const double start_x{800.0};
  const double bar_str_width{180.0};
  const double bar_width{200.0};

  // スライダー margin.
  SimpleGUI::Slider(U"Margin : {}"_fmt(static_cast<int32>(margin_)), margin_, 0,
                    GetMarginMax(), Vec2{start_x, 125}, bar_str_width,
                    bar_width);

  // スライダー frame step.
  const double frame_step_max =
      video_reader_.has_value()
          ? (std::max)(1.0, video_reader_->getFrameCount() / 5.0)
          : 20;
  SimpleGUI::Slider(U"Frame Step : {}"_fmt(static_cast<int32>(frame_step_)),
                    frame_step_, 1, frame_step_max, Vec2{start_x, 175},
                    bar_str_width, bar_width);
  if (SimpleGUI::ButtonAt(
          U"◁", Vec2{start_x + bar_str_width + bar_width + 25, 195}, 40)) {
    frame_step_ = (std::max)(1.0, frame_step_ - 1);
  }
  if (SimpleGUI::ButtonAt(
          U"▷", Vec2{start_x + bar_str_width + bar_width + 75, 195}, 40)) {
    frame_step_ = (std::min)(frame_step_ + 1, frame_step_max);
  }

  FontAsset(U"Font")(U"Total Image Num : {}"_fmt(GetTotalImageCount()))
      .draw(20, start_x + 20, 225, Palette::Black);

  // スライダー columns.
  SimpleGUI::Slider(U"Columns : {}"_fmt(static_cast<int32>(columns_)), columns_,
                    1, 20, Vec2{start_x, 275}, bar_str_width, bar_width);

  const int32 h_image_num =
      (GetTotalImageCount() + static_cast<int32>(columns_) - 1) /
      static_cast<int32>(columns_);
  FontAsset(U"Font")(U"Rows : {}"_fmt(h_image_num))
      .draw(20, start_x + 20, 325, Palette::Black);

  // スライダー export scale.
  SimpleGUI::Slider(U"Scale : {:.2f}x"_fmt(export_scale_), export_scale_, 0.01,
                    1.0, Vec2{start_x, 375}, bar_str_width, bar_width);
  FontAsset(U"Font")(
      U"Tile Size : {} x {}"_fmt(GetTileSize().x, GetTileSize().y))
      .draw(20, start_x + 20, 425, Palette::Black);
  const Size tile_size = GetTileSize();
  const int32 col = static_cast<int32>(columns_);
  const int32 row =
      (GetTotalImageCount() + col - 1) / col;  // 切り上げで行数を計算.
  const int32 output_width =
      col * tile_size.x + static_cast<int32>((col - 1) * margin_);
  const int32 output_height =
      row * tile_size.y + static_cast<int32>((row - 1) * margin_);
  FontAsset(U"Font")(U"Total Width : {} "_fmt(output_width))
      .draw(20, start_x + 20, 475,
            20000 < output_width ? Palette::Red : Palette::Black);
  FontAsset(U"Font")(U"Total Height : {} "_fmt(output_height))
      .draw(20, start_x + 20, 500,
            20000 < output_height ? Palette::Red : Palette::Black);
}

void OpenFileScene::DrawPreview() const {
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

void OpenFileScene::DrawOutputOverview() const {
  const Vec2 base_pos{50, 600};
  const Size image_size{100, 55};
  const RectF left_rect{base_pos, image_size};
  const RectF right_rect{
      base_pos.movedBy(20 + image_size.x + margin_ / GetMarginMax() * 30, 0.0),
      image_size};

  // 枠線付きの四角形を描画.
  left_rect.draw(Color{40});
  right_rect.draw(Color{40});
  FontAsset(U"Font")(U"Image").drawAt(20, left_rect.center(), Color{255});
  FontAsset(U"Font")(U"Image").drawAt(20, right_rect.center(), Color{255});

  // サイズを表す矢印を描画.
  const Line arrow{left_rect.rightCenter(), right_rect.leftCenter()};
  const Line disc_line{arrow.center().movedBy(0.0, 50.0), arrow.center()};

  arrow.drawDoubleHeadedArrow(5.0, {5.0, 5.0}, Color{0});
  disc_line.drawArrow(2.0, {5.0, 5.0}, Color{0});
  FontAsset(U"Font")(U"Margin {}"_fmt(static_cast<int32>(margin_)))
      .drawAt(15, arrow.center().movedBy(0.0, 65.0), Color{0});

  // 画像の分割数を表示.
  const RectF big_rect{base_pos + Vec2{300, -40}, Size{250, 150}};
  const Line col_line{big_rect.tl().movedBy(0, -10),
                      big_rect.tr().movedBy(0, -10)};
  const Line row_line{big_rect.tr().movedBy(10, 0),
                      big_rect.br().movedBy(10, 0)};
  const Size tile_size = GetTileSize();
  const int32 col = static_cast<int32>(columns_);
  const int32 row =
      (GetTotalImageCount() + col - 1) / col;  // 切り上げで行数を計算.
  const int32 output_width =
      col * tile_size.x + static_cast<int32>((col - 1) * margin_);
  const int32 output_height =
      row * tile_size.y + static_cast<int32>((row - 1) * margin_);
  big_rect.draw(Color{80});
  FontAsset(U"Font")(U"Output Image").drawAt(25, big_rect.center(), Color{255});

  col_line.drawDoubleHeadedArrow(2.0, {5.0, 5.0}, Color{0});
  row_line.drawDoubleHeadedArrow(2.0, {5.0, 5.0}, Color{0});
  FontAsset(U"Font")(U"col:{} ({}ppx)"_fmt(col, output_width))
      .drawAt(15, col_line.center().movedBy(0, -10), Color{0});
  FontAsset(U"Font")(U"row:{} ({}ppx)"_fmt(row, output_height))
      .draw(15, row_line.center().movedBy(10, 0), Color{0});
}

void OpenFileScene::DrawMovieDetails() const {
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

int32 OpenFileScene::GetTotalImageCount() const {
  return video_reader_.has_value()
             ? static_cast<int32>(video_reader_->getFrameCount() / frame_step_)
             : 1;
}

double OpenFileScene::GetMarginMax() const {
  return video_.has_value()
             ? (std::min)(video_->width(), video_->height()) / 10.0
             : 100.0;
  ;
}

Size OpenFileScene::GetTileSize() const {
  // export_scale_ を小数点第3位で四捨五入する.
  const double scale =
      std::round(export_scale_ * 100.0) / 100.0;  // 小数点第3位で四捨五入
  return video_.has_value() ? Size{static_cast<int32>(video_->width() * scale),
                                   static_cast<int32>(video_->height() * scale)}
                            : Size{100, 100};
}

bool OpenFileScene::CanConvert() const {
  // 画像サイズを取得.
  const Size tile_size = GetTileSize();
  const int32 col = static_cast<int32>(columns_);
  const int32 row =
      (GetTotalImageCount() + col - 1) / col;  // 切り上げで行数を計算.
  const int32 output_width =
      col * tile_size.x + static_cast<int32>((col - 1) * margin_);
  const int32 output_height =
      row * tile_size.y + static_cast<int32>((row - 1) * margin_);

  const int32 limit_size = 20000;

  if (output_width <= limit_size && output_height <= limit_size) {
    return true;
  }

  return false;
}

}  // namespace movie_to_image_sequence
