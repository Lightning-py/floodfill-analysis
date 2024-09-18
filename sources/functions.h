#ifndef FUNCTIONS
#define FUNCTIONS

#include <stdio.h>
#include <stdlib.h>

#include <opencv4/opencv2/opencv.hpp>
#include <queue>

using namespace cv;

typedef cv::Vec3b Pixel;
typedef cv::Vec2i PixelCoord;
typedef std::vector<PixelCoord> vectPixelCoord;
typedef std::pair<PixelCoord, PixelCoord> pairPixelCoord;
typedef unsigned long long ull;

#define WHITE Pixel(255, 255, 255)
#define BLACK Pixel(0, 0, 0)
#define GREEN Pixel(0, 255, 0)
#define RED Pixel(0, 0, 255)

vectPixelCoord getFilledVertices(Mat* image);
std::vector<bool> getFilledVerticesBool(Mat* image);

vectPixelCoord getBorder(Mat* image, const vectPixelCoord& filledVertices);

pairPixelCoord getRectangularBorder(Mat* image,
                                    const vectPixelCoord& filledVertices,
                                    const vectPixelCoord& border);

vectPixelCoord getApproxymation(Mat* image, const vectPixelCoord& border,
                                pairPixelCoord rectangularBorder);

ull getLenght(PixelCoord first, PixelCoord second);

inline Pixel getPixel(Mat* image, int x, int y);
inline bool NotOffBounds(Mat* image, int x, int y);
inline bool isWhite(Mat* image, int x, int y);
inline bool isBlack(Mat* image, int x, int y);

#endif  // FUNCTIONS
