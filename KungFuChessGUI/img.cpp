#include "img.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>


Img::Img() {
    // Constructor - img is automatically initialized as empty, windowName stays unset
}

Img::Img(std::string windowName)
    : windowName(std::move(windowName)) {
    // img is automatically initialized as empty
}

Img& Img::read(const std::string& path,
               const std::pair<int, int>& size,
               bool keep_aspect,
               int interpolation) {
    img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + path);
    }

    if (size.first != 0 && size.second != 0) {  // Check if size is not empty
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect) {
            double scale = std::min(static_cast<double>(target_w) / w, 
                                   static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
        } else {
            cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
        }
    }

    return *this;
}

Img& Img::create(int width, int height, int type) {
    img = cv::Mat::zeros(height, width, type);
    return *this;
}

void Img::draw_on(Img& other_img, int x, int y) {
    if (img.empty() || other_img.img.empty()) {
        throw std::runtime_error("Both images must be loaded before drawing.");
    }

    cv::Mat source_img = img;
    cv::Mat target_img = other_img.img;

    int h = source_img.rows;
    int w = source_img.cols;
    int H = target_img.rows;
    int W = target_img.cols;

    if (y + h > H || x + w > W) {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    cv::Mat roi = target_img(cv::Rect(x, y, w, h));

    if (source_img.channels() == 4) {
        // Alpha-blend per pixel using the source's alpha channel, keeping the target's own channel count.
        std::vector<cv::Mat> srcChannels;
        cv::split(source_img, srcChannels);

        cv::Mat alpha;
        srcChannels[3].convertTo(alpha, CV_32FC1, 1.0 / 255.0);

        cv::Mat roiBgr;
        bool roiHasAlpha = roi.channels() == 4;
        if (roiHasAlpha) {
            cv::cvtColor(roi, roiBgr, cv::COLOR_BGRA2BGR);
        } else {
            roiBgr = roi;
        }

        std::vector<cv::Mat> roiChannels;
        cv::split(roiBgr, roiChannels);

        std::vector<cv::Mat> blended(3);
        for (int c = 0; c < 3; ++c) {
            cv::Mat srcF, roiF;
            srcChannels[c].convertTo(srcF, CV_32FC1);
            roiChannels[c].convertTo(roiF, CV_32FC1);
            cv::Mat mixed = srcF.mul(alpha) + roiF.mul(1.0f - alpha);
            mixed.convertTo(blended[c], roiChannels[c].type());
        }

        cv::Mat blendedBgr;
        cv::merge(blended, blendedBgr);

        if (roiHasAlpha) {
            cv::cvtColor(blendedBgr, roi, cv::COLOR_BGR2BGRA);
        } else {
            blendedBgr.copyTo(roi);
        }
    } else {
        // No alpha channel: direct copy, converting channel count to match the target.
        cv::Mat converted = source_img;
        if (source_img.channels() == 3 && roi.channels() == 4) {
            cv::cvtColor(source_img, converted, cv::COLOR_BGR2BGRA);
        } else if (source_img.channels() == 1 && roi.channels() == 3) {
            cv::cvtColor(source_img, converted, cv::COLOR_GRAY2BGR);
        }
        converted.copyTo(roi);
    }
}

Img Img::crop(int x, int y, int width, int height) const {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }

    cv::Rect rect(x, y, width, height);
    rect &= cv::Rect(0, 0, img.cols, img.rows); // clamp to bounds

    Img result;
    result.img = img(rect).clone();
    return result;
}

void Img::put_text(const std::string& txt, int x, int y, double font_size,
                   const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show() {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    if (windowName.empty()) {
        throw std::runtime_error("Img has no window name; construct it with Img(windowName) before calling show().");
    }

    cv::imshow(windowName, img);
} 