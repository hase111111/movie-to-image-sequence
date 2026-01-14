#include "system_main.h"

#include <Siv3D.hpp>

#include "app_const.h"
#include "idle_scene.h"
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
  IdleScene idle_scene;
  ImageExporter image_exporter;

  while (System::Update()) {
    switch (current_scene_) {
      case SystemMain::CurrentScene::OpenFile: {
        const auto res = open_file_scene.Main();
        if (res.has_value()) {
          // 出力シーンへ移行.
          export_thread_ = std::make_shared<std::thread>(
              [&image_exporter, res]() { image_exporter.Export(*res); });
          current_scene_ = SystemMain::CurrentScene::OutputInProgress;
        }
        break;
      }
      case SystemMain::CurrentScene::OutputInProgress: {
        idle_scene.Main(image_exporter);
        if (image_exporter.GetProgress() > 100) {
          export_thread_->join();
          export_thread_.reset();
          const auto img = image_exporter.GetData();
          // 現在時刻を取得してファイル名に使用.
          const DateTime now = DateTime::Now();
          const String file_name = U"image_{}_{}_{}_{}_{}_{}.png"_fmt(
              now.year, now.month, now.day, now.hour, now.minute, now.second);
          Print << img.save(file_name);
          image_exporter.Reset();
          current_scene_ = SystemMain::CurrentScene::OpenFile;
        }
        break;
      }
      default: {
        break;
      }
    }
  }
}

}  // namespace movie_to_image_sequence
