import { protocol } from "./protobuf/protocol.ts";
import { hexColorToUint32, msToTicks } from "./timeUtils.ts";
import type { Module } from "./module";


export function CRGB(r: number, g: number, b: number) {
  return 0 | (r << 16) | (g << 8) | b;
}

// Update the convertUIModulesToProtocol function to use the module's direction
export function convertUIModulesToProtocol(uiModules: Module[]): protocol.Sequence {

  const animations = uiModules.map(module => {
    // Create layers array from color effect and masks
    const layers: protocol.Layer[] = [];

    // Add color effect layer
    if (module.colorEffect) {
      const colorLayer = convertUILayerToProtocol(module.colorEffect, ModuleType.COLOR);
      if (colorLayer) layers.push(colorLayer);
    }

    // Add mask1 layer if present
    if (module.mask1) {
      const maskLayer = convertUILayerToProtocol(module.mask1, ModuleType.MASK);
      if (maskLayer) layers.push(maskLayer);
    }

    // Add mask2 layer if present
    if (module.mask2) {
      const maskLayer = convertUILayerToProtocol(module.mask2, ModuleType.MASK);
      if (maskLayer) layers.push(maskLayer);
    }

    return new protocol.Animation({
      direction: module.direction,
      duration: msToTicks(module.duration),
      brightness: 255,
      first_tick: 0,
      layers
    });
  });

  return new protocol.Sequence({
    animations
  })
}



const colorTypeToLayerType: Record<string, number> = {
  'single': 0,     // SingleColor
  'rainbow': 1,    // RainbowColor
  'sectionsWave': 2, // SectionsWaveColor
  'sections': 3,   // SectionsColor
  'fade': 4,       // FadeColor
  'switch': 5,     // SwitchColor
};


const maskTypeToLayerType: Record<string, number> = {
  'blink': 50,     // BlinkMask
  'invert': 51,    // InvertMask
  'pulseSawtooth': 52, // PulseSawtoothMask
  'pulse': 53,     // PulseMask
  'sawtooth': 54,  // SawtoothMask
  'sectionsWave': 55, // SectionsWaveMask
  'sections': 56,  // SectionsMask
  'stars': 57,     // StarsMask
  'wave': 58,      // WaveMask
};

enum ModuleType {
  COLOR,
  MASK
}

// Helper function to convert UI layer (color effect or mask) to protocol Layer
function convertUILayerToProtocol(uiLayer: { type: string; [key: string]: any }, type: ModuleType): protocol.Layer | null {
  // Get the protocol layer type
  const layerType = type === ModuleType.COLOR ? colorTypeToLayerType[uiLayer.type] : maskTypeToLayerType[uiLayer.type];
  if (layerType === undefined) return null;

  const layer = new protocol.Layer({
    type: layerType,
  })

  // Set common parameters based on UI layer
  if (uiLayer.duration !== undefined) layer.duration = uiLayer.duration;
  if (uiLayer.length !== undefined) layer.length = uiLayer.length;
  if (uiLayer.gap !== undefined) layer.gap = uiLayer.gap;
  if (uiLayer.frequency !== undefined) layer.frequency = uiLayer.frequency;
  if (uiLayer.speed !== undefined) layer.speed = uiLayer.speed;

  // Handle color for SingleColor
  if (uiLayer.type === 'single' && uiLayer.color) {
    layer.color = hexColorToUint32(uiLayer.color);
  }

  // Handle colors array for multi-color effects
  if (['fade', 'sections', 'sectionsWave', 'switch'].includes(uiLayer.type) && Array.isArray(uiLayer.colors)) {
    layer.colors = uiLayer.colors.map(hexColorToUint32);
  }

  // Handle sections for mask types that need it
  if (['blink', 'sectionsWave', 'sections'].includes(uiLayer.type) && Array.isArray(uiLayer.sections)) {
    layer.sections = new Uint8Array(uiLayer.sections);
  }

  return layer;
}
