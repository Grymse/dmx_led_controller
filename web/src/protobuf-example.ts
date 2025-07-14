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


function CRGB(r: number, g: number, b: number) {
  return 0 | (r << 16) | (g << 8) | b;
}

const animation = new Animation({
  /* direction: Direction.FORWARD, */
  layers: [
    /* new SingleColor(CRGB::Blue),
    new FadeColor({CRGB(0,0,255),CRGB(0,255,0) ,CRGB(255,0,0)}, 50),
    new SectionsWaveMask({255, 255, 125, 0}, 50), */
    
    new Layer({
      type: LayerType.SingleColor,
      color: CRGB(0, 0, 255),
    }),
    new Layer({
      type: LayerType.FadeColor,
      duration: 50,
      colors: [CRGB(0,0,255),CRGB(0,255,0),CRGB(255,0,0)]
    }),
    /* new Layer({
      type: LayerType.SectionsWaveMask,
      duration: 50,
      sections: new Uint8Array([255, 255, 125, 0])
    }), */
  ],
  duration: 2000,
});

const sequence = Sequence.fromObject({
  animations: [animation],
});

const settings = new Settings({
  group_id: 10,
  virtual_offset: 200,
});

const message = new Message({
  request_state: true,
});

const newBinary = message.serializeBinary();
console.log(newBinary);
console.log("Serialized message length:", newBinary.length);
// Example binary data for testing

// 8, 255, 1, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0
// 8, 255, 1, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0, 18, 37, 16, 208, 15, 26, 3, 32, 255, 1, 26, 15, 8, 4, 16, 50, 66, 9, 255, 1, 128, 254, 3, 128, 128, 252, 7, 26, 10, 8, 55, 16, 50, 74, 4, 255, 255, 125, 0,

/* const sequence = Sequence.deserializeBinary(newBinary);
const obj = sequence.toObject();
console.dir(obj, { depth: null }); */