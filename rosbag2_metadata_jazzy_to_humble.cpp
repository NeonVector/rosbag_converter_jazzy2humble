/* ROS2 jazzy rosbags don't work in humble because line 'offered_qos_profiles:' in jazzy rosbags "metadata.yaml" has a different style from same line in humble "metadata.yaml".
** So... This file uses 'ROS2 QoS policies' to refactor rosbag jazzy "metadata.yaml" to humble "metadata.yaml".
*/

/* ROS2 QoS policies:
https://docs.ros.org/en/humble/Concepts/Intermediate/About-Quality-of-Service-Settings.html
*/

/* This code:
- changes 'offered_qos_profiles:' option (to humble style)
- deletes 'type_description_hash:' option (humble doesn't have it)
- changes 'custom_data:' option (humble doesn't have it)
- changes 'ros_distro:' option (humble doesn't have it)
*/

/* The correctness of "metadata.yaml" refactoring is generally not guaranteed!
This code was successfully tested on several jazzy rosbags.
Rosbag error like 'requesting incompatible QoS. No messages will be sent to it. Last incompatible policy: RELIABILITY_QOS_POLICY' tells that some code dictionary is incorrect.
The std::maps keywords may need to have another corresponding values.
*/

/* Run this code in rosbag "metadata.yaml" directory. */

#include <iostream>
#include <fstream>
#include <string>
#include <map>

// User parameter
const std::string out_file_name = "metadata_humble.yaml"; // name different from "metadata.yaml"

// ROS2 'QoS policies' dictionary (unconfident!!!)
std::map<std::string, std::string> history{{"keep_last", "1"}, {"keep_all", "2"}, {"unknown", "3"}};
std::map<std::string, std::string> reliability{{"best_effort", "2"}, {"reliable", "1"}};
std::map<std::string, std::string> durability{{"transient_local", "1"}, {"volatile", "2"}};
std::map<std::string, std::string> liveliness{{"automatic", "1"}, {"manual_by_topic", "2"}};

// Yaml hardcoded parameters
const std::string in_file_name = "metadata.yaml";
const std::string target_line = "        offered_qos_profiles:"; //'jazzy' yaml file line that needs to be changed
const int option_spaces_number = 12; // spaces number for 'offered_qos_profiles:' options
const int option_sec_number = 19;
const int option_nsec_number = 20;
const std::string start_options = " \"- ";
const std::string space_between_options = "\\n  ";
const std::string duration_space = "\\n    ";
const std::string end_options = "\"";

void change_option(std::string option, std::map<std::string, std::string> option_dictionary, std::fstream& yaml_file_in, std::string& new_target_line);
void change_option(std::string option, std::fstream& yaml_file_in, std::string& new_target_line);
void change_option(std::string option, std::fstream& yaml_file_in, std::string& new_target_line, bool duration);
void change_option(std::fstream& yaml_file_in, std::string& new_target_line);

int main() {

  std::fstream yaml_file_in;  // ROS2 'jazzy' rosbag yaml file
  std::fstream yaml_file_out;  // ROS2 'humble' rosbag yaml file

  // Open the file to perform read operation
  yaml_file_in.open(in_file_name, std::ios::in);
  // Open the file to perform a write operation
  yaml_file_out.open(out_file_name, std::ios::out);

  // Checking whether the files are opened
  if (! yaml_file_in.is_open() || ! yaml_file_out.is_open()) {
    return 0;
  }

  std::string in_line; // 'jazzy' yaml file current line
    
  // Reading from 'jazzy' yaml file by line and writing to 'humble' yaml file
  while (std::getline(yaml_file_in, in_line)) {

    // Catch line that needs to be changed
    if (in_line == target_line) {

      std::string new_target_line = target_line + start_options; //'humble' yaml file line

      // 'history:' option
      change_option("history: ", history, yaml_file_in, new_target_line);
      // 'depth:' option
      change_option("depth: ", yaml_file_in, new_target_line);
      // 'reliability:' option
      change_option("reliability: ", reliability, yaml_file_in, new_target_line);
      // 'durability:' option
      change_option("durability: ", durability, yaml_file_in, new_target_line);
      // 'deadline:' option
      change_option("deadline:", yaml_file_in, new_target_line, true);
      // 'lifespan:' option
      change_option("lifespan:", yaml_file_in, new_target_line, true);
      // 'liveliness:' option
      change_option("liveliness: ", liveliness, yaml_file_in, new_target_line);
      // 'liveliness_lease_duration:' option
      change_option("liveliness_lease_duration:", yaml_file_in, new_target_line, true);
      // 'avoid_ros_namespace_conventions:' option
      change_option(yaml_file_in, new_target_line);

      new_target_line += end_options;

      // Print the data of the string
      yaml_file_out << new_target_line << "\n";

      // Skip duplicate information if there is one
      // And skip 'type_description_hash:'
      std::getline(yaml_file_in, in_line);
      std::string duplicate_flag = "history: ";
      if (in_line.substr(option_spaces_number, duplicate_flag.size()) == duplicate_flag) {
        for (int k = 0; k < 15; ++k) {
          std::getline(yaml_file_in, in_line);
        }
      }

    } else {

      if (in_line == "  custom_data: ~") {
        in_line = "  custom_data: this yaml is refactored from jazzy style for compatibility with humble";
      }

      if (in_line == "  ros_distro: jazzy") {
        in_line = "  ros_distro: humble";
      }
        
      // Print the data of the string
      yaml_file_out << in_line << "\n";
    }

  }

  // Close the file object.
  yaml_file_in.close(); 
  yaml_file_out.close();
  
  std::cout << "Created rosbag humble \"" << out_file_name << "\"\n";
  return 0;
}

void change_option(std::string option, std::map<std::string, std::string> option_dictionary, std::fstream& yaml_file_in, std::string& new_target_line) {
  std::string line;
  std::getline(yaml_file_in, line);
  if (line.substr(option_spaces_number, option.size()) == option) {
    new_target_line += option + option_dictionary[line.substr(option_spaces_number + option.size())] + space_between_options;
  }
}

void change_option(std::string option, std::fstream& yaml_file_in, std::string& new_target_line) {
  std::string line;
  std::getline(yaml_file_in, line);
  if (line.substr(option_spaces_number, option.size()) == option) {
    new_target_line += option + line.substr(option_spaces_number + option.size()) + space_between_options;
  }
}

void change_option(std::string option, std::fstream& yaml_file_in, std::string& new_target_line, bool duration) {
  std::string line;
  std::getline(yaml_file_in, line);
  if (line.substr(option_spaces_number, option.size()) == option) {
    new_target_line += option + duration_space;
    std::getline(yaml_file_in, line);
    new_target_line += std::string("sec: ") + line.substr(option_sec_number) + duration_space;
    std::getline(yaml_file_in, line);
    new_target_line += std::string("nsec: ") + line.substr(option_nsec_number) + space_between_options;
  }
}

void change_option(std::fstream& yaml_file_in, std::string& new_target_line) {
  std::string line;
  std::getline(yaml_file_in, line);
  new_target_line += line.substr(option_spaces_number);
}