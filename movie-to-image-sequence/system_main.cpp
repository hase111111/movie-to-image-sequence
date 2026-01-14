#include "system_main.h"

#include <Siv3D.hpp>

#include "app_const.h"
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

  while (System::Update()) {
    open_file_scene.Main();
  }
}

}  // namespace movie_to_image_sequence
