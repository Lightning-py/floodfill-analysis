#include "functions.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread(argv[1], IMREAD_COLOR);

    int rows = image.rows;
    int cols = image.cols;

    Mat image_output = cv::Mat::zeros(rows, cols, CV_8UC3);

    vectPixelCoord filledVertices = getFilledVertices(&image);
    vectPixelCoord border = getBorder(&image, filledVertices);

    for (auto pixel : border) {
        image_output.at<Pixel>(pixel[1], pixel[0]) = WHITE;
    }

    pairPixelCoord rectangularBorder =
        getRectangularBorder(&image, filledVertices, border);

    int x_min = rectangularBorder.first[0], x_max = rectangularBorder.second[0],
        y_min = rectangularBorder.first[1], y_max = rectangularBorder.second[1];

    for (int i = x_min; i <= x_max; ++i) {
        image_output.at<Pixel>(y_min, i) = GREEN;
        image_output.at<Pixel>(y_max, i) = GREEN;
    }

    for (int j = y_min; j <= y_max; ++j) {
        image_output.at<Pixel>(j, x_min) = GREEN;
        image_output.at<Pixel>(j, x_max) = GREEN;
    }

    vectPixelCoord approxymation =
        getApproxymation(&image, border, rectangularBorder);

    cv::line(image_output, cv::Point2i(approxymation[0]),
             cv::Point2i(approxymation[1]), RED);
    cv::line(image_output, cv::Point2i(approxymation[1]),
             cv::Point2i(approxymation[2]), RED);
    cv::line(image_output, cv::Point2i(approxymation[2]),
             cv::Point2i(approxymation[3]), RED);
    cv::line(image_output, cv::Point2i(approxymation[3]),
             cv::Point2i(approxymation[0]), RED);

    cv::imwrite(strcat(argv[1], "_output.png"), image_output);
    // cv::imshow("Image", image_output);
    // cv::waitKey(0);
    // cv::destroyAllWindows();

    return 0;
}