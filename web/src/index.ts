import { protocol } from "./lib/protobuf/protocol.ts";

const {
  Direction,
  Sequence,
  Animation,
  Layer,
  RainbowColor,
  SawtoothMask,
  SectionsMask,
  SingleColor,
} = protocol;

class Requester {
  sendSequence(sequence: protocol.Sequence) {
    console.log(sequence.serializeBinary());
  }
}

const layer = new Layer({
  sawtoothMask: new SawtoothMask({
    wavelength: 200,
    wavegap: 50,
    duration: 200,
  }),
});

new Layer({
  sawtoothMask: new SawtoothMask({
    wavelength: 200,
    wavegap: 50,
    duration: 200,
  }),
});

const animation = new Animation({
  direction: Direction.FORWARD,
  layers: [
    new Layer({
      rainbowColor: new RainbowColor({ duration: 200, length: 50 }),
    }),
    new Layer({
      sawtoothMask: new SawtoothMask({
        wavelength: 200,
        wavegap: 50,
        duration: 200,
      }),
    }),
  ],
});

const layerRequest = Sequence.fromObject({
  brightness: 200,
  animations: [animation, animation],
});

const binary = layerRequest.serializeBinary();
console.log(binary);
