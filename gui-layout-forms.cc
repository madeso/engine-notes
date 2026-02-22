/*
   This implements the forms layout.
   Widgets are drawn in a design window and are anchored to the window edges. 
   It doesn't handle all layouts but is easy to understand and easy
   to tweak in a wysiwyg layout editor.
*/
#include <variant>

// simple match lib
template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };
template<typename T> struct Matcher
{
    T v;
    auto operator<<(auto c)
    { return std::visit(c, v); }
    auto operator<<(const auto& c) const
    { return std::visit(c, v); }
};
#define MATCH(t) Matcher{t} << overloads

struct Size {
    int cx;
    int cy;
};

struct Rect {
    int left;
    int top;
    int right;
    int bottom;
};


Size size_of(const Rect& r) {
    return {
        .cx = r.right - r.left,
        .cy = r.top - r.bottom
    };
};

// todo(Gustav): cz: bitfield with invalid values? how to parse to/from c?
enum class HorizontalAnchor {left, right, both};
enum class VerticalAnchor {top, bottom, both};
struct Anchor
{
    HorizontalAnchor horizontal;
    VerticalAnchor vertical;
};


struct HorizontalLeft {int left_offset; int width; };
struct HorizontalRight {int right_offset; int width; };
struct HorizontalBoth {int left_offset; int right_offset;};
using HorizontalSizeInfo = std::variant<HorizontalLeft, HorizontalRight, HorizontalBoth>;
    
struct VerticalTop { int top_offset; int height;};
struct VerticalBottom { int bottom_offset; int height;};
struct VerticalBoth { int top_offset; int bottom_offset;};
using VerticalSizeInfo = std::variant<VerticalTop, VerticalBottom, VerticalBoth>;

struct SizeInfo
{
    HorizontalSizeInfo horizontal;
    VerticalSizeInfo vertical;
};

SizeInfo size_info_from_design_information(const Rect& widget_rect, Anchor anchor, const Rect& design_size)
{
    const auto size = size_of(widget_rect);

    const auto horizontal = ([&]() -> HorizontalSizeInfo {
        const auto left_offset = widget_rect.left - design_size.left;
        const auto right_offset = design_size.right - widget_rect.right;
        const auto width = size.cx;
        switch(anchor.horizontal) {
        case HorizontalAnchor::left: return HorizontalLeft { left_offset, width };
        case HorizontalAnchor::right: return HorizontalRight { right_offset, width };
        case HorizontalAnchor::both: return HorizontalBoth { left_offset, right_offset };
        }
    })();
    const auto vertical = ([&]() -> VerticalSizeInfo {
        const auto top_offset = widget_rect.top - design_size.top;
        const auto bottom_offset = design_size.bottom - widget_rect.bottom;
        const auto height = size.cy;
        switch(anchor.vertical) {
        case VerticalAnchor::top: return VerticalTop{top_offset, height};
        case VerticalAnchor::bottom: return VerticalBottom{bottom_offset, height};
        case VerticalAnchor::both: return VerticalBoth{top_offset, bottom_offset};
        }
    })();
    
    return {horizontal, vertical};
}

struct LeftRight {int left; int right;};
struct TopBottom {int top; int bottom;};

int calc_left(const Rect& window_size, int left_offset) { return left_offset + window_size.left; }
int calc_right(const Rect& window_size, int right_offset) { return window_size.right - right_offset; }
int calc_top(const Rect& window_size, int top_offset) { return top_offset + window_size.top; }
int calc_bottom(const Rect& window_size, int bottom_offset) { return window_size.bottom - bottom_offset; }

Rect position_widget(const SizeInfo& info, const Rect& window_size)
{
    const LeftRight lr = MATCH(info.horizontal)
    {
        [&](HorizontalLeft& d) {
            const auto left = calc_left(window_size, d.left_offset);
            return LeftRight{left, left + d.width};
        },
        [&](HorizontalRight& d) {
            const auto right = calc_right(window_size, d.right_offset);
            return LeftRight{right - d.width, right};
        },
        [&](HorizontalBoth& d) {
            return LeftRight{calc_left(window_size, d.left_offset), calc_right(window_size, d.right_offset)};
        }
    };

    const TopBottom tb = MATCH(info.vertical)
    {
        [&](const VerticalTop& d) {
            const auto top = calc_top(window_size, d.top_offset);
            return TopBottom{top, top + d.height};
        },
        [&](const VerticalBottom& d) {
            const auto bottom = calc_bottom(window_size, d.bottom_offset);
            return TopBottom{bottom - d.height, bottom};
        },
        [&](const VerticalBoth& d) {
            return TopBottom{calc_top(window_size, d.top_offset), calc_bottom(window_size, d.bottom_offset)};
        }
    };

    return {lr.left, tb.top, lr.right, tb.bottom};
}
