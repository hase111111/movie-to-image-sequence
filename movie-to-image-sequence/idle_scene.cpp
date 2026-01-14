#include "idle_scene.h"

#include <Siv3D.hpp>

#include "app_const.h"

namespace movie_to_image_sequence {

void IdleScene::Main(const ImageExporter& exporter) {
  // 待ち時間中のぐるぐるを描画．
  const int32 circle_count = 12;
  const double radius = 120.0;
  const Vec2 center =
      Vec2{app_const::kScreenWidth, app_const::kScreenHeight} / 2;
  const double time = Scene::Time();  // 時間
  const double speed = 15.0;          // 回転速度
  for (int32 i = 0; i < circle_count; ++i) {
    const double angle = Math::TwoPi / circle_count * i;
    const double phase = i - time * speed;

    // 先頭が明るく、後ろが暗くなる
    const double alpha =
        -(Math::Fmod(phase + circle_count, circle_count) / circle_count);

    const Vec2 pos = center + Vec2{std::cos(angle), std::sin(angle)} * radius;

    Circle{pos, 20.0}.draw(ColorF{0.2, 0.6, 1.0, alpha});
  }

  // メッセージを描画．
  FontAsset(U"Font")(U"Processing...{} %"_fmt(exporter.GetProgress()))
      .drawAt(center.x, center.y - radius - 80.0, Palette::Black);
}

}  // namespace movie_to_image_sequence
