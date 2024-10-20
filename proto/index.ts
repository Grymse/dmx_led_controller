import { led_protocols } from "./dist/protocol.ts";
const { Direction, Layer, SetLayersRequest } = led_protocols;

const layerRequest = SetLayersRequest.fromObject({
  direction: Direction.FORWARD,
  brightness: 255,
  layers: [
    {
      effect_p1: 1,
      p2_p3: 2,
      effect_set: [1, 4, 2147483647, 2147483647],
    },
  ],
});

const binary = layerRequest.serializeBinary();

console.log(SetLayersRequest.deserializeBinary(binary).toObject());
