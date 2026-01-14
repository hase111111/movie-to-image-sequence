#include "system_main.h"

#include <Siv3D.hpp>

#include "app_const.h"
#include "image_exporter.h"
#include "open_file_scene.h"

namespace movie_to_image_sequence {

bool SystemMain::Initialize() {
  // スクリーンサイズの設定.
  Window::Resize(app_const::kScreenWidth, app_const::kScreenHeight);

  // ウィンドウタイトルの設定.
  Window::SetTitle(U"{} ver {}"_fmt(app_const::AppName, app_const::AppVersion));

  // 背景色を白にする．
  Scene::SetBackground(Palette::Antiquewhite);

  // フォントのロード．
  FontAsset::Register(U"Font", 48, Typeface::Bold);

  return true;
}

void SystemMain::Main() {
  OpenFileScene open_file_scene;
  ImageExporter image_exporter;

  while (System::Update()) {
    switch (current_scene_) {
      case SystemMain::CurrentScene::OpenFile: {
        const auto res = open_file_scene.Main();
        if (res.has_value()) {
          // 出力シーンへ移行.
          image_exporter.Export(*res);
          current_scene_ = SystemMain::CurrentScene::OutputInProgress;
        }
        break;
      }
      case SystemMain::CurrentScene::OutputInProgress: {
        break;
      }
      default: {
        break;
      }
    }
  }
}

}  // namespace movie_to_image_sequence
