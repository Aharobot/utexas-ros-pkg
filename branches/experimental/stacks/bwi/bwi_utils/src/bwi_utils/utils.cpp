#include "utils.h"

namespace bwi_utils {

  std::string frameIdFromLevelId(std::string level_id) {
    assert (level_id.find('/') == -1);
    return "/" + level_id + "/map";
  }

  std::string levelIdFromLevelFrameId(std::string frame_id) {
    std::vector<std::string> components;
    boost::split(components, frame_id, boost::is_any_of("/"));
    assert (components.size() == 3);
    assert (components[0] == "");
    assert (components[2] == "map");
    return components[1];
  }

}