#include "layer.h"
#include "debug.h"
#include "protocol.pb.h"
#include <vector>

struct LayerResult {
  ILayer* layer;
  bool success;
};

class LayerDecoder {
  public:
  static LayerResult decode(_protocol_Layer incomingLayer, std::vector<u32_t>* effectSet) {
    ILayer* decodedLayer;

    switch (incomingLayer.type) {
      case protocol_LayerType_BlinkMask: {

        //decodedLayer = new BlinkMask(*effectSet, incomingLayer.p1);
        break;

      default:
        debug("Missing layerId %d", incomingLayer.type);
        return { nullptr, false };
      }
    }
    // Implementation of the decoding method
    return { decodedLayer, true };
  }
};

