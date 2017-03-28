// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/group.h"

#include "base/strings/utf_string_conversions.h"
#include "nativeui/gfx/color.h"
#include "nativeui/gfx/geometry/insets.h"
#include "nativeui/gfx/geometry/size_conversions.h"
#include "nativeui/gfx/geometry/vector2d.h"
#include "nativeui/gfx/win/text_win.h"
#include "nativeui/state.h"
#include "nativeui/win/container_win.h"

namespace nu {

namespace {

// The offset of title to left of rect.
const int kTitleLeftMargin = 5;

class GroupView : public ContainerView,
                  public ContainerView::Delegate {
 public:
  explicit GroupView(Group* delegate)
      : ContainerView(this, ControlType::Group),
        delegate_(delegate),
        color_(GetThemeColor(ThemeColor::Text)),
        font_(State::GetCurrent()->GetDefaultFont()) {}


  void SetTitle(const base::string16& title) {
    title_ = title;

    // Update the rect of the title.
    title_bounds_ = RectF(PointF(kTitleLeftMargin * scale_factor(), 0),
                          MeasureText(this, font_.get(), title_));
  }

  base::string16 GetTitle() const {
    return title_;
  }

  Insets GetBorder() const {
    int padding = std::ceil(2 * scale_factor());
    return Insets(title_bounds_.height() + padding, padding * 2,
                  padding * 2, padding * 2);
  }

  // ContainerView:
  void Layout() override {
    Rect child_alloc(size_allocation());
    child_alloc.Inset(GetBorder());
    delegate_->GetContentView()->GetNative()->SizeAllocate(child_alloc);
  }

  std::vector<ViewImpl*> GetChildren() override {
    return std::vector<ViewImpl*>{delegate_->GetContentView()->GetNative()};
  }

  // ViewImpl:
  void Draw(PainterWin* painter, const Rect& dirty) override {
    // Draw title.
    if (RectF(dirty).Intersects(title_bounds_))
      painter->DrawPixelStringWithFlags(title_, font_.get(), title_bounds_,
                                        Painter::TextAlignLeft);

    // Calculate the border bounds.
    RectF drawing_bounds(SizeF(size_allocation().size()));
    Insets border = GetBorder();
    RectF border_bounds(drawing_bounds);
    border_bounds.Inset(border.left() / 2, border.top() / 2,
                        border.right() / 2, border.bottom() / 2);

    // Draw border.
    painter->Save();
    painter->ClipPixelRect(drawing_bounds, Painter::CombineMode::Replace);
    painter->ClipPixelRect(title_bounds_, Painter::CombineMode::Exclude);
    painter->SetColor(Color(255, 170, 170, 170));
    painter->DrawPixelRect(border_bounds);
    painter->Restore();

    // Draw child.
    Vector2d child_offset(border.left(), border.top());
    painter->Save();
    painter->TranslatePixel(child_offset);
    delegate_->GetContentView()->GetNative()->Draw(
        painter, dirty - child_offset);
    painter->Restore();
  }

 private:
  Group* delegate_;

  Color color_;
  scoped_refptr<Font> font_;
  RectF title_bounds_;

  base::string16 title_;
};

}  // namespace

void Group::PlatformInit() {
  TakeOverView(new GroupView(this));
}

void Group::PlatformSetContentView(Container* container) {
  container->GetNative()->SetParent(GetNative());
  static_cast<GroupView*>(GetNative())->Layout();
}

void Group::SetTitle(const std::string& title) {
  static_cast<GroupView*>(GetNative())->SetTitle(base::UTF8ToUTF16(title));
  GetNative()->Invalidate();
}

std::string Group::GetTitle() const {
  return base::UTF16ToUTF8(static_cast<GroupView*>(GetNative())->GetTitle());
}

SizeF Group::GetBorderSize() const {
  GroupView* group = static_cast<GroupView*>(GetNative());
  Rect bounds;
  bounds.Inset(-group->GetBorder());
  return ScaleSize(SizeF(bounds.size()), 1.0f / group->scale_factor());
}

}  // namespace nu
