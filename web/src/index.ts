import { protocol } from "./lib/protobuf/protocol.ts";
const { Direction, Sequence } = protocol;

const animation = {
  direction: Direction.BACKWARD,
  layers: [
    {
      effect: 1,
      p1: 1,
      p2: 3,
      p3: 3,
      effect_set: [255 * 255 * 255, 255 * 255 * 255],
    },
    {
      effect: 1,
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
