file(REMOVE_RECURSE
  "../msg_gen"
  "../src/CameraVideoTopic/msg"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/sensor_msgs_generate_messages_py.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
