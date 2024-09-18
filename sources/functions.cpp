#include "functions.h"

vectPixelCoord getBorder(Mat* image, const vectPixelCoord& filledVertices) {
    std::vector<Vec2i> border;

    int rows = image->rows;
    int cols = image->cols;

    std::vector<std::vector<bool>> used(rows, std::vector<bool>(cols, 0));

    if (filledVertices.size() == 0) return border;

    std::queue<Vec2i> q;
    q.push(filledVertices[0]);

    int counter = 0;

    while (q.size() > 0) {
        Vec2i coords = q.front();

        if (used[coords[1]][coords[0]] == 1) {
            q.pop();
            continue;
        }

        used[coords[1]][coords[0]] = 1;

        // перечислим все соседние точки
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if ((y == 0) && (x == 0)) continue;
                // если новая точка находится внутри сетки
                if (NotOffBounds(image, coords[0] + x, coords[1] + y)) {
                    // если данная точка черная, а соседняя точка белая то
                    // мы нашли границу
                    if (isBlack(image, coords[0], coords[1]) &&
                        isWhite(image, coords[0] + x, coords[1] + y)) {
                        border.push_back(Vec2i(coords[0] + x, coords[1] + y));
                    }
                    // если новая точка не посещалась
                    if (used[coords[1] + y][coords[0] + x] == 0) {
                        q.push(Vec2i(coords[0] + x, coords[1] + y));

                        counter++;
                    }
                }
            }
        }

        q.pop();
    }

    return border;
}

vectPixelCoord getFilledVertices(Mat* image) {
    int rows = image->rows;
    int cols = image->cols;

    Pixel pixel(0, 0, 0);

    std::vector<Vec2i> result;

    if (getPixel(image, 0, 0) == pixel) {
        result.push_back(Vec2i(0, 0));
    }

    if (getPixel(image, cols - 1, 0) == pixel) {
        result.push_back(Vec2i(cols - 1, 0));
    }

    if (getPixel(image, 0, rows - 1) == pixel) {
        result.push_back(Vec2i(0, rows - 1));
    }

    if (getPixel(image, cols - 1, rows - 1) == pixel) {
        result.push_back(Vec2i(cols - 1, rows - 1));
    }

    return result;
}

std::vector<bool> getFilledVerticesBool(Mat* image) {
    int rows = image->rows;
    int cols = image->cols;

    Pixel pixel(0, 0, 0);

    std::vector<bool> result(4, 0);

    if (getPixel(image, 0, 0) == pixel) {
        result[0] = 1;
    }

    if (getPixel(image, cols - 1, 0) == pixel) {
        result[1] = 1;
    }

    if (getPixel(image, 0, rows - 1) == pixel) {
        result[3] = 1;
    }

    if (getPixel(image, cols - 1, rows - 1) == pixel) {
        result[2] = 1;
    }

    return result;
}

pairPixelCoord getRectangularBorder(Mat* image,
                                    const vectPixelCoord& filledVertices,
                                    const vectPixelCoord& border) {
    int x_min = image->cols;
    int y_min = image->rows;
    int x_max = 0;
    int y_max = 0;

    for (auto pixel : border) {
        x_min = std::min(x_min, pixel[0]);
        x_max = std::max(x_max, pixel[0]);

        y_min = std::min(y_min, pixel[1]);
        y_max = std::max(y_max, pixel[1]);
    }

    for (auto pixel : filledVertices) {
        x_min = std::min(x_min, pixel[0]);
        x_max = std::max(x_max, pixel[0]);

        y_min = std::min(y_min, pixel[1]);
        y_max = std::max(y_max, pixel[1]);
    }

    return std::make_pair(PixelCoord(x_min, y_min), PixelCoord(x_max, y_max));
}

vectPixelCoord getApproxymation(Mat* image, const vectPixelCoord& border,
                                pairPixelCoord rectangularBorder) {
    std::vector<bool> filled = getFilledVerticesBool(image);

    PixelCoord top_left = rectangularBorder.first;
    PixelCoord bottom_right = rectangularBorder.second;

    PixelCoord bottom_left = PixelCoord(top_left[0], bottom_right[1]);
    PixelCoord top_right = PixelCoord(bottom_right[0], top_left[1]);

    PixelCoord NEW_top_left = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_bottom_left = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_top_right = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_bottom_right = PixelCoord(10E+7, 10E+7);

    for (auto pixel : border) {
        if (getLenght(top_left, pixel) < getLenght(top_left, NEW_top_left))
            NEW_top_left = pixel;
        if (getLenght(top_right, pixel) < getLenght(top_right, NEW_top_right))
            NEW_top_right = pixel;
        if (getLenght(bottom_left, pixel) <
            getLenght(bottom_left, NEW_bottom_left))
            NEW_bottom_left = pixel;
        if (getLenght(bottom_right, pixel) <
            getLenght(bottom_right, NEW_bottom_right))
            NEW_bottom_right = pixel;
    }

    if (filled[0]) NEW_top_left = top_left;
    if (filled[1]) NEW_top_right = top_right;
    if (filled[2]) NEW_bottom_right = bottom_right;
    if (filled[3]) NEW_bottom_left = bottom_left;

    vectPixelCoord rectangularBorderult = {NEW_top_left, NEW_top_right,
                                           NEW_bottom_right, NEW_bottom_left};
    return rectangularBorderult;
}

#pragma force_inline
inline Pixel getPixel(Mat* image, int x, int y) {
    return image->at<Vec3b>(y, x);
}

#pragma force_inline
inline bool NotOffBounds(Mat* image, int x, int y) {
    bool alright = true;

    if ((x < 0) || (x >= image->cols)) alright = false;

    if ((y < 0) || (y >= image->rows)) alright = false;

    return alright;
}

#pragma force_inline
inline bool isWhite(Mat* image, int x, int y) {
    return (getPixel(image, x, y) == WHITE);
}

#pragma force_inline
inline bool isBlack(Mat* image, int x, int y) {
    return (getPixel(image, x, y) == BLACK);
}

ull getLenght(PixelCoord first, PixelCoord second) {
    return std::pow((ull)std::abs(first[0] - second[0]), 2) +
           std::pow((ull)std::abs(first[1] - second[1]), 2);
}
