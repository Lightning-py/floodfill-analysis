#ifndef FUNCTIONS
#define FUNCTIONS 1

#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <opencv4/opencv2/opencv.hpp>
#include <queue>
#include <unordered_set>
#include <vector>

typedef cv::Vec3b Pixel;
typedef cv::Vec2i PixelCoord;
typedef std::vector<PixelCoord> vectPixelCoord;
typedef std::pair<PixelCoord, PixelCoord> pairPixelCoord;
typedef unsigned long long ull;

#define WHITE Pixel(255, 255, 255)
#define BLACK Pixel(0, 0, 0)
#define GREEN Pixel(0, 255, 0)
#define RED Pixel(0, 0, 255)
#define BLUE Pixel(255, 0, 0)
#define CYAN Pixel(255, 255, 0)
#define YELLOW Pixel(0, 255, 255)
#define MAGENTA Pixel(255, 0, 255)
#define GRAY Pixel(128, 128, 128)
#define DARK_GRAY Pixel(64, 64, 64)
#define LIGHT_GRAY Pixel(192, 192, 192)

struct vertex {
    int x;
    int y;
};

struct segment {
    PixelCoord leftUp;
    PixelCoord leftBottom;
    PixelCoord rightUp;
    PixelCoord rightBottom;

    uint32_t count = 0;
    bool is_full;
    bool is_empty;
    float integrityPart;

    std::vector<PixelCoord> border;
    std::vector<PixelCoord> filledVertices;

    PixelCoord newLeftUp;
    PixelCoord newLeftBottom;
    PixelCoord newRightUp;
    PixelCoord newRightBottom;

    std::pair<PixelCoord, PixelCoord> rectangularBorder;
    uint32_t coordx;
    uint32_t coordy;
};

ull getLenght(PixelCoord first, PixelCoord second);
inline Pixel getPixel(cv::Mat* image, int x, int y);
inline bool NotOffBounds(cv::Mat* image, int x, int y);
inline bool isWhite(cv::Mat* image, int x, int y);
inline bool isBlack(cv::Mat* image, int x, int y);

class image {
   public:
    uint32_t elementWIdth;
    uint32_t elementHeight;
    uint32_t widthCount;
    uint32_t heightCount;

    image(uint32_t width, uint32_t height, cv::Mat* image, cv::Mat* imageOutput)
        : elementWIdth(width),
          elementHeight(height),
          imageObject(image),
          imageOutput(imageOutput) {
        widthCount = (uint32_t)std::ceil((float)image->cols / elementWIdth);
        heightCount = (uint32_t)std::ceil((float)image->rows / elementHeight);

        if (widthCount == 0 || heightCount == 0)
            throw std::invalid_argument(
                "Размеры сегмента таковы, что изображение не может быть "
                "разбито на сегменты");

        segments = std::vector<std::vector<segment>>(
            heightCount, std::vector<segment>(widthCount, segment{}));
    }

    cv::Mat* imageObject;
    cv::Mat* imageOutput;
    std::vector<std::vector<segment>> segments;

    void divideOnSegments(bool show);
    void getFilledVerticesSegm(segment& segm);
    void getFilledVertices(uint32_t row, uint32_t col);
    std::vector<bool> getFilledVerticesBoolSegm(segment& segm);
    std::vector<bool> getFilledVerticesBool(uint32_t col, uint32_t row);
    void calculateFilledVertices();
    void getBorder();
    void calculateCoeficient();
    void getRecctangularBorder(segment& segm, bool show);
    void calculateRectangularBorder(bool show);
    void getApproxymation(segment& segm, bool show);
    void calculateApproxymation(bool show);
    PixelCoord getNeightbourSegment(int i, int j, int number);
    void setNeightbourSegmentCoord(int i, int j, int number, PixelCoord pixel);
    void getFinalPositions(bool show);

    void solve();

    bool segments_show = false;
    bool rectangular_border_show = false;
    bool approxymation_show = false;
    bool final_position_show = false;
};

#endif  // FUNCTIONS
