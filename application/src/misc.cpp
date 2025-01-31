#include "misc.h"

#include <filesystem>
#include <algorithm>
#include <exception>
#include <iostream>
#include <optional>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <array>

bool has_text(cv::Mat&& input)
{
    static bool fst {true};
    static cv::dnn::dnn4_v20241223::Net nn;
    if (fst)
        nn = cv::dnn::readNet("/application/resources/frozen_east_text_detection.pb");
    fst = false;

    auto blob = cv::dnn::blobFromImage(input, 1.0, cv::Size(320, 320), cv::Scalar(123.68, 116.78, 103.94), true, false);
    nn.setInput(blob);
    
    std::vector<std::string> outputLayers {"feature_fusion/Conv_7/Sigmoid", "feature_fusion/concat_3"};
    std::vector<cv::Mat> outputs;
    nn.forward(outputs, outputLayers);
    cv::Mat& scores = outputs[0];

    int rows {scores.size[2]};
    int cols {scores.size[3]};

    std::vector<float> confs;
    for (int y = 0; y < rows; y++) {
        const float* scoresData {scores.ptr<float>(0, 0, y)};
        for (int x = 0; x < cols; x++) {
            float score = scoresData[x];
            confs.push_back(score);
        }
    }

    float max_score {*std::max_element(confs.begin(), confs.end())};
    return max_score > 0.998;
}


bool has_face(cv::Mat&& input)
{
    static bool fst {true};
    static cv::CascadeClassifier faces_detector;
    if (fst) 
        faces_detector.load("/application/resources/haarcascade_frontalface_default.xml");
    fst = false;
    cv::cvtColor(input, input, cv::COLOR_BGR2GRAY);
    std::vector<cv::Rect> faces;

    faces_detector.detectMultiScale(input, faces, 1.1, 
                            3, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    return faces.size() != 0;
}

void move_file(const std::string& path, classes::Content content, [[maybe_unused]]json& out_json, const std::string& output_path)
{
    std::string file_name {std::find(path.rbegin(), path.rend(), '/').base(), path.end()};

    std::string output_section = classes::content_arr[content];


    //   for (auto& el : out_json.items())
    // {
    //   std::cout << el.value() << '\n';
    // }
    // std::cout << out_json.dump(2) << "\n";
    // out_json[output_section].push_back(file_name);


    auto dst {output_path + "/" + output_section};

    std::filesystem::copy(path, dst);
    std::filesystem::remove(path);
}

bool check_config_key(const json& config, std::string_view key)
{
    if(config[key].is_null()) {
        std::cerr << "Config.json is misssing '" << key << "' field.\n";
        return false;
    }

    return true;
}

bool check_config(const json& config)
{
    return  check_config_key(config, "extensions_supported") &&
            check_config_key(config, "output_folder") &&
            check_config_key(config, "input_folder");
}

std::optional<json> get_config(std::string_view path)
{
    json config;
    try {
        std::ifstream config_raw {static_cast<std::string>(path)};
        config = json::parse(config_raw);
    }
    catch(std::exception& e) {
        std::cerr << "Failed to load the config.json at " << path << "\n";
        return {};
    }

    return config;
}

void init_output_dir(const std::string& base_path)
{
    for (auto& el : classes::content_arr) {
        if(! std::filesystem::exists(base_path + "/" + el))
            std::filesystem::create_directories(base_path + "/" + el);
    }
}

json get_output_json(const std::string& path)
{
    json output_json;
    if(std::filesystem::exists(path + "/meta.json") && !std::filesystem::is_empty(path + "/meta.json") ) {
        std::ifstream output_i {path + "/meta.json"};
        output_json = json::parse(output_i);
        output_i.close();
    }
    else{
        for(const auto& el : classes::content_arr)
            output_json[el] = json::array();
    }
    return output_json;
}

void write_json(const json& write, const std::string& directory)
{
    std::ofstream output_o{directory + "/meta.json"};
    output_o << write.dump(4);       
    output_o.close();
}
