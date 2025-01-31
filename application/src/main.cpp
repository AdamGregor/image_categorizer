#include "misc.h"

#include <thread>

int main(){
    auto opt_config = get_config("/application/resources/config.json");
    if(!opt_config)
        return 1;
    json& config {*opt_config};
        
    if(!check_config(config))
        return 1;
        
    std::string output_directory {config["output_folder"]};
    std::string input_directory {config["input_folder"]};
    
    init_output_dir(output_directory);
    
    while(1){
        json output_json =get_output_json(output_directory);

        if (! std::filesystem::exists(config["input_folder"])) {
            std::cerr << "\""<<config["input_folder"]<<"\" does not exist.\n";
            return 1;
        }

        for(const auto& entry : std::filesystem::directory_iterator(input_directory)){
            classes::Content img_content = classes::other;
            std::string path {fs_to_string(entry)};
            std::string file {std::find(path.rbegin(), path.rend(), '/').base(), path.end()};

            std::cout << "Processing" << path << "\n";
            
            auto last_dot {std::find(file.rbegin(), file.rend(), '.').base()};
            if (last_dot == file.begin())
            {
                img_content = classes::unsupported;
                move_file(path, img_content, output_json, output_directory);
                continue;
            }
            
            std::string extension {last_dot, file.end()};
            auto& extensions_supported {config["extensions_supported"]};
            if(std::find(extensions_supported.begin(), extensions_supported.end(), extension) == extensions_supported.end()) {
                std::cerr << "File has unsupported extension " << extension << "\n";
                img_content = classes::unsupported;
            }

            cv::Mat input_img {cv::imread(path)};
            if (input_img.empty()) {
                std::cerr << "Failed to load the image \n";
                img_content = classes::unsupported;
            }

            if(img_content == classes::unsupported){
                move_file(path, img_content, output_json, output_directory);
                continue;
            }

            bool cont_face {has_face(input_img.clone())};
            bool cont_text {has_text(input_img.clone())};
            
            if (cont_face && cont_text)
                img_content = classes::face_text;
            else
                img_content = static_cast<classes::Content>((cont_face * classes::face) + (cont_text * classes::text));
            std::string str_content = classes::content_arr[img_content];

            std::cout << "\t\t Image contains " << str_content << "\n"; 
        
            move_file(path, img_content, output_json, output_directory);
        }

        write_json(output_json, output_directory);
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}