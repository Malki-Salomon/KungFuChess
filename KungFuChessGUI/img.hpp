#pragma once

//#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>

class Img {
public:
    // Default-constructs a "headless" image: a plain offscreen pixel buffer
    // with no window identity. Used for cached textures and composited
    // canvases that are drawn onto another Img rather than shown directly
    // (see TextureManager, BoardRenderer, BoardAnimationController). show() is not
    // valid on an image constructed this way.
    Img();

    // Constructs an image bound to a specific display window. windowName
    // must be unique per concurrently-running session (e.g. a session ID)
    // so that multiple sessions' OpenCV windows - and the mouse callbacks
    // registered on them - don't collide.
    explicit Img(std::string windowName);

    /**
     * Load image from path and optionally resize.
     * 
     * @param path Image file to load
     * @param size Target size in pixels (width, height). If empty, keep original
     * @param keep_aspect If true, shrink so the longer side fits size while preserving aspect ratio
     * @param interpolation OpenCV interpolation flag (e.g., cv::INTER_AREA for shrink, cv::INTER_LINEAR for enlarge)
     * @return Reference to this object for method chaining
     */
    Img& read(const std::string& path,
              const std::pair<int, int>& size = {},
              bool keep_aspect = false,
              int interpolation = cv::INTER_AREA);

    /**
     * Allocate a blank canvas of the given size, discarding any existing image.
     * Used to create a drawing surface that other images can be composited onto
     * via draw_on, without loading anything from disk.
     *
     * @param width Canvas width in pixels
     * @param height Canvas height in pixels
     * @param type OpenCV Mat type (default: CV_8UC3)
     * @return Reference to this object for method chaining
     */
    Img& create(int width, int height, int type = CV_8UC3);

    /**
     * Draw this image onto another image at position (x, y)
     * 
     * @param other_img The target image to draw on
     * @param x X coordinate for top-left corner
     * @param y Y coordinate for top-left corner
     */
    void draw_on(Img& other_img, int x, int y);
    
    /**
     * Put text on the image
     * 
     * @param txt Text to draw
     * @param x X coordinate for text position
     * @param y Y coordinate for text position (baseline)
     * @param font_size Font scale factor
     * @param color Text color (BGR or BGRA)
     * @param thickness Text thickness
     */
    void put_text(const std::string& txt, int x, int y, double font_size,
                  const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
                  int thickness = 1);
    
    /**
     * Display the image in this instance's window (the name passed to the
     * Img(windowName) constructor). Non-blocking: does not wait for a key
     * press and does not destroy the window, so it is safe to call once per
     * game-loop tick from a persistent window (needed for a mouse callback
     * registered on that window to keep working across frames).
     *
     * Throws if this Img was default-constructed (no window name set).
     */
    void show();

    /**
     * Returns a new Img holding a copy of the sub-rectangle
     * (x, y, width, height) of this image, clamped to its bounds. Used to
     * grab a cell-sized patch of a larger cached texture (e.g. the board
     * background) so it can be drawn back via draw_on to "erase" whatever
     * was previously painted on top of it, without redrawing the whole
     * canvas.
     */
    Img crop(int x, int y, int width, int height) const;

    /**
     * Get the underlying OpenCV Mat
     */
    const cv::Mat& get_mat() const { return img; }

    /**
     * Check if image is loaded
     */
    bool is_loaded() const { return !img.empty(); }

    /**
     * The window name this instance shows into, if any.
     */
    const std::string& getWindowName() const { return windowName; }

private:
    cv::Mat img;
    std::string windowName;
};