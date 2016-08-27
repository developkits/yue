// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/gfx/font.h"

#include "nativeui/gfx/platform_font.h"

namespace nu {

Font::Font() : platform_font_(PlatformFont::CreateDefault()) {
}

Font::Font(const std::string& font_name, int font_size)
    : platform_font_(PlatformFont::CreateFromNameAndSize(font_name,
                                                         font_size)) {
}

Font::~Font() {
}

std::string Font::GetFontName() const {
  return platform_font_->GetFontName();
}

int Font::GetFontSize() const {
  return platform_font_->GetFontSize();
}

#if defined(OS_WIN) || defined(OS_MACOSX) || defined(OS_IOS)
NativeFont Font::GetNativeFont() const {
  return platform_font_->GetNativeFont();
}
#endif

}  // namespace nu
