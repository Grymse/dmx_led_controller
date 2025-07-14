<script setup lang="ts">
import { ref, computed } from 'vue';
import { Button } from "primevue";
import BluetoothMenu from "@/components/BluetoothMenu.vue";
import AnimationModule from "@/components/AnimationModule.vue";
import ColorEffectEditor from "@/components/ColorEffectEditor.vue";
import MaskEditor from "@/components/MaskEditor.vue";
import PrimeVue from 'primevue/config';

// Animation module data structure
interface Module {
  id: number;
  name: string;
  duration: number; // Module duration in milliseconds
  colorEffect: { type: string; [key: string]: any };
  mask1: { type: string; [key: string]: any };
  mask2: { type: string; [key: string]: any };
}

// Sample data - in real app, this might come from a store
const modules = ref<Module[]>([
  {
    id: 1,
    name: 'Rainbow Wave',
    duration: 2000, // 2 seconds
    colorEffect: { type: 'rainbow', duration: 50, length: 150 },
    mask1: { type: 'wave', duration: 200, length: 300, gap: 100 },
    mask2: { type: 'stars', frequency: 400, speed: 30, length: 1 }
  },
  {
    id: 2,
    name: 'Fade Pulse',
    duration: 5000, // 5 seconds
    colorEffect: {
      type: 'fade',
      duration: 300,
      colors: ['#FF0000', '#00FF00', '#0000FF']
    },
    mask1: { type: 'pulse', duration: 500 },
    mask2: { type: 'sawtooth', duration: 500, length: 300, gap: 150 }
  }
]);

// Track active module
const activeModuleId = ref<number | null>(modules.value.length > 0 ? modules.value[0].id : null);

// Module management functions
const selectModule = (id: number) => {
  activeModuleId.value = id;
};

const addModule = () => {
  const newId = modules.value.length > 0
    ? Math.max(...modules.value.map(m => m.id)) + 1
    : 1;

  const newModule = {
    id: newId,
    name: `New Module ${newId}`,
    duration: 2000, // Default to 2 seconds
    colorEffect: { type: 'single', color: '#FF0000' },
    mask1: { type: 'none' },
    mask2: { type: 'none' }
  };

  modules.value.push(newModule);
  activeModuleId.value = newId;
};

const removeModule = (id: number) => {
  const index = modules.value.findIndex(m => m.id === id);
  if (index !== -1) {
    modules.value.splice(index, 1);

    // Update active module if we removed the active one
    if (activeModuleId.value === id) {
      activeModuleId.value = modules.value.length > 0 ? modules.value[0].id : null;
    }
  }
};

// Update module duration
const updateModuleDuration = (id: number, duration: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex].duration = duration;
  }
};

// Get active module data
const activeModule = computed(() => {
  return modules.value.find(m => m.id === activeModuleId.value) || null;
});

// Update module properties
const updateColorEffect = (effect: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModule.value?.id);
  if (moduleIndex !== -1) {
    // Create a new object to ensure reactivity
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      colorEffect: { ...effect }
    };
  }
};

const updateMask1 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModule.value?.id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: { ...mask }
    };
  }
};

const updateMask2 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModule.value?.id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: { ...mask }
    };
  }
};

// Calculate total animation duration
const totalDuration = computed(() => {
  return modules.value.reduce((total, module) => total + module.duration, 0);
});
</script>

<template>
  <main class="w-screen h-screen flex flex-col">
    <!-- Fixed position container for BluetoothMenu -->
    <div class="fixed top-4 right-4 z-50">
      <BluetoothMenu />
    </div>

    <!-- Main centered container with 80% width -->
    <div class="w-4/5 mx-auto flex flex-col flex-grow my-4">
      <div class="flex flex-col bg-white rounded-lg shadow-md flex-grow overflow-hidden">
        <!-- Top Row with Animation Modules -->
        <div class="w-full py-4 bg-gray-100 border-b border-gray-200">
          <div class="px-6">
            <div class="flex items-center justify-between mb-3">
              <div class="text-2xl">DMX LED Controller</div>
              <div class="text-sm text-gray-600">
                Total Duration: {{ (totalDuration / 1000).toFixed(1) }}s
              </div>
            </div>

            <!-- Animation Modules -->
            <div class="flex flex-wrap gap-2 items-center">
              <AnimationModule
                v-for="module in modules"
                :key="`module-${module.id}-${JSON.stringify(module.colorEffect)}`"
                :id="module.id"
                :name="module.name"
                :duration="module.duration"
                :is-active="module.id === activeModuleId"
                :color-effect="module.colorEffect"
                @click="selectModule"
                @remove="removeModule"
                @update:duration="updateModuleDuration"
              />

              <!-- Add Module Button -->
              <button
                @click="addModule"
                class="w-8 h-8 bg-gray-200 hover:bg-gray-300 rounded-full flex items-center justify-center text-xl font-bold transition-colors"
              >
                +
              </button>
            </div>
          </div>
        </div>

        <!-- Three Columns Layout (fills remaining height) -->
        <div class="grid grid-cols-3 gap-6 p-6 flex-grow overflow-hidden">
          <!-- Color Effect Column -->
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full">
            <h2 class="text-xl font-semibold mb-4">Color Effect</h2>
            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <ColorEffectEditor
                  :effect="activeModule.colorEffect"
                  @update:effect="updateColorEffect"
                />
              </div>
              <div v-else class="text-gray-500 italic">
                Select or add a module to configure
              </div>
            </div>
          </div>

          <!-- Mask 1 Column -->
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full">
            <h2 class="text-xl font-semibold mb-4">Mask</h2>
            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <MaskEditor
                  :mask="activeModule.mask1"
                  title="Mask"
                  @update:mask="updateMask1"
                />
              </div>
              <div v-else class="text-gray-500 italic">
                Select or add a module to configure
              </div>
            </div>
          </div>

          <!-- Mask 2 Column -->
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full">
            <h2 class="text-xl font-semibold mb-4">Mask 2</h2>
            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <MaskEditor
                  :mask="activeModule.mask2"
                  title="Mask 2"
                  @update:mask="updateMask2"
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
