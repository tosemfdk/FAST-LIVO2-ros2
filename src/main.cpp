#include "LIVMapper.h"

#include <thread>

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto mapper = std::make_shared<LIVMapper>();

  std::thread processing_thread([mapper]() { mapper->run(); });

  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(mapper);
  exec.spin();

  rclcpp::shutdown();
  if (processing_thread.joinable()) {
    processing_thread.join();
  }
  return 0;
}
