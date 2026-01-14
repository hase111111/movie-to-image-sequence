#include <Siv3D.hpp>

#include "system_main.h"

void Main() {
  using movie_to_image_sequence::SystemMain;
  SystemMain system_main;
  if (!system_main.Initialize()) {
    return;
  }
  system_main.Main();
}
