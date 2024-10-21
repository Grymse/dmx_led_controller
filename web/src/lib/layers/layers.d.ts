import { protocol } from "./lib/protobuf/protocol.ts";

interface SingleColor {
  type: protocol.LayerType.SingleColor;
  color: CRGB;
}

interface RainbowColor {
  type: protocol.LayerType.RainbowColor;
  p1: number;
  duration: number;
}

interface SectionsWaveColor {
  type: protocol.LayerType.SectionsWaveColor;
  colors: CRGB[];
  duration: number;
}

interface SectionsColor {
  type: protocol.LayerType.SectionsColor;
  colors: CRGB[];
  duration: number;
}

interface FadeColor {
  type: protocol.LayerType.FadeColor;
  colors: CRGB[];
  duration: number;
}

interface SwitchColor {
  type: protocol.LayerType.SwitchColor;
  colors: CRGB[];
  duration: number;
}

interface BlinkMask {
  type: protocol.LayerType.BlinkMask;
  mask: Uint8Array;
  duration: number;
}

interface InvertMask {
  type: protocol.LayerType.InvertMask;
}

interface PulseSawtoothMask {
  type: protocol.LayerType.PulseSawtoothMask;
  p1: number;
  duration: number;
}

interface PulseMask {
  type: protocol.LayerType.PulseMask;
  p1: number;
  duration: number;
}

interface SawtoothMask {
  type: protocol.LayerType.SawtoothMask;
  p1: number;
  p2: number;
  duration: number;
}

interface SectionsWaveMask {
  type: protocol.LayerType.SectionsWaveMask;
  mask: Uint8Array;
  duration: number;
}

interface SectionsMask {
  type: protocol.LayerType.SectionsMask;
  mask: Uint8Array;
  duration: number;
}

interface StarsMask {
  type: protocol.LayerType.StarsMask;
  p1: number;
  p2: number;
  duration: number;
}

interface WaveMask {
  type: protocol.LayerType.WaveMask;
  p1: number;
  p2: number;
  duration: number;
}

// Union type for all layer types
type Layer =
  | SingleColor
  | RainbowColor
  | SectionsWaveColor
  | SectionsColor
  | FadeColor
  | SwitchColor
  | BlinkMask
  | InvertMask
  | PulseSawtoothMask
  | PulseMask
  | SawtoothMask
  | SectionsWaveMask
  | SectionsMask
  | StarsMask
  | WaveMask;
