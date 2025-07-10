




# DMX LED Controller

A sophisticated ESP32-based LED animation framework designed for creating custom lighting solutions. Built around a simple **layered animation system** that allows users to combine multiple effects to create complex, professional-quality lighting displays. The LED updates at 40hz providing smooth animations.

## 🚀 Project Status

**⚠️ Experimental** - This project is actively under development with multiple working prototypes. The platform shows great promise but still requires significant development before production use.

## ✨ Key Features

### 🎨 Layered Animation System
Create complex lighting effects by combining simple layers:
- **Color Layers** (6 types): Define base colors and gradients
- **Mask Layers** (8 types): Apply effects and modifications
- **Layer Composition**: Mix and match for infinite possibilities

**Example combinations:**
- `RainbowColor + WaveMask + StarsMask` → Rainbow wave with twinkling stars
- `FadeColor + PulseSawtoothMask + SawtoothMask` → Pulsing color transitions with sawtooth patterns

### 🌐 Wireless Network Control
- **RF24 (nRF24L01)** radio communication between controllers
- **Group-based targeting** - Control specific zones independently
- **Takeover functionality** - Override standalone mode for live control
- **Virtual LED indexing** - Chain multiple strips seamlessly

### 📡 Multiple Control Interfaces
- **Web Interface** - Real-time live control and preset programming
- **DMX Protocol** - Professional lighting integration (planned)
- **Standalone Mode** - Run programmed sequences independently

### ⚡ High Performance
- **FastLED** Great LED-Strip communication library
- **Multi-process scheduler** for concurrent operations
- **Protocol Buffers** for ultra-compact wireless transmission (<250 bytes)
- **Sub-25ms animation updates** for smooth effects

## 🏗️ Architecture

### Core Components

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Web Interface │    │   ESP32 Firmware │    │  LED Hardware   │
│                 │    │                  │    │                 │
│ ┌─────────────┐ │    │ ┌──────────────┐ │    │ ┌─────────────┐ │
│ │ TypeScript  │ │    │ |   Animator   │ │    │ │ WS2811 LEDs │ │
│ │ Layer Editor│ │    │ │              │ │    │ │   (300x)    │ │
│ └─────────────┘ │    │ └──────────────┘ │    │ └─────────────┘ │
│                 │    │ ┌──────────────┐ │    │                 │
│ ┌─────────────┐ │    │ │ Layer System │ │    │                 │
│ │ Live Control│ │    │ │ - Colors (6) │ │    │                 │
│ │ Interface   │ │    │ │ - Masks (8)  │ │    │                 │
│ └─────────────┘ │    │ └──────────────┘ │    │                 │
└─────────────────┘    │ ┌──────────────┐ │    └─────────────────┘
                       │ │ RF24 Network │ │
                       │ │ Scheduler    │ │
                       │ └──────────────┘ │
                       └──────────────────┘
```

### Layer System

**Color Layers:**
- `SingleColor` - Solid color
- `RainbowColor` - HSV rainbow animation
- `FadeColor` - Smooth color transitions
- `SectionsColor` - Color sections/zones
- `SectionsWaveColor` - Wave-based color sections
- `SwitchColor` - Switching between colors

**Mask Layers:**
- `BlinkMask` - Blinking effects
- `InvertMask` - Color inversion
- `PulseMask` - Pulse effects
- `SawtoothMask` - Sawtooth wave patterns
- `StarsMask` - Twinkling star effects
- `WaveMask` - Wave animations
- `SectionsWaveMask` - Wave-based masking sections
- `PulseSawtoothMask` - Combined pulse and sawtooth

## 🛠️ Hardware Requirements

### Current Platform Specifications
- **MCU**: ESP32-C3 Super Mini
- **LED Controller**: WS2811 strips (12V, up to 300 LEDs)
- **Radio**: nRF24L01 module
- **Power**: 12V supply (LED strip dependent)

### Planned Hardware Support
*Documentation for custom hardware configurations coming soon.*

## 🔧 Development Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) IDE or CLI
- [Node.js](https://nodejs.org/) (for web interface development)
- ESP32-C3 development board
- nRF24L01 radio module

### Building the Firmware

```bash
src/ Primarily code for lightcontroller
lib/ Libraries for lightcontroller
proto/ Protobuf protocol. Compiles into C and TypeScript
web/ Web interface
```

```bash
# Clone the repository
git clone <repository-url>
cd dmx_led_controller

# Build and upload firmware
pio run --target upload

# Monitor serial output
pio device monitor
```

### Web Interface Development

```bash
# Navigate to web directory
cd web

# Install dependencies
npm install

# Start development server
npm run dev
```

## 📋 Protocol Buffer Schema

The communication protocol supports efficient wireless transmission:

```protobuf
message Layer {
  LayerType type = 1;
  uint32 duration = 2;
  uint32 length = 3;
  uint32 color = 4;
  uint32 gap = 5;
  uint32 frequency = 6;
  uint32 speed = 7;
  repeated uint32 colors = 8;
  bytes sections = 9;
}

message Animation {
  Direction direction = 1;
  uint32 duration = 2;
  repeated Layer layers = 3;
}

message Sequence {
  uint32 brightness = 1;
  repeated Animation animations = 2;
}
```

## 🎯 Roadmap

### ✅ Completed Features
- [x] Layer-based animation system (6 color + 8 mask layers)
- [x] Protocol buffer communication protocol
- [x] Multi-process scheduler
- [x] Virtual LED indexing
- [x] Basic web interface structure
- [x] RF24 wireless communication

### 🔄 In Progress
- [ ] Web interface layer editor
- [ ] Group management system
- [ ] Bluetooth connectivity
- [ ] Flash storage for sequences

### 📅 Planned Features
- [ ] DMX protocol integration
- [ ] Live control interface
- [ ] Preset management system
- [ ] Advanced radio protocol (multi-packet support)
- [ ] Additional layer types
- [ ] Hardware documentation

## 🎮 Usage Examples

### Creating a Layer Composition

```cpp
// C++ Firmware
sequenceScheduler->add({
  new FadeColor({CRGB::Red, CRGB::Blue}, 1200),
  new SawtoothMask(50, 0, 300),
  new StarsMask(300, 50, 3),
}, 10000);
```

```typescript
// TypeScript Web Interface
const animation = new Animation({
  layers: [
    new Layer({
      type: LayerType.FadeColor,
      colors: [CRGB(255,0,0), CRGB(0,0,255)],
      duration: 1200
    }),
    new Layer({
      type: LayerType.SawtoothMask,
      duration: 50,
      length: 300
    }),
    new Layer({
      type: LayerType.StarsMask,
      frequency: 300,
      speed: 50,
      length: 3
    })
  ]
});
```

## 🎨 Use Cases

- **Custom Lamps** - Build unique lighting fixtures with programmable effects
- **Event Lighting** - Professional-grade lighting control for events and performances  
- **Architectural Lighting** - Permanent installations with dynamic lighting
- **Interactive Art** - Responsive lighting installations
- **Stage Design** - Concert and theater lighting effects
- **DIY Projects** - Hobbyist lighting projects with professional capabilities

## 🤝 Contributing

This project welcomes contributions! Areas where help is particularly needed:

- **Web Interface Development** - TypeScript/React components
- **DMX Protocol Implementation** - Professional lighting integration
- **Hardware Documentation** - Setup guides and schematics
- **Layer Development** - New animation effects
- **Testing** - Hardware compatibility and performance testing

## 📄 License

*License information to be added*

---

## 🔍 Debug Output Example

```
Layer: RainbowColor: d: 50.00, l: 150.00
Layer: WaveMask: d: 200, l: 300, g: 100
Layer: StarsMask: f: 400, s: 30, l: 1
Animator took 21ms

Layer: FadeColor: d: 300, c: [#FF0000, #00FF00, #0000FF, #FFFF00, #FF00FF, #00FFFF]
Layer: WaveMask: d: 300, l: 1200, g: 100
Layer: SawtoothMask: d: 500, l: 300, g: 150
Animator took 29ms
```

*Performance metrics showing sub-30ms animation updates for smooth, real-time effects.*