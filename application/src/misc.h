#pragma once
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <nlohmann/json.hpp>
#include <string_view>




using json = nlohmann::json;
enum Content{other, face, text, face_text, unsupported, n_options};
constexpr std::array content_arr{"other", "face", "text", "face_and_text", "unsupported"};
const std::string INPUT_PATH {"/application/input"};
static_assert(n_options == content_arr.size(), "Missing Content-to-string data!");

std::string fs_to_string(auto fs)
{
    return fs.path().generic_string();
}

void move_file(const std::string& path, Content content, json& out_json, const std::string& output_path);
bool check_config_key(const json& config, std::string_view key);
void write_json(const json& json, const std::string& directory);
std::optional<json> get_config(std::string_view path);
void init_output_dir(const std::string& base_path);
json get_output_json(const std::string& path);
bool check_config(const json& config);
bool has_text(cv::Mat&& input);
bool has_face(cv::Mat&& input);