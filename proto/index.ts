import { led_protocols } from "./dist/protocol.ts";
const { Direction, Layer, Sequence } = led_protocols;

const animation = {
  direction: Direction.BACKWARD,
  layers: [
    {
      effect: 1,
      p1: 1,
      p2: 2,
      p3: 3,
      effect_set: [
        255 * 255 * 255,
        255 * 255 * 255,
        255 * 255 * 255,
        255 * 255 * 255,
        255 * 255 * 255,
        255 * 255 * 255,
      ],
    },
    {
      effect: 1,
      p1: 1,
      p2: 2,
      p3: 3,
      effect_set: [255, 255, 255],
    },
    {
      effect: 1,
      p1: 1,
      p2: 2,
      p3: 3,
      effect_set: [255, 255, 255],
    },
  ],
};

const layerRequest = Sequence.fromObject({
  brightness: 255,
  animations: [animation, animation, animation, animation],
});

const binary = layerRequest.serializeBinary();
