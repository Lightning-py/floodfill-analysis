#include <argparse/argparse.hpp>

#include "functions.h"

using namespace cv;

int main(int argc, char** argv) {
    // if (argc != 2) {
    //     printf("usage: DisplayImage.out <Image_Path>\n");
    //     return -1;
    // }

    // if (Image.empty()) {
    //     printf("Error: Unable to open or read image file %s\n", argv[1]);
    //     return -1;
    // }

    // Mat image_output = Mat::zeros(image.rows, image.cols, CV_8UC3);

    // Im.divideOnSegments(true);
    // Im.getBorder();
    // Im.calculateCoeficient();
    // Im.calculateFilledVertices();
    // Im.calculateRectangularBorder(true);
    // Im.calculateApproxymation(true);
    // Im.getFinalPositions(true);

    argparse::ArgumentParser program("floodfill-analysis");

    program.add_argument("-s", "--segments")
        .help("показывать разделенные сегменты")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-r", "--rectangular-border")
        .help(
            "показывать прямоугольную границу (наименьший прямоугольник, в "
            "который можно вписать фигуру)")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-a", "--approxymation")
        .help("показывать первичную аппроксимацию фигуры")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-f", "--final-position")
        .help("показывать финальную аппроксимацию фигуры")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-F", "--file").required();

    program.add_argument("-H", "--height").required().scan<'d', int>();

    program.add_argument("-W", "--width").required().scan<'d', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto filename = program.get<std::string>("-F");
    auto height = program.get<int>("-H");
    auto width = program.get<int>("-W");

    Mat Image;
    Image = imread(filename, IMREAD_COLOR);
    // int height = std::stoi(std::string(argv[3]));
    // int width = std::stoi(std::string(argv[2]));

    Mat image_output = Image.clone();
    image Im(height, width, &Image, &image_output);

    if (program.get<bool>("--segments")) {
        Im.segments_show = true;
    }

    if (program.get<bool>("--rectangular-border")) {
        Im.rectangular_border_show = true;
    }

    if (program.get<bool>("--approxymation")) {
        Im.approxymation_show = true;
    }

    if (program.get<bool>("--final-position")) {
        Im.final_position_show = true;
    }

    Im.solve();

    // for (auto i : Im.segments[0][0].filledVertices) std::cout << i << "\t";
    // std::cout << std::endl;
    // Im.getApproxymation(Im.segments[0][0], true);
    // Im.getRecctangularBorder(Im.segments[0][0], true);
    // for (auto& arr : Im.segments) {
    //     for (auto& segm : arr) {
    //         // std::cout << "Black cells in segment: " << segm.count <<
    //         // std::endl;
    //         std::cout << segm.leftUp << "\t" << segm.rightBottom <<
    //         std::endl;
    //         // std::cout << "Border";
    //         // for (auto i: segm.border) std::cout <<
    //         for (auto i : segm.filledVertices) std::cout << i << "\t";
    //         std::cout << std::endl;
    //         std::cout << segm.rectangularBorder.first << "\t"
    //                   << segm.rectangularBorder.second << std::endl;
    //     }
    // }

    // vectPixelCoord filledVertices = getFilledVertices(&image);

    // vectPixelCoord border = getBorder(&image, filledVertices);

    // for (auto pixel : border) {
    //     image_output.at<Pixel>(pixel[1], pixel[0]) = WHITE;
    // }

    // pairPixelCoord rectangularBorder =
    //     getRectangularBorder(&image, filledVertices, border);

    // int x_min = rectangularBorder.first[0], x_max =
    // rectangularBorder.second[0],
    //     y_min = rectangularBorder.first[1], y_max =
    //     rectangularBorder.second[1];

    // for (int i = x_min; i <= x_max; ++i) {
    //     image_output.at<Pixel>(y_min, i) = GREEN;
    //     image_output.at<Pixel>(y_max, i) = GREEN;
    // }

    // for (int j = y_min; j <= y_max; ++j) {
    //     image_output.at<Pixel>(j, x_min) = GREEN;
    //     image_output.at<Pixel>(j, x_max) = GREEN;
    // }

    // vectPixelCoord approxymation =
    //     getApproxymation(&image, border, rectangularBorder);

    // cv::line(image_output, cv::Point2i(approxymation[0]),
    //          cv::Point2i(approxymation[1]), RED);
    // cv::line(image_output, cv::Point2i(approxymation[1]),
    //          cv::Point2i(approxymation[2]), RED);
    // cv::line(image_output, cv::Point2i(approxymation[2]),
    //          cv::Point2i(approxymation[3]), RED);
    // cv::line(image_output, cv::Point2i(approxymation[3]),
    //          cv::Point2i(approxymation[0]), RED);

    cv::imwrite(filename + "_output.png", image_output);

    return 0;
}