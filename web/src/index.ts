import { protocol } from "./lib/protobuf/protocol.ts";
const { Direction, Sequence, LayerType, MyPack } = protocol;

const animation = {
  direction: Direction.BACKWARD,
  layers: [
    {
      type: 1,
      p1: 1,
      p2: 3,
      p3: 3,
      effect_set: [255 * 255 * 255, 255 * 255 * 255],
    },
    {
      type: 2,
      p1: 1,
      p2: 2,
      effect_set: [255, 255, 255],
    },
  ],
};

const layerRequest = Sequence.fromObject({
  brightness: 255,
  animations: [animation, animation],
});

const binary = layerRequest.serializeBinary();
console.log(binary);
