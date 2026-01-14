#include <Siv3D.hpp>

/// 文字列を Image として生成（透明背景）
Image MakeLabelImage(const Font& font, const String& text) {
  const RectF region = font(text).region();
  Image img(region.size.asPoint(), Color(0, 0));

  // fontをoverwriteで描画
  font(text).overwriteAt(img, region.center());

  return img;
}

void Main() {
  // ========= 設定 =========
  const FilePath videoPath = U"example.mp4";

  // 何フレームごとに抜くか（例: 5 → 5フレームに1枚）
  const int32 frameStep = 100;

  // タイルサイズ
  const Size tileSize(320, 180);

  // 1行あたりの枚数
  const int32 columns = 10;

  // 余白
  const int32 margin = 50;

  // 出力
  const FilePath outPath = U"frame_sequence.png";
  // ========================

  VideoReader reader(videoPath);

  if (!reader) {
    Print << U"[Error] Failed to open video";
    System::Sleep(2s);
    return;
  }

  const int32 totalFrames = reader.getFrameCount();
  Print << U"Frames: " << totalFrames;

  Font font(24, Typeface::Bold);
  Array<Image> frames;
  Image frame;

  for (int32 i{0}, index{1}; i < totalFrames; i += frameStep, ++index) {
    // frameStep-1フレーム分スキップ
    for (int32 j{0}; j < frameStep - 1; ++j) {
      if (!reader.readFrame(frame)) {
        break;
      }
    }

    if (!reader.readFrame(frame)) {
      break;
    }

    // frames << frame.resized(tileSize);
    const auto scaleX = static_cast<double>(tileSize.x) / frame.width();
    const auto scaleY = static_cast<double>(tileSize.y) / frame.height();
    const auto scale = std::min(scaleX, scaleY);
    auto scaled_frame = frame.scaled(scale);

    // --- 番号Image生成 ---
    const String label = Format(index);
    Image labelImg = MakeLabelImage(font, label);

    // --- 右上に配置 ---
    const Point labelPos(tileSize.x - labelImg.width() - 6, 6);

    labelImg.overwrite(scaled_frame, labelPos);

    frames << scaled_frame;
  }

  if (frames.isEmpty()) {
    Print << U"[Error] No frames extracted";
    System::Sleep(2s);
    return;
  }

  // ====== タイル合成 ======
  const int32 rows = (frames.size() + columns - 1) / columns;

  const int32 width = margin + columns * (tileSize.x + margin);
  const int32 height = margin + rows * (tileSize.y + margin);

  Image sheet(width, height, Color(255));  // 白背景

  for (int32 i = 0; i < frames.size(); ++i) {
    const int32 r = i / columns;
    const int32 c = i % columns;

    const Point pos(margin + c * (tileSize.x + margin),
                    margin + r * (tileSize.y + margin));

    frames[i].overwrite(sheet, pos);
  }

  sheet.save(outPath);
  Print << U"Saved: " << outPath;

  // 確認表示
  Texture tex(sheet);
  while (System::Update()) {
    tex.draw();
  }
}
