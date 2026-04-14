cmake_minimum_required(VERSION 3.8)
project(assignment4)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
#find_package(orocos_bfl REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(message_filters REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(clearpath_common REQUIRED)
find_package(clearpath_sensors_description REQUIRED)
find_package(clearpath_simulator REQUIRED)

add_library( ekf SHARED src/ekf.cpp src/ekf_models.cpp src/measurement_pdf.cpp src/system_pdf.cpp src/utilities.cpp )
target_link_libraries( ekf orocos-bfl )
ament_target_dependencies( ekf rclcpp geometry_msgs sensor_msgs tf2 tf2_geometry_msgs )

add_executable( ekf_node src/ekf_node.cpp )
target_link_libraries( ekf_node ekf )
ament_target_dependencies( ekf_node rclcpp geometry_msgs sensor_msgs tf2 message_filters)

install( TARGETS ekf LIBRARY DESTINATION lib )
install( TARGETS ekf_node DESTINATION lib/${PROJECT_NAME} )
install( DIRECTORY launch DESTINATION share/${PROJECT_NAME} )
install( DIRECTORY config DESTINATION share/${PROJECT_NAME} )
install( DIRECTORY models DESTINATION share/${PROJECT_NAME} )
install( DIRECTORY worlds DESTINATION share/${PROJECT_NAME} )
install( DIRECTORY urdf DESTINATION share/${PROJECT_NAME} )

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
