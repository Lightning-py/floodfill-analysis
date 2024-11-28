#include "functions.h"

PixelCoord fective(-1, -1);

void image::getFilledVerticesSegm(segment& segm) {
    std::vector<PixelCoord> result;

    if (isBlack(imageObject, segm.leftUp[0], segm.leftUp[1]))
        result.push_back(segm.leftUp);
    if (isBlack(imageObject, segm.leftBottom[0], segm.leftBottom[1]))
        result.push_back(segm.leftBottom);
    if (isBlack(imageObject, segm.rightUp[0], segm.rightUp[1]))
        result.push_back(segm.rightUp);
    if (isBlack(imageObject, segm.rightBottom[0], segm.rightBottom[1]))
        result.push_back(segm.rightBottom);

    segm.filledVertices = result;
}

void image::getFilledVertices(uint32_t row, uint32_t col) {
    getFilledVerticesSegm(segments[row][col]);
}

void image::calculateFilledVertices() {
    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments[i].size(); ++j) {
            getFilledVertices(i, j);
        }
    }
}

std::vector<bool> image::getFilledVerticesBoolSegm(segment& segm) {
    std::vector<bool> result(4, 0);

    if (isBlack(imageObject, segm.leftUp[0], segm.leftUp[1])) result[0] = 1;

    if (isBlack(imageObject, segm.leftBottom[0], segm.leftBottom[1]))
        result[3] = 1;

    if (isBlack(imageObject, segm.rightUp[0], segm.rightUp[1])) result[1] = 1;

    if (isBlack(imageObject, segm.rightBottom[0], segm.rightBottom[1]))
        result[2] = 1;

    return result;
}

std::vector<bool> image::getFilledVerticesBool(uint32_t col, uint32_t row) {
    return getFilledVerticesBoolSegm(segments[row][col]);
}

void image::getBorder() {
    int rows = imageObject->rows;
    int cols = imageObject->cols;

    std::vector<std::vector<std::pair<bool, bool>>> used(
        rows, std::vector<std::pair<bool, bool>>(cols, std::make_pair(0, 0)));

    std::queue<PixelCoord> q;
    q.push(PixelCoord(0, 0));

    int counter = 0;

    while (q.size() > 0) {
        PixelCoord coords = q.front();

        if (used[coords[1]][coords[0]].first == 1) {
            q.pop();
            continue;
        }

        used[coords[1]][coords[0]].first = 1;

        // перечислим все соседние точки
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                if ((y == 0) && (x == 0)) continue;
                // если новая точка находится внутри сетки
                if (NotOffBounds(imageObject, coords[0] + x, coords[1] + y)) {
                    // если данная точка черная, а соседняя точка белая то
                    // мы нашли границу
                    if (isBlack(imageObject, coords[0], coords[1]) &&
                        isWhite(imageObject, coords[0] + x, coords[1] + y)) {
                        // coords[0] + x, coords[1] + y

                        segments[(coords[1] + y) / elementHeight]
                                [(coords[0] + x) / elementWIdth]
                                    .border.push_back(PixelCoord(
                                        coords[0] + x, coords[1] + y));

                        // segm.border.emplace(
                        // PixelCoord(coords[0] + x, coords[1] + y));
                    }
                    // если новая точка не посещалась
                    if (used[coords[1] + y][coords[0] + x].first == 0) {
                        q.push(PixelCoord(coords[0] + x, coords[1] + y));
                        if ((used[coords[1] + y][coords[0] + x].second == 0) &&
                            (isBlack(imageObject, coords[0] + x,
                                     coords[1] + y))) {
                            segments[(coords[1] + y) / elementHeight]
                                    [(coords[0] + x) / elementWIdth]
                                        .count++;

                            used[coords[1] + y][coords[0] + x].second = 1;
                        }

                        counter++;
                    }
                }
            }
        }

        q.pop();
    }
}

#pragma force_inline
inline Pixel getPixel(cv::Mat* image, int x, int y) {
    return image->at<Pixel>(y, x);
}

#pragma force_inline
inline void setPixel(cv::Mat* image, int x, int y, Pixel color) {
    image->at<Pixel>(y, x) = color;
}

#pragma force_inline
inline bool NotOffBounds(cv::Mat* image, int x, int y) {
    bool alright = true;

    if ((x < 0) || (x >= image->cols)) alright = false;

    if ((y < 0) || (y >= image->rows)) alright = false;

    return alright;
}

#pragma force_inline
inline bool isWhite(cv::Mat* image, int x, int y) {
    return (getPixel(image, x, y) == WHITE);
}

#pragma force_inline
inline bool isBlack(cv::Mat* image, int x, int y) {
    return (getPixel(image, x, y) == BLACK);
}

ull getLenght(PixelCoord first, PixelCoord second) {
    return std::pow((ull)std::abs(first[0] - second[0]), 2) +
           std::pow((ull)std::abs(first[1] - second[1]), 2);
}

void draw(cv::Mat* image, const PixelCoord& leftUp,
          const PixelCoord& rightBottom, Pixel color) {
    for (int i = leftUp[0]; i < rightBottom[0]; ++i) {
        setPixel(image, i, leftUp[1], color);
        setPixel(image, i, rightBottom[1], color);
    }

    for (int i = leftUp[1]; i < rightBottom[1]; ++i) {
        setPixel(image, leftUp[0], i, color);
        setPixel(image, rightBottom[0], i, color);
    }
}

void image::calculateCoeficient() {
    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments[i].size(); ++j) {
            segments[i][j].integrityPart =
                segments[i][j].count / (float)((segments[i][j].leftBottom[1] -
                                                segments[i][j].leftUp[1]) *
                                               (segments[i][j].rightBottom[0] -
                                                segments[i][j].leftBottom[0]));

            if (segments[i][j].integrityPart >= 0.99) {
                segments[i][j].is_full = true;
                segments[i][j].is_empty = false;

                segments[i][j].newLeftUp = segments[i][j].leftUp;
                segments[i][j].newLeftBottom = segments[i][j].leftBottom;
                segments[i][j].newRightUp = segments[i][j].rightUp;
                segments[i][j].newRightBottom = segments[i][j].rightBottom;
            } else {
                if (segments[i][j].integrityPart < 0.1) {
                    segments[i][j].is_empty = true;
                    segments[i][j].is_full = false;
                }

                segments[i][j].newLeftUp = fective;
                segments[i][j].newLeftBottom = fective;
                segments[i][j].newRightUp = fective;
                segments[i][j].newRightBottom = fective;
            }
        }
    }
}

void image::divideOnSegments(bool show) {
    PixelCoord leftUp(0, 0);
    PixelCoord leftBottom(0, elementHeight);
    PixelCoord rightUp(elementWIdth, 0);
    PixelCoord rightBottom(elementWIdth, elementHeight);

    for (int i = 0; i < heightCount; ++i) {
        for (int j = 0; j < widthCount; ++j) {
            segments[i][j] = segment{leftUp,
                                     leftBottom,
                                     rightUp,
                                     rightBottom,
                                     0,
                                     false,
                                     false,
                                     -1,
                                     std::vector<PixelCoord>(),
                                     std::vector<PixelCoord>(),
                                     fective,
                                     fective,
                                     fective,
                                     fective,
                                     std::make_pair(-1, -1),
                                     (uint32_t)i,
                                     (uint32_t)j};

            if (show) {
                draw(imageOutput, leftUp, rightBottom, CYAN);
            }

            leftUp[0] += elementWIdth;
            leftBottom[0] += elementWIdth;
            rightUp[0] = std::min((uint32_t)imageObject->cols - 1,
                                  rightUp[0] + elementWIdth);
            rightBottom[0] = std::min((uint32_t)imageObject->cols - 1,
                                      rightBottom[0] + elementWIdth);
        }

        leftUp[1] += elementHeight;
        leftBottom[1] = std::min((uint32_t)imageObject->rows - 1,
                                 leftBottom[1] + elementHeight);

        rightUp[1] += elementHeight;
        rightBottom[1] = std::min((uint32_t)imageObject->rows - 1,
                                  rightBottom[1] + elementHeight);
        leftUp[0] = 0;
        leftBottom[0] = 0;
        rightUp[0] = elementWIdth;
        rightBottom[0] = elementWIdth;
    }
}

void image::getRecctangularBorder(segment& segm, bool show) {
    int x_min = segm.rightBottom[0];
    int y_min = segm.rightBottom[1];
    int x_max = 0;
    int y_max = 0;

    for (auto pixel : segm.border) {
        x_min = std::min(x_min, pixel[0]);
        x_max = std::max(x_max, pixel[0]);

        y_min = std::min(y_min, pixel[1]);
        y_max = std::max(y_max, pixel[1]);
    }

    for (auto pixel : segm.filledVertices) {
        x_min = std::min(x_min, pixel[0]);
        x_max = std::max(x_max, pixel[0]);

        y_min = std::min(y_min, pixel[1]);
        y_max = std::max(y_max, pixel[1]);
    }

    if (show) {
        draw(imageOutput, PixelCoord(x_min + 1, y_min + 1),
             PixelCoord(x_max - 1, y_max - 1), GREEN);
    }

    segm.rectangularBorder =
        std::make_pair(PixelCoord(x_min, y_min), PixelCoord(x_max, y_max));
}

void image::calculateRectangularBorder(bool show) {
    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments[i].size(); ++j) {
            getRecctangularBorder(segments[i][j], show);
        }
    }
}

void image::getApproxymation(segment& segm, bool show) {
    if ((segm.is_empty || segm.is_full) == 1) return;
    std::vector<bool> filled = getFilledVerticesBoolSegm(segm);

    PixelCoord top_left = segm.rectangularBorder.first;
    PixelCoord bottom_right = segm.rectangularBorder.second;

    PixelCoord bottom_left = PixelCoord(top_left[0], bottom_right[1]);
    PixelCoord top_right = PixelCoord(bottom_right[0], top_left[1]);

    PixelCoord NEW_top_left = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_bottom_left = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_top_right = PixelCoord(10E+7, 10E+7);
    PixelCoord NEW_bottom_right = PixelCoord(10E+7, 10E+7);

    for (auto pixel : segm.border) {
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
    if (filled[3]) NEW_bottom_right = bottom_right;
    if (filled[2]) NEW_bottom_left = bottom_left;

    // vectPixelCoord rectangularBorderult = {NEW_top_left, NEW_top_right,
    //    NEW_bottom_right, NEW_bottom_left};
    segm.newLeftUp = NEW_top_left;
    segm.newLeftBottom = NEW_bottom_left;
    segm.newRightUp = NEW_top_right;
    segm.newRightBottom = NEW_bottom_right;

    // cv::polylines(*imageOutput, ppt, npt, 1, cv::Scalar(0, 255, 255));
    if (show) {
        cv::line(*imageOutput, cv::Point2i(segm.newLeftUp),
                 cv::Point2i(segm.newRightUp), YELLOW);
        cv::line(*imageOutput, cv::Point2i(segm.newRightUp),
                 cv::Point2i(segm.newRightBottom), YELLOW);
        cv::line(*imageOutput, cv::Point2i(segm.newRightBottom),
                 cv::Point2i(segm.newLeftBottom), YELLOW);
        cv::line(*imageOutput, cv::Point2i(segm.newLeftBottom),
                 cv::Point2i(segm.newLeftUp), YELLOW);
    }
}

void image::calculateApproxymation(bool show) {
    for (int i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < segments[i].size(); ++j) {
            getApproxymation(segments[i][j], show);
        }
    }
}

PixelCoord image::getNeightbourSegment(int i, int j, int number) {
    if (i < 0 || i >= segments.size() || j < 0 || j >= segments[0].size())
        return fective;

    if (number == 0) return segments[i][j].newLeftUp;
    if (number == 1) return segments[i][j].newRightUp;
    if (number == 2) return segments[i][j].newLeftBottom;
    if (number == 3) return segments[i][j].newRightBottom;
    return fective;
}

void image::setNeightbourSegmentCoord(int i, int j, int number,
                                      PixelCoord pixel) {
    if (i < 0 || i >= segments.size() || j < 0 || j >= segments[0].size())
        return;

    if (number == 0) segments[i][j].newLeftUp = pixel;
    if (number == 1) segments[i][j].newRightUp = pixel;
    if (number == 2) segments[i][j].newLeftBottom = pixel;
    if (number == 3) segments[i][j].newRightBottom = pixel;
}

void image::getFinalPositions(bool show) {
    // for (auto& arr : segments) {
    //     for (auto& segm : arr) {
    //         std::cout << segm.newLeftUp << "\t" << segm.newRightUp << "\t"
    //                   << segm.newLeftBottom << "\t" << segm.newRightBottom
    //                   << std::endl;
    //     }
    // }

    int counter;
    for (int i = 0; i < segments.size(); ++i) {
        counter = 0;

        PixelCoord res(0, 0);
        PixelCoord temp;

        temp = segments[i][0].newLeftBottom;
        if (temp != fective) {
            counter++;
            res += temp;
        }

        temp = getNeightbourSegment(i + 1, 0, 0);
        if (temp != fective) {
            counter++;
            res += temp;
        }

        if (counter != 0) {
            res /= counter;
            segments[i][0].leftBottom = res;
            setNeightbourSegmentCoord(i + 1, 0, 0, res);
        } else {
            segments[i][0].leftBottom = fective;
            setNeightbourSegmentCoord(i + 1, 0, 0, fective);
        }

        for (int j = 0; j < segments[0].size(); ++j) {
            counter = 0;

            PixelCoord res(0, 0);
            PixelCoord temp;

            temp = segments[i][j].newRightBottom;
            if (temp != fective) {
                counter++;
                res += temp;
            }

            temp = getNeightbourSegment(i + 1, j, 1);
            if (temp != fective) {
                counter++;
                res += temp;
            }

            temp = getNeightbourSegment(i + 1, j + 1, 0);
            if (temp != fective) {
                counter++;
                res += temp;
            }

            temp = getNeightbourSegment(i, j + 1, 2);
            if (temp != fective) {
                counter++;
                res += temp;
            }

            if (counter != 0) {
                res /= counter;
                segments[i][j].rightBottom = res;
                setNeightbourSegmentCoord(i + 1, j, 1, res);
                setNeightbourSegmentCoord(i + 1, j + 1, 0, res);
                setNeightbourSegmentCoord(i, j + 1, 2, res);

            } else {
                segments[i][j].rightBottom = fective;
                setNeightbourSegmentCoord(i + 1, j, 1, fective);
                setNeightbourSegmentCoord(i + 1, j + 1, 0, fective);
                setNeightbourSegmentCoord(i, j + 1, 2, fective);
            }
        }
    }

    for (int j = 0; j < segments.size() - 1; ++j) {
        counter = 0;

        PixelCoord res(0, 0);
        PixelCoord temp;

        temp = segments[0][j].newRightUp;
        if (temp != fective) {
            counter++;
            res += temp;
        }

        temp = getNeightbourSegment(0, j + 1, 0);
        if (temp != fective) {
            counter++;
            res += temp;
        }

        if (counter != 0) {
            res /= counter;
            segments[0][j].rightUp = res;
            setNeightbourSegmentCoord(0, j + 1, 0, res);
        } else {
            segments[0][j].rightUp = fective;
            setNeightbourSegmentCoord(0, j + 1, 0, fective);
        }
    }

    if (show) {
        for (auto& arr : segments) {
            for (auto& segm : arr) {
                if (segm.is_empty) continue;
                cv::line(*imageOutput, cv::Point2i(segm.newLeftUp),
                         cv::Point2i(segm.newRightUp), YELLOW);
                cv::line(*imageOutput, cv::Point2i(segm.newRightUp),
                         cv::Point2i(segm.newRightBottom), YELLOW);
                cv::line(*imageOutput, cv::Point2i(segm.newRightBottom),
                         cv::Point2i(segm.newLeftBottom), YELLOW);
                cv::line(*imageOutput, cv::Point2i(segm.newLeftBottom),
                         cv::Point2i(segm.newLeftUp), YELLOW);
            }
        }
    }

    // cv::line(*imageOutput, cv::Point2i(segments[0][0].newLeftUp),
    //          cv::Point2i(segments[0][0].newRightUp), YELLOW);
    // cv::line(*imageOutput, cv::Point2i(segments[0][0].newRightUp),
    //          cv::Point2i(segments[0][0].newRightBottom), YELLOW);
    // cv::line(*imageOutput, cv::Point2i(segments[0][0].newRightBottom),
    //          cv::Point2i(segments[0][0].newLeftBottom), YELLOW);
    // cv::line(*imageOutput, cv::Point2i(segments[0][0].newLeftBottom),
    //          cv::Point2i(segments[0][0].newLeftUp), YELLOW);

    // for (auto& arr : segments) {
    //     for (auto& segm : arr) {
    //         std::cout << segm.newLeftUp << "\t" << segm.newRightUp << "\t"
    //                   << segm.newLeftBottom << "\t" << segm.newRightBottom
    //                   << std::endl;
    //     }
    // }
}
