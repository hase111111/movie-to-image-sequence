#include "system_main.h"

#include <Siv3D.hpp>

#include "app_const.h"

namespace movie_to_image_sequence {

bool SystemMain::Initialize() {
  // スクリーンサイズの設定.
  Window::Resize(app_const::kScreenWidth, app_const::kScreenHeight);

  // ウィンドウタイトルの設定.
  Window::SetTitle(U"{} ver {}"_fmt(app_const::AppName, app_const::AppVersion));

  return true;
}

void SystemMain::Main() {
  while (System::Update()) {
  }
}

}  // namespace movie_to_image_sequence
