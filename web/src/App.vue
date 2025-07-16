// App.vue
<template>
  <main class="w-screen h-screen flex flex-col bg-gray-100">
    <!-- Main centered container with 80% width -->
    <div class="w-4/5 mx-auto flex flex-col flex-grow mt-4">
      <div class="flex flex-col bg-white rounded-lg shadow-md flex-grow overflow-hidden">
        <!-- Module Sequence Header -->
        <div class="p-4 flex justify-between items-center">
          <div class="flex items-center">
            <h1 class="text-2xl font-bold">Sequence</h1>
            <span class="ml-4 px-3 py-1 bg-gray-100 rounded-full text-gray-700">
              Total Duration: {{ formatDuration(totalDuration) }}
            </span>
            <SaveLocally :currentSequence="modules" @updateSequence="handleSequenceUpdate" />
          </div>
          <!-- Control buttons (moved from BluetoothMenu) -->
          <div class="flex flex-row items-center gap-4">
            <SerialConnectButton />
            <!-- Connection Status Chip -->
            <div class="flex items-center gap-2">
              <span>Status:</span>
              <Badge :value="connectionStatus" class="text-white" :severity="serialStore.isConnected ? 'success' : 'danger'" />
            </div>
            <!-- Button Group for Controls -->
            <PlaySaveButtons ref="playSaveButtons" :currentSequence="currentSequence" @save="playAndSave" />
          </div>
        </div>

        <!-- Module Sequence Component -->
        <ModuleSequence
          :modules="modules"
          :activeModuleId="activeModuleId"
          @select="handleSelectModule"
          @remove="handleRemoveModule"
          @update:modules="handleUpdateModules"
          @update:duration="handleUpdateModuleDuration"
          @update:name="handleUpdateModuleName"
          @update:direction="handleUpdateDirection"
          @add="handleAddModule"
        />


        <!-- Three Columns Layout (fills remaining height) -->
        <div class="bg-gray-50 border border-indigo-400 bg-indigo-50 grid grid-cols-3 gap-0 p-6 flex-grow overflow-hidden rounded-lg">
          <!-- Color Effect Column -->
          <div
            class="col-span-1 bg-white shadow-md p-4 flex flex-col h-full transition-all duration-300 ease-in-out"
            :class="{ 'animate-editor': animateEditors }"
          >
            <h2 class="text-xl font-semibold mb-4">Color Effect</h2>
            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <ColorEffectEditor
                  :effect="activeModule.colorEffect"
                  @update:effect="handleUpdateColorEffect"
                />
              </div>
              <div v-else class="text-gray-500 italic">
                Select or add a module to configure
              </div>
            </div>
          </div>

          <!-- Mask 1 Column -->
          <div
            class="col-span-1 bg-white shadow-md p-4 flex flex-col h-full relative transition-all duration-300 ease-in-out"
            :class="{ 'animate-editor': animateEditors, 'animate-editor-delay-1': animateEditors }"
          >
            <div class="flex justify-between items-center mb-4">
              <h2 class="text-xl font-semibold">Mask</h2>
              <!-- Remove mask button (only show if mask exists) -->
              <button
                v-if="activeModule && activeModule.mask1"
                @click="handleRemoveMask1"
                class="w-6 h-6 rounded-full flex items-center justify-center text-sm bg-gray-100 text-gray-600 hover:bg-gray-200"
                title="Remove mask"
              >
                ×
              </button>
            </div>

            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <!-- Show add mask button if no mask -->
                <div v-if="!activeModule.mask1" class="flex flex-col items-center justify-center h-full">
                  <Button
                    @click="handleAddMask1"
                    icon="pi pi-plus"
                    rounded
                    severity="info"
                    aria-label="Add Mask"
                    class="w-12 h-12 !text-xl !flex !items-center !justify-center"
                  />


                  <span class="mt-2 text-gray-500">Add Mask</span>
                </div>
                <!-- Show mask editor if mask exists -->
                <MaskEditor
                  v-else
                  :mask="activeModule.mask1"
                  title="Mask"
                  @update:mask="handleUpdateMask1"
                />
              </div>
              <div v-else class="text-gray-500 italic">
                Select or add a module to configure
              </div>
            </div>
          </div>

          <!-- Mask 2 Column -->
          <div
            class="col-span-1 bg-white shadow-md p-4 flex flex-col h-full relative transition-all duration-300 ease-in-out"
            :class="{ 'animate-editor': animateEditors, 'animate-editor-delay-2': animateEditors }"
          >
            <div class="flex justify-between items-center mb-4">
              <h2 class="text-xl font-semibold">Mask 2</h2>
              <!-- Remove mask button (only show if mask exists) -->
              <button
                v-if="activeModule && activeModule.mask2"
                @click="handleRemoveMask2"
                class="w-6 h-6 rounded-full flex items-center justify-center text-sm bg-gray-100 text-gray-600 hover:bg-gray-200"
                title="Remove mask"
              >
                ×
              </button>
            </div>

            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <!-- Show add mask button if no mask -->
                <div v-if="!activeModule.mask2" class="flex flex-col items-center justify-center h-full">
                  <Button
                    @click="handleAddMask2"
                    icon="pi pi-plus"
                    rounded
                    severity="info"
                    aria-label="Add Mask"
                    class="w-12 h-12 !text-xl !flex !items-center !justify-center"
                  />
                  <span class="mt-2 text-gray-500">Add Mask</span>
                </div>
                <!-- Show mask editor if mask exists -->
                <MaskEditor
                  v-else
                  :mask="activeModule.mask2"
                  title="Mask 2"
                  @update:mask="handleUpdateMask2"
                />
              </div>
              <div v-else class="text-gray-500 italic">
                Select or add a module to configure
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </main>
</template>

<script setup lang="ts">

import { ref, computed, watch } from 'vue';
import ColorEffectEditor from "@/components/ColorEffectEditor.vue";
import MaskEditor from "@/components/MaskEditor.vue";
import ModuleSequence from "@/components/ModuleSequence.vue";
import Button from 'primevue/button';
import Badge from 'primevue/badge';
import type { Module } from './lib/module';
import { protocol } from './lib/protobuf/protocol';
import { convertUIModulesToProtocol } from './lib/serialiser';
import { useSerialStore } from './stores/serial.ts';
import SerialConnectButton from "@/components/SerialConnectButton.vue";
import PlaySaveButtons from './components/PlaySaveButtons.vue';
import SaveLocally from './components/SaveLocally.vue';

const {
  Direction,
  Sequence,
  Animation,
  Layer,
  Message
} = protocol;

const emit = defineEmits(['play']);

// Sample initial modules - with mask1 present and mask2 null by default
const modules = ref<Module[]>([
  {
    id: 1,
    name: 'Rainbow + Wave',
    duration: 2000,
    direction: Direction.FORWARD, // Add direction
    colorEffect: { type: 'rainbow', duration: 50, length: 150 },
    mask1: { type: 'wave', duration: 200, length: 300, gap: 100 },
    mask2: null
  },
  {
    id: 2,
    name: 'Fade + Pulse',
    duration: 5000,
    direction: Direction.FORWARD, // Add direction
    colorEffect: {
      type: 'fade',
      duration: 300,
      colors: ['#FF0000', '#00FF00', '#0000FF']
    },
    mask1: { type: 'pulse', duration: 500 },
    mask2: null
  }
]);

/* watch(modules, (newModules) => {
  console.log('Modules updated:', newModules);
}, { deep: true }); */

// Track active module
const activeModuleId = ref<number | null>(modules.value.length > 0 ? modules.value[0].id : null);
const previousActiveModuleId = ref<number | null>(null);

// Animation state
const animateEditors = ref(false);

// Get active module data
const activeModule = computed(() => {
  return modules.value.find(m => m.id === activeModuleId.value) || null;
});

// Calculate total sequence duration
const totalDuration = computed(() => {
  return modules.value.reduce((total, module) => total + module.duration, 0);
});

// Format duration in human-readable format
const formatDuration = (ms: number) => {
  const seconds = Math.floor(ms / 1000);
  const minutes = Math.floor(seconds / 60);

  if (minutes > 0) {
    return `${minutes}m ${seconds % 60}s`;
  }

  return `${seconds}s`;
};


// Update the currentSequence computed property
const currentSequence = computed<protocol.Sequence>(() => {
  return convertUIModulesToProtocol(modules.value);
});



// Handlers for module sequence events
// Handler for updating module duration
const handleUpdateModuleDuration = (id: number, duration: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      duration
    };
  }
};

const handleUpdateModules = (newModules: Module[]) => {
  modules.value = newModules;
};


// Handler for updating module name
const handleUpdateModuleName = (id: number, name: string) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      name
    };
  }
};


const handleSelectModule = (id: number) => {
  // Store previous active module for animation comparison
  previousActiveModuleId.value = activeModuleId.value;

  // Only trigger animation if we're selecting a different module
  if (id !== activeModuleId.value) {
    // Set the new active module ID
    activeModuleId.value = id;

    // Trigger animation
    animateEditors.value = true;

    // Reset animation flag after animation completes
    setTimeout(() => {
      animateEditors.value = false;
    }, 600); // Match this with the CSS animation duration
  }
};

// Add a handler for direction updates
const handleUpdateDirection = (id: number, direction: typeof Direction) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      // @ts-expect-error fucking typescript
      direction
    };
  }
};

// Update the handleAddModule function
const handleAddModule = () => {
  const newId = modules.value.length > 0
    ? Math.max(...modules.value.map(m => m.id)) + 1
    : 1;

  // Start with a basic color effect
  const colorEffect = { type: 'single', color: '#FF0000' };

  // Create a meaningful name based on the effect
  const name = 'Single Color';

  const newModule = {
    id: newId,
    name,
    duration: 2000,
    direction: Direction.FORWARD, // Set default direction
    colorEffect,
    mask1: null,
    mask2: null
  };

  // Store previous active module
  previousActiveModuleId.value = activeModuleId.value;

  // Add new module and select it
  modules.value.push(newModule);
  activeModuleId.value = newId;

  // Trigger animation for new module selection
  animateEditors.value = true;
  setTimeout(() => {
    animateEditors.value = false;
  }, 600);
};

// Update handlers for color effect and masks to update the module name
const updateModuleName = (moduleId: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === moduleId);
  if (moduleIndex === -1) return;

  const module = modules.value[moduleIndex];
  let name = '';

  // Add color effect name
  if (module.colorEffect) {
    const effectName = {
      'single': 'Single Color',
      'rainbow': 'Rainbow',
      'sectionsWave': 'Wave Sections',
      'sections': 'Sections',
      'fade': 'Fade',
      'switch': 'Switch'
    }[module.colorEffect.type] || module.colorEffect.type;

    name += effectName;
  }

  // Add mask1 name if present
  if (module.mask1) {
    const maskName = {
      'blink': 'Blink',
      'invert': 'Invert',
      'pulseSawtooth': 'Pulse Sawtooth',
      'pulse': 'Pulse',
      'sawtooth': 'Sawtooth',
      'sectionsWave': 'Sections Wave',
      'sections': 'Sections',
      'stars': 'Stars',
      'wave': 'Wave'
    }[module.mask1.type] || module.mask1.type;

    name += ' + ' + maskName;
  }

  // Add mask2 name if present
  if (module.mask2) {
    const maskName = {
      'blink': 'Blink',
      'invert': 'Invert',
      'pulseSawtooth': 'Pulse Sawtooth',
      'pulse': 'Pulse',
      'sawtooth': 'Sawtooth',
      'sectionsWave': 'Sections Wave',
      'sections': 'Sections',
      'stars': 'Stars',
      'wave': 'Wave'
    }[module.mask2.type] || module.mask2.type;

    name += ' + ' + maskName;
  }

  // Update the module name
  modules.value[moduleIndex] = {
    ...modules.value[moduleIndex],
    name
  };
};

// Update the handleUpdateColorEffect function
const handleUpdateColorEffect = (effect: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      colorEffect: { ...effect }
    };
    // Update name after changing the effect
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleUpdateMask1 function
const handleUpdateMask1 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: { ...mask }
    };
    // Update name after changing the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleUpdateMask2 function
const handleUpdateMask2 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: { ...mask }
    };
    // Update name after changing the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleAddMask1 function
const handleAddMask1 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: { type: 'blink', duration: 500 }
    };
    // Update name after adding the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleRemoveMask1 function
const handleRemoveMask1 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: null
    };
    // Update name after removing the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleAddMask2 function
const handleAddMask2 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: { type: 'blink', duration: 500 }
    };
    // Update name after adding the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleRemoveMask2 function
const handleRemoveMask2 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: null
    };
    // Update name after removing the mask
    updateModuleName(activeModuleId.value);
  }
};

// Update the handleUpdateDuration function
const handleUpdateDuration = (id: number, durationMs: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex].duration = durationMs; // Store as ms in UI
  }
};

// Connection status (basic implementation)
const serialStore = useSerialStore();
const connectionStatus = computed(() => serialStore.isConnected ? 'Connected' : 'Disconnected');
const chipClass = computed(() => serialStore.isConnected ? 'bg-green-500 text-white' : 'bg-red-500 text-white');

// Play/Save action methods
const play = () => {
  console.log('Play action triggered');
  console.log('Sequence:', currentSequence.value);
  console.log('Sequence serialized:', currentSequence.value.serializeBinary());
  // Implement your play logic here
};

const playAndSave = () => {
  console.log('Play and save action triggered');
  console.log('Sequence data:', currentSequence.value);
  // Implement your play and save logic here
};

const playSaveButtons = ref(null);
// For debugging: log the current sequence when it changes
watch(currentSequence, (newSequence) => {
  playSaveButtons.value?.handlePlay();
}, { deep: true });

// Handle removing a module
const handleRemoveModule = (id: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    // Store the current active module
    const wasActive = activeModuleId.value === id;

    // Remove the module
    modules.value.splice(moduleIndex, 1);

    // If we removed the last module, create a new default module
    if (modules.value.length === 0) {
      const newModule = {
        id: 1,
        name: 'Single Color',
        duration: 2000,
        colorEffect: { type: 'single', color: '#FF0000' },
        mask1: null,
        mask2: null
      };
      modules.value.push(newModule);
      activeModuleId.value = 1;

      // Trigger animation for the new module
      animateEditors.value = true;
      setTimeout(() => {
        animateEditors.value = false;
      }, 600);
    }
    // If we removed the active module, select another one
    else if (wasActive) {
      // Select previous module if possible, otherwise the first one
      const newIndex = Math.max(0, moduleIndex - 1);
      activeModuleId.value = modules.value[newIndex].id;

      // Trigger animation for the newly selected module
      animateEditors.value = true;
      setTimeout(() => {
        animateEditors.value = false;
      }, 600);
    }
  }
};

const handleSequenceUpdate = (sequenceData: any) => {
  modules.value = sequenceData;
  activeModuleId.value = sequenceData.length > 0 ? sequenceData[0].id : null;
};
</script>

<style scoped>
/* Animation for editors when switching modules */
@keyframes fadeInSlideUp {
  0% {
    opacity: 0.5;
    transform: translateY(10px) scale(0.98);
  }
  100% {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}

@keyframes highlight {
  0% {
    box-shadow: 0 0 0 2px rgba(99, 102, 241, 0);
  }
  30% {
    box-shadow: 0 0 0 4px rgba(99, 102, 241, 0.7);
  }
  100% {
    box-shadow: 0 0 0 2px rgba(99, 102, 241, 0);
  }
}

.animate-editor {
  animation: fadeInSlideUp 0.2s ease-out, highlight 0.5s ease;
}

.animate-editor-delay-1 {
  animation-delay: 0.1s;
}

.animate-editor-delay-2 {
  animation-delay: 0.2s;
}
</style>
