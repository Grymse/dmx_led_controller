<script setup lang="ts">
import { ref, computed } from 'vue';
import BluetoothMenu from "@/components/BluetoothMenu.vue";
import ColorEffectEditor from "@/components/ColorEffectEditor.vue";
import MaskEditor from "@/components/MaskEditor.vue";
import ModuleSequence from "@/components/ModuleSequence.vue";

// Define the Module interface (could be moved to a types file)
interface Module {
  id: number;
  name: string;
  duration: number;
  colorEffect: { type: string; [key: string]: any };
  mask1: { type: string; [key: string]: any } | null;
  mask2: { type: string; [key: string]: any } | null;
}

// Sample initial modules - with mask1 present and mask2 null by default
const modules = ref<Module[]>([
  {
    id: 1,
    name: 'Rainbow Wave',
    duration: 2000,
    colorEffect: { type: 'rainbow', duration: 50, length: 150 },
    mask1: { type: 'wave', duration: 200, length: 300, gap: 100 },
    mask2: null
  },
  {
    id: 2,
    name: 'Fade Pulse',
    duration: 5000,
    colorEffect: {
      type: 'fade',
      duration: 300,
      colors: ['#FF0000', '#00FF00', '#0000FF']
    },
    mask1: { type: 'pulse', duration: 500 },
    mask2: null
  }
]);

// Track active module
const activeModuleId = ref<number | null>(modules.value.length > 0 ? modules.value[0].id : null);

// Get active module data
const activeModule = computed(() => {
  return modules.value.find(m => m.id === activeModuleId.value) || null;
});

// Handlers for module sequence events
const handleUpdateModules = (newModules: Module[]) => {
  modules.value = newModules;
};

const handleSelectModule = (id: number) => {
  activeModuleId.value = id;
};

const handleAddModule = () => {
  const newId = modules.value.length > 0
    ? Math.max(...modules.value.map(m => m.id)) + 1
    : 1;

  const newModule = {
    id: newId,
    name: `New Module ${newId}`,
    duration: 2000,
    colorEffect: { type: 'single', color: '#FF0000' },
    mask1: { type: 'none' }, // Default mask in first slot
    mask2: null  // No mask in second slot by default
  };

  modules.value.push(newModule);
  activeModuleId.value = newId;
};

const handleRemoveModule = (id: number) => {
  const index = modules.value.findIndex(m => m.id === id);
  if (index !== -1) {
    modules.value.splice(index, 1);

    // Update active module if we removed the active one
    if (activeModuleId.value === id) {
      activeModuleId.value = modules.value.length > 0 ? modules.value[0].id : null;
    }
  }
};

const handleUpdateDuration = (id: number, duration: number) => {
  const moduleIndex = modules.value.findIndex(m => m.id === id);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex].duration = duration;
  }
};

// Handlers for editor updates
const handleUpdateColorEffect = (effect: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      colorEffect: { ...effect }
    };
  }
};

const handleUpdateMask1 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: { ...mask }
    };
  }
};

const handleUpdateMask2 = (mask: any) => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: { ...mask }
    };
  }
};

// Add new handlers for adding/removing masks
const handleAddMask1 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: { type: 'none' } // Initialize with a default mask
    };
  }
};

const handleRemoveMask1 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask1: null
    };
  }
};

const handleAddMask2 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: { type: 'none' } // Initialize with a default mask
    };
  }
};

const handleRemoveMask2 = () => {
  if (!activeModule.value) return;

  const moduleIndex = modules.value.findIndex(m => m.id === activeModuleId.value);
  if (moduleIndex !== -1) {
    modules.value[moduleIndex] = {
      ...modules.value[moduleIndex],
      mask2: null
    };
  }
};
</script>

<template>
  <main class="w-screen h-screen flex flex-col">
    <!-- Fixed position container for BluetoothMenu -->
    <div class="fixed top-2 right-4 z-50">
      <BluetoothMenu />
    </div>

    <!-- Main centered container with 80% width -->
    <div class="w-4/5 mx-auto flex flex-col flex-grow mt-12">
      <div class="flex flex-col bg-white rounded-lg shadow-md flex-grow overflow-hidden">
        <!-- Module Sequence Component -->
        <ModuleSequence
          :modules="modules"
          :active-module-id="activeModuleId"
          @update:modules="handleUpdateModules"
          @update:duration="handleUpdateDuration"
          @select="handleSelectModule"
          @add="handleAddModule"
          @remove="handleRemoveModule"
        />

        <!-- Three Columns Layout (fills remaining height) -->
        <div class="grid grid-cols-3 gap-6 p-6 flex-grow overflow-hidden">
          <!-- Color Effect Column -->
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full">
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
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full relative">
            <div class="flex justify-between items-center mb-4">
              <h2 class="text-xl font-semibold">Mask</h2>
              <!-- Remove mask button (only show if mask exists) -->
              <button
                v-if="activeModule && activeModule.mask1"
                @click="handleRemoveMask1"
                class="w-6 h-6 rounded-full flex items-center justify-center text-sm bg-gray-100 text-gray-600 hover:bg-red-200"
                title="Remove mask"
              >
                ×
              </button>
            </div>

            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <!-- Show add mask button if no mask -->
                <div v-if="!activeModule.mask1" class="flex flex-col items-center justify-center h-full">
                  <button
                    @click="handleAddMask1"
                    class="bg-blue-500 hover:bg-blue-600 text-white rounded-full w-12 h-12 flex items-center justify-center text-xl font-bold"
                  >
                    +
                  </button>
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
          <div class="col-span-1 bg-white rounded-lg shadow-md p-4 flex flex-col h-full relative">
            <div class="flex justify-between items-center mb-4">
              <h2 class="text-xl font-semibold">Mask 2</h2>
              <!-- Remove mask button (only show if mask exists) -->
              <button
                v-if="activeModule && activeModule.mask2"
                @click="handleRemoveMask2"
                class="w-6 h-6 rounded-full flex items-center justify-center text-sm bg-gray-100 text-gray-600 hover:bg-red-200"
                title="Remove mask"
              >
                ×
              </button>
            </div>

            <div class="flex-grow overflow-auto">
              <div v-if="activeModule">
                <!-- Show add mask button if no mask -->
                <div v-if="!activeModule.mask2" class="flex flex-col items-center justify-center h-full">
                  <button
                    @click="handleAddMask2"
                    class="bg-blue-500 hover:bg-blue-600 text-white rounded-full w-12 h-12 flex items-center justify-center text-xl font-bold"
                  >
                    +
                  </button>
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
