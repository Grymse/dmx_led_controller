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
  SectionsColor,
} = protocol;

class Requester {
  sendSequence(sequence: protocol.Sequence) {
    console.log(sequence.serializeBinary());
  }
}
/* 
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
}); */

const animation = new Animation({
  direction: Direction.FORWARD,
  layers: [
    new Layer({
      sectionsMask: new SectionsMask({
        sections: [255, 0, 255, 0, 255],
        duration: 100,
      }),
    }),
  ],
});

const layerRequest = Sequence.fromObject({
  brightness: 200,
  animations: [animation],
});

const binary = layerRequest.serializeBinary();
console.log(binary);
