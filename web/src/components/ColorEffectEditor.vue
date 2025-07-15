<script setup lang="ts">
import { ref, watch, computed } from 'vue';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import ColorPickerGroup from './ColorPickerGroup.vue';
import { convertTimeValues, ticksToMs, msToTicks } from '@/lib/timeUtils';

// Constants for duration bounds (in ms)
const MIN_DURATION_MS = 175; // 7 ticks * 25ms
const MAX_DURATION_MS = 25000; // 1000 ticks * 25ms

const props = defineProps<{
  effect: { type: string; [key: string]: any };
}>();

const emit = defineEmits(['update:effect']);

// Create a local copy of the effect with ms values for the UI
const uiEffect = ref<Record<string, any>>({});

// Watch for changes in the effect prop (which contains tick values)
watch(() => props.effect, (newEffect) => {
  // Convert incoming tick values to ms for the UI
  if (newEffect) {
    uiEffect.value = convertTimeValues(newEffect, false);
  }
}, { immediate: true, deep: true });

// Define the configuration for each effect type
const effectConfigs = {
  'single': {
    label: 'Single Color',
    id: 0, // SingleColor = 0
    params: [
      { key: 'color', type: 'color', default: '#FF0000', required: true },
    ]
  },
  'rainbow': {
    label: 'Rainbow',
    id: 1, // RainbowColor = 1
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'length', type: 'slider', default: 150, min: 1, max: 500, step: 5, label: 'Length', required: true }
    ]
  },
  'sectionsWave': {
    label: 'Sections Wave',
    id: 2, // SectionsWaveColor = 2
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sections': {
    label: 'Sections',
    id: 3, // SectionsColor = 3
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'fade': {
    label: 'Fade',
    id: 4, // FadeColor = 4
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'switch': {
    label: 'Switch',
    id: 5, // SwitchColor = 5
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  }
};

// Generate effectTypes array for select dropdown
const effectTypes = Object.entries(effectConfigs).map(([value, config]) => ({
  value,
  label: config.label
}));

// Get current effect configuration
const currentEffectConfig = computed(() => {
  return effectConfigs[uiEffect.value.type] || null;
});

// Handle updates to the effect
const updateEffect = (updates: any) => {
  // Update the local UI version
  uiEffect.value = { ...uiEffect.value, ...updates };

  // Convert time values from ms back to ticks for the actual data model
  const tickUpdates = convertTimeValues(updates, true);
  emit('update:effect', { ...props.effect, ...tickUpdates });
};

// Ensure effect has required parameters when type changes
const initializeEffect = () => {
  const config = currentEffectConfig.value;
  if (!config) return;

  const updates: Record<string, any> = {};
  let hasUpdates = false;

  config.params.forEach(param => {
    // Check if parameter exists and is valid
    if (param.key === 'color' && (uiEffect.value.color === undefined || typeof uiEffect.value.color !== 'string')) {
      updates.color = param.default;
      hasUpdates = true;
    } else if (param.key === 'colors' && (!uiEffect.value.colors || !Array.isArray(uiEffect.value.colors) || uiEffect.value.colors.length === 0)) {
      updates.colors = [...param.default];
      hasUpdates = true;
    } else if (param.key !== 'color' && param.key !== 'colors' && uiEffect.value[param.key] === undefined) {
      updates[param.key] = param.default;
      hasUpdates = true;
    }
    // Ensure duration is within bounds
    else if (param.key === 'duration') {
      if (uiEffect.value.duration < MIN_DURATION_MS) {
        updates.duration = MIN_DURATION_MS;
        hasUpdates = true;
      } else if (uiEffect.value.duration > MAX_DURATION_MS) {
        updates.duration = MAX_DURATION_MS;
        hasUpdates = true;
      }
    }
  });

  if (hasUpdates) {
    updateEffect(updates);
  }
};

// Handle color updates from ColorPickerGroup
const updateColors = (colors: string[] | string) => {
  if (uiEffect.value.type === 'single') {
    updateEffect({ color: colors });
  } else if (Array.isArray(colors)) {
    updateEffect({ colors });
  }
};

// Watch for effect type changes to initialize properties
watch(() => uiEffect.value.type, () => {
  initializeEffect();
}, { immediate: true });
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Effect Type</label>
      <select
        class="w-full border border-gray-300 rounded-md p-2"
        v-model="uiEffect.type"
        @change="updateEffect({ type: $event.target.value })"
      >
        <option v-for="type in effectTypes" :key="type.value" :value="type.value">
          {{ type.label }}
        </option>
      </select>
    </div>

    <!-- Dynamic parameters based on effect type -->
    <div v-if="currentEffectConfig" class="mb-4">
      <template v-for="param in currentEffectConfig.params" :key="param.key">
        <!-- Color Parameter -->
        <div v-if="param.key === 'color'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">Color</label>
          <ColorPickerGroup
            :colors="uiEffect.color || param.default"
            :multiple="false"
            @update:colors="updateColors"
          />
        </div>

        <!-- Multiple Colors Parameter -->
        <div v-else-if="param.key === 'colors'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">Colors</label>
          <ColorPickerGroup
            :colors="uiEffect.colors || param.default"
            :multiple="true"
            @update:colors="updateColors"
          />
        </div>

        <!-- Slider Parameter -->
        <div v-else-if="param.type === 'slider'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            {{ param.label }}{{ param.unit ? ' (' + param.unit + ')' : '' }}
          </label>
          <div class="flex items-center space-x-3">
            <InputText
              v-model.number="uiEffect[param.key]"
              type="number"
              :min="param.min"
              :max="param.max"
              class="w-20"
              @change="updateEffect({ [param.key]: uiEffect[param.key] })"
            />
            <div class="slider-container">
              <Slider
                v-model="uiEffect[param.key]"
                :min="param.min"
                :max="param.max"
                :step="param.step"
                @change="updateEffect({ [param.key]: uiEffect[param.key] })"
                class="slider-component"
              />
            </div>
          </div>
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
</style>
