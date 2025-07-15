import { protocol } from "./lib/protobuf/protocol.ts";

const {
  Direction,
  BroadcastSequence,
  Settings,
  State,
  Sequence,
  Animation,
  Layer,
  LayerType,
  Message
} = protocol;

export function CRGB(r: number, g: number, b: number) {
  return 0 | (r << 16) | (g << 8) | b;
}

const animation = new Animation({
  direction: Direction.FORWARD,
  layers: [
    new Layer({
      type: LayerType.SingleColor,
      color: CRGB(0, 255, 0),
    }),
    new Layer({
      type: LayerType.SwitchColor,
      colors: [
        CRGB(255, 0, 0),
        CRGB(0, 255, 0),
        CRGB(0, 0, 255),
      ],
      duration: 50,
    }),
  ],
  duration: 2000,
  first_tick: 0,
  brightness: 255,
});

const sequence = Sequence.fromObject({
  animations: [animation],
});

new Message({
  sequence,
}).serializeBinary();

const settings = new Settings({
  group_id: 10,
  virtual_offset: 200,
});

