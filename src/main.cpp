#include <memory>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <functional>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float32_multi_array.hpp>
#include <std_msgs/msg/u_int8_multi_array.hpp>

class float_to_uint8_converter: public rclcpp::Node {
	rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr pos_sub;
	rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr uart_pub;

	void sub_callback(std_msgs::msg::Float32MultiArray::SharedPtr recv_msg) {
		std_msgs::msg::UInt8MultiArray pub_msg;
		for(auto a : recv_msg->data) {
			uint32_t tmp = *reinterpret_cast<uint32_t*>(&a);
			
			std::vector<uint8_t> tmp_vec = {static_cast<uint8_t>(tmp >> 24), static_cast<uint8_t>(tmp >> 16), static_cast<uint8_t>(tmp >> 8), static_cast<uint8_t>(tmp)};
			pub_msg.data.insert(pub_msg.data.end(), tmp_vec.begin(), tmp_vec.end());
		}

		uart_pub->publish(pub_msg);
	}

public:
	float_to_uint8_converter(std::string node_name = "float_to_uint_converter") : Node(node_name) {
		pos_sub = this->create_subscription<std_msgs::msg::Float32MultiArray>(
			"float_msg", 10, std::bind(&float_to_uint8_converter::sub_callback, this, std::placeholders::_1));
		uart_pub = this->create_publisher<std_msgs::msg::UInt8MultiArray>("uint_msg", 10);
	}
};

int main(int argc, char ** argv) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<float_to_uint8_converter>());
	rclcpp::shutdown();
}
