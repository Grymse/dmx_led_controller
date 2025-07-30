<script setup lang="ts">
import { ref, computed, watch } from 'vue';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import Dropdown from 'primevue/dropdown';
import { convertTimeValues } from '@/lib/timeUtils';
import SelectButton from 'primevue/selectbutton';

// Constants for duration bounds (in ms)
const MIN_DURATION_MS = 175; // 7 ticks * 25ms
const MAX_DURATION_MS = 50000; // 2000 ticks * 25ms

const props = defineProps<{
  mask: { type: string; [key: string]: any };
  title: string;
}>();

const emit = defineEmits(['update:mask']);

// Create a local copy of the mask with ms values for the UI
const uiMask = ref<Record<string, any>>({});

// Watch for changes in the mask prop (which contains tick values)
watch(() => props.mask, (newMask) => {
  // Convert incoming tick values to ms for the UI
  if (newMask) {
    uiMask.value = convertTimeValues(newMask, false);
  }
}, { immediate: true, deep: true });

// Handle updates to the mask
const updateMask = (updates: any) => {
  // Update the local UI version
  uiMask.value = { ...uiMask.value, ...updates };

  // Convert from ms back to ticks for the actual data model
  const tickUpdates = convertTimeValues(updates, true);
  emit('update:mask', { ...props.mask, ...tickUpdates });
};

const maskConfigs: {
  [key: string]: {
    label: string;
    icon: string;
    id: number;
    params: {
      key: string;
      type: string;
      default: number;
      min?: number;
      max?: number;
      step?: number;
      label: string;
      unit?: string;
      required: boolean;
      options?: { label: string; value: number }[];
    }[];
  };
} = {
  'blink': {
    label: 'Blink',
    icon: 'pi pi-eye',
    id: 50, // BlinkMask = 50
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'numSections', type: 'dropdown', default: 4, options: [
          { label: '2', value: 2 },
          { label: '3', value: 3 },
          { label: '4', value: 4 },
          { label: '5', value: 5 },
          { label: '6', value: 6 },
          { label: '7', value: 7 },
          { label: '8', value: 8 }
        ], label: 'Number of Sections', required: true },
      ...Array.from({ length: 8 }, (_, i) => ({
        key: `intensity${i + 1}`,
        type: 'selectbutton',
        default: 255,
        options: [
          { label: '0%', value: 0 },
          { label: '25%', value: 63 },
          { label: '50%', value: 127 },
          { label: '75%', value: 191 },
          { label: '100%', value: 255 }
        ],
        label: `Section ${i + 1} Intensity`,
        required: true
      }))
    ]
  },
  'invert': {
    label: 'Invert',
    icon: 'pi pi-arrows-h',
    id: 51, // InvertMask = 51
    params: [] // No parameters needed for invert
  },
  'pulseSawtooth': {
    label: 'Pulse Sawtooth',
    icon: 'pi pi-chart-line',
    id: 52, // PulseSawtoothMask = 52
    params: [
      { key: 'gap', type: 'slider', default: 150, min: 0, max: 500, step: 5, label: 'Gap', unit: 'ms', required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'pulse': {
    label: 'Pulse',
    icon: 'pi pi-heart',
    id: 53, // PulseMask = 53
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'gap', type: 'slider', default: 150, min: 0, max: 500, step: 5, label: 'Gap', unit: 'ms', required: true },
    ]
  },
  'sawtooth': {
    label: 'Sawtooth',
    icon: 'pi pi-chart-bar',
    id: 54, // SawtoothMask = 54
    params: [
      { key: 'length', type: 'slider', default: 300, min: 1, max: 500, step: 5, label: 'Length', required: true },
      { key: 'gap', type: 'slider', default: 150, min: 0, max: 500, step: 5, label: 'Gap', required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sectionsWave': {
    label: 'Sections Wave',
    icon: 'pi pi-sliders-h',
    id: 55, // SectionsWaveMask = 55
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'numSections', type: 'dropdown', default: 4, options: [
          { label: '2', value: 2 },
          { label: '3', value: 3 },
          { label: '4', value: 4 },
          { label: '5', value: 5 },
          { label: '6', value: 6 },
          { label: '7', value: 7 },
          { label: '8', value: 8 }
        ], label: 'Number of Sections', required: true },
      ...Array.from({ length: 8 }, (_, i) => ({
        key: `intensity${i + 1}`,
        type: 'selectbutton',
        default: 255,
        options: [
          { label: '0%', value: 0 },
          { label: '25%', value: 63 },
          { label: '50%', value: 127 },
          { label: '75%', value: 191 },
          { label: '100%', value: 255 }
        ],
        label: `Section ${i + 1} Intensity`,
        required: true
      }))
    ]
  },
  'sections': {
    label: 'Sections',
    icon: 'pi pi-table',
    id: 56, // SectionsMask = 56
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'numSections', type: 'dropdown', default: 4, options: [
          { label: '2', value: 2 },
          { label: '3', value: 3 },
          { label: '4', value: 4 },
          { label: '5', value: 5 },
          { label: '6', value: 6 },
          { label: '7', value: 7 },
          { label: '8', value: 8 }
        ], label: 'Number of Sections', required: true },
      ...Array.from({ length: 8 }, (_, i) => ({
        key: `intensity${i + 1}`,
        type: 'selectbutton',
        default: 255,
        options: [
          { label: '0%', value: 0 },
          { label: '25%', value: 63 },
          { label: '50%', value: 127 },
          { label: '75%', value: 191 },
          { label: '100%', value: 255 }
        ],
        label: `Section ${i + 1} Intensity`,
        required: true
      }))
    ]
  },
  'stars': {
    label: 'Stars',
    icon: 'pi pi-star',
    id: 57, // StarsMask = 57
    params: [
      { key: 'frequency', type: 'slider', default: 100, min: 1, max: 1000, step: 10, label: 'Frequency', required: true },
      { key: 'speed', type: 'slider', default: 30, min: 1, max: 100, step: 1, label: 'Speed', required: true },
      { key: 'length', type: 'slider', default: 1, min: 1, max: 10, step: 1, label: 'Length', required: true }
    ]
  },
  'wave': {
    label: 'Wave',
    icon: 'pi pi-wave-pulse',
    id: 58, // WaveMask = 58
    params: [
      { key: 'length', type: 'slider', default: 100, min: 1, max: 500, step: 5, label: 'Length', required: true },
      { key: 'gap', type: 'slider', default: 0, min: 0, max: 500, step: 5, label: 'Gap', required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  }
};
// Generate maskTypes array for select dropdown
const maskTypes = Object.entries(maskConfigs).map(([value, config]) => ({
  value,
  label: config.label,
  icon: config.icon
}));

// Get current mask configuration
const currentMaskConfig = computed(() => {
  return maskConfigs[uiMask.value.type] || null;
});

// Ensure mask has required parameters when type changes
const initializeMask = () => {
  const config = currentMaskConfig.value;
  if (!config) return;

  const updates: Record<string, any> = {};
  let hasUpdates = false;

  config.params.forEach(param => {
    // Check if parameter exists and set to default if not
    if (uiMask.value[param.key] === undefined) {
      updates[param.key] = param.default;
      hasUpdates = true;
    }
    // Ensure duration is within bounds
    else if (param.key === 'duration') {
      if (uiMask.value.duration < MIN_DURATION_MS) {
        updates.duration = MIN_DURATION_MS;
        hasUpdates = true;
      } else if (uiMask.value.duration > MAX_DURATION_MS) {
        updates.duration = MAX_DURATION_MS;
        hasUpdates = true;
      }
    }
  });

  if (hasUpdates) {
    updateMask(updates);
  }
};

// Watch for mask type changes to initialize properties
watch(() => uiMask.value.type, () => {
  initializeMask();
}, { immediate: true });

// Handle dropdown change
const handleDropdownChange = (event: any) => {
  updateMask({ type: event.value });
};

function getIconForParam(paramKey: string) {
  switch (paramKey) {
    case 'duration':
      return 'pi pi-clock';
    case 'length':
      return 'pi pi-arrows-h';
    case 'gap':
      return 'pi pi-align-justify';
    case 'frequency':
      return 'pi pi-chart-line';
    case 'speed':
      return 'pi pi-bolt';
    default:
      return 'pi pi-sliders-v';
  }
}
</script>
<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Mask Type</label>
      <Dropdown
        v-model="uiMask.type"
        :options="maskTypes"
        optionLabel="label"
        optionValue="value"
        placeholder="Select a Mask"
        class="w-full"
        @change="handleDropdownChange"
      >
        <template #value="slotProps">
          <div v-if="slotProps.value" class="flex items-center gap-2">
            <i :class="maskConfigs[slotProps.value].icon" style="font-size: 1rem"></i>
            <div>{{ maskConfigs[slotProps.value].label }}</div>
          </div>
          <span v-else>Select a Mask</span>
        </template>
        <template #option="slotProps">
          <div class="flex items-center gap-2">
            <i :class="slotProps.option.icon" style="font-size: 1rem"></i>
            <div>{{ slotProps.option.label }}</div>
          </div>
        </template>
      </Dropdown>
    </div>

    <!-- Dynamic parameters based on mask type -->
    <div v-if="currentMaskConfig" class="mb-4">
      <template v-for="param in currentMaskConfig.params" :key="param.key">
        <!-- Slider Parameter -->
        <div v-if="param.type === 'slider'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i :class="getIconForParam(param.key)" class="mr-1"></i>
            {{ param.label }}{{ param.unit ? ' (' + param.unit + ')' : '' }}
          </label>
          <div class="flex items-center space-x-3">
            <InputText
              v-model.number="uiMask[param.key]"
              type="number"
              :min="param.min"
              :max="param.max"
              class="w-20"
              @change="updateMask({ [param.key]: uiMask[param.key] })"
            />
            <div class="slider-container">
              <Slider
                v-model="uiMask[param.key]"
                :min="param.min"
                :max="param.max"
                :step="param.step"
                @change="updateMask({ [param.key]: uiMask[param.key] })"
                class="slider-component"
              />
            </div>
          </div>
        </div>
        <!-- Dropdown Parameter -->
        <div v-else-if="param.type === 'dropdown'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i :class="getIconForParam(param.key)" class="mr-1"></i>
            {{ param.label }}
          </label>
          <Dropdown
            v-model="uiMask[param.key]"
            :options="param.options"
            optionLabel="label"
            optionValue="value"
            class="w-full"
            @change="updateMask({ [param.key]: uiMask[param.key] })"
          />
        </div>
      </template>

      <!-- SelectButton Parameters for intensity -->
      <template v-if="currentMaskConfig.label === 'Sections Wave' || currentMaskConfig.label === 'Sections' || currentMaskConfig.label === 'Blink'" >
        <div v-for="i in uiMask.numSections" :key="`intensity${i}`" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i class="pi pi-sliders-v mr-1"></i>
            Section {{ i }} Intensity
          </label>
          <SelectButton
            v-model="uiMask[`intensity${i}`]"
            :options="[
              { label: '0%', value: 0 },
              { label: '25%', value: 63 },
              { label: '50%', value: 127 },
              { label: '75%', value: 191 },
              { label: '100%', value: 255 }
            ]"
            optionLabel="label"
            optionValue="value"
            class="w-full"
            @change="updateMask({ [`intensity${i}`]: uiMask[`intensity${i}`] })"
          />
        </div>
      </template>
    </div>
  </div>
</template>

<style scoped>
.slider-container {
  flex: 1;
  padding: 0 8px;
  max-width: calc(100% - 100px);
}

.slider-component {
  width: 100%;
}

:deep(.p-dropdown) {
  width: 100%;
}

:deep(.p-dropdown-item) {
  display: flex;
  align-items: center;
}
</style>
