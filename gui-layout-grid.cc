#include <vector>
#include <variant>
#include <cmath>
#include <optional>

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


struct vec2
{
    float x;
    float y;
};

struct Size
{
    float width;
    float height;
};
struct Lrtb
{
    float left;
    float right;
    float top;
    float bottom;
};
struct Rect
{
    vec2 position;
    vec2 size;
};

struct GridFitUniform {};
struct GridFitWidth {float cell_height;};
struct GridFitHeight {float cell_width;};
struct GridFitFixedRows {int rows; std::optional<float> cell_width; std::optional<float> cell_height;};
struct GridFitFixedColumns {int columns; std::optional<float> cell_width; std::optional<float> cell_height;};

struct Grid
{
    std::variant<
        GridFitUniform,
        GridFitWidth, GridFitHeight,
        GridFitFixedRows, GridFitFixedColumns>
        type;
    vec2 spacing; // space between elements
    const Lrtb& padding; // space around all elements
};

int ceil_to_int(float f)
{
    return static_cast<int>(std::ceil(f));
}

// internal
struct SizeInfo { int rows; int columns; };

int size_default(int number_of_children)
{
    return ceil_to_int(std::sqrt(static_cast<float>(number_of_children)));
}
int size_from_fixed(int number_of_children, int fixed)
{
    return ceil_to_int(static_cast<float>(number_of_children) / static_cast<float>(fixed));
}

// based on https://www.youtube.com/watch?v=CGsEJToeXmA
std::vector<Rect> perform_grid_layout(const Grid& grid, const Size& size, int number_of_children)
{
    const SizeInfo info = MATCH(grid.type)
    {
        [&](const GridFitWidth&)
        {
            const auto columns = size_default(number_of_children);
            return SizeInfo{.rows = size_from_fixed(number_of_children, columns), .columns = columns};
        },
        [&](const GridFitHeight&)
        {
            const auto rows = size_default(number_of_children);
            return SizeInfo{.rows = rows, .columns = size_from_fixed(number_of_children, rows)};
        },
        [&](const GridFitUniform&)
        {
            const auto both = size_default(number_of_children);
            return SizeInfo{.rows = both, .columns = both};
        },
        [&](const GridFitFixedColumns& fix)
        {
            const auto columns = fix.columns;
            return SizeInfo{.rows = size_from_fixed(number_of_children, columns), .columns = columns};
        },
        [&](const GridFitFixedRows& fix)
        {
            const auto rows = fix.rows;
            return SizeInfo{.rows = rows, .columns = size_from_fixed(number_of_children, rows)};
        }
    };

    // todo(Gustav): cleanup
    const float cell_width = size.width / static_cast<float>(info.columns) - ((grid.spacing.x / static_cast<float>(info.columns)) * (info.columns - 1))
        - (grid.padding.left / static_cast<float>(info.columns)) - (grid.padding.right / static_cast<float>(info.columns));
    const float cell_height = size.height / static_cast<float>(info.rows) - ((grid.spacing.y / static_cast<float>(info.rows)) * (info.rows - 1))
        - (grid.padding.top / static_cast<float>(info.rows)) - (grid.padding.bottom / static_cast<float>(info.rows));

    const vec2 cellSize = MATCH(grid.type)
    {
        [&](const GridFitWidth& width)
        {
            return vec2{cell_width, width.cell_height};
        },
        [&](const GridFitHeight& height)
        {
            return vec2{height.cell_width, cell_height};
        },
        [&](const GridFitUniform&)
        {
            return vec2{cell_width, cell_height};
        },
        [&](const GridFitFixedColumns& fix)
        {
            return vec2{
                fix.cell_width.value_or(cell_width),
                fix.cell_height.value_or(cell_height)};
        },
        [&](const GridFitFixedRows& fix)
        {
            return vec2{
                fix.cell_width.value_or(cell_width),
                fix.cell_height.value_or(cell_height)};
        }
    };

    std::vector<Rect> ret;
    for (int i = 0; i < number_of_children; i++)
    {
        const auto rowCount = i / info.columns;
        const auto columnCount = i % info.columns;

        const auto xPos = (cellSize.x * columnCount) + (grid.spacing.x * columnCount) + grid.padding.left;
        const auto yPos = (cellSize.y * rowCount) + (grid.spacing.y * rowCount) + grid.padding.top;

        ret.emplace_back(Rect{{xPos, yPos}, cellSize});
    }
    return ret;
}