<script setup lang="ts">
import { ref, watch, computed } from 'vue';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import Dropdown from 'primevue/dropdown';
import ColorPickerGroup from './ColorPickerGroup.vue';
import { convertTimeValues } from '@/lib/timeUtils';

// Constants for duration bounds (in ms)
const MIN_DURATION_MS = 175; // 7 ticks * 25ms
const MAX_DURATION_MS = 50000; // 2000 ticks * 25ms

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
    icon: 'pi pi-circle-fill',
    id: 0, // SingleColor = 0
    params: [
      { key: 'color', type: 'color', default: '#FF0000', required: true },
    ]
  },

  'rainbow': {
    label: 'Rainbow',
    icon: 'pi pi-palette',
    id: 1, // RainbowColor = 1
    params: [
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true },
      { key: 'length', type: 'slider', default: 150, min: 1, max: 500, step: 5, label: 'Length', required: true }
    ]
  },

  'sectionsWave': {
    label: 'Sections Wave',
    icon: 'pi pi-sliders-h',
    id: 2, // SectionsWaveColor = 2
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },

  'sections': {
    label: 'Sections',
    icon: 'pi pi-bars',
    id: 3, // SectionsColor = 3
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },

  'fade': {
    label: 'Fade',
    icon: 'pi pi-sort-amount-down-alt',
    id: 4, // FadeColor = 4
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 1000, min: MIN_DURATION_MS, max: MAX_DURATION_MS, step: 25, label: 'Duration', unit: 'ms', required: true }
    ]
  },

  'switch': {
    label: 'Switch',
    icon: 'pi pi-sync',
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
  label: config.label,
  icon: config.icon
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

// PrimeVue dropdown option template
const optionTemplate = (option: any) => {
  return {
    template: `
      <div class="flex items-center gap-2">
        <i class="${option.icon}" style="font-size: 1rem"></i>
        <div>${option.label}</div>
      </div>
    `
  };
};

// Handle dropdown change
const handleDropdownChange = (event: any) => {
  updateEffect({ type: event.value });
};
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Effect Type</label>
      <Dropdown
        v-model="uiEffect.type"
        :options="effectTypes"
        optionLabel="label"
        optionValue="value"
        placeholder="Select an Effect"
        class="w-full"
        @change="handleDropdownChange"
      >
        <template #value="slotProps">
          <div v-if="slotProps.value" class="flex items-center gap-2">
            <i :class="effectConfigs[slotProps.value].icon" style="font-size: 1rem"></i>
            <div>{{ effectConfigs[slotProps.value].label }}</div>
          </div>
          <span v-else>Select an Effect</span>
        </template>
        <template #option="slotProps">
          <div class="flex items-center gap-2">
            <i :class="slotProps.option.icon" style="font-size: 1rem"></i>
            <div>{{ slotProps.option.label }}</div>
          </div>
        </template>
      </Dropdown>
    </div>

    <!-- Dynamic parameters based on effect type -->
    <div v-if="currentEffectConfig" class="mb-4">
      <template v-for="param in currentEffectConfig.params" :key="param.key">
        <!-- Color Parameter -->
        <div v-if="param.key === 'color'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i class="pi pi-palette mr-1"></i> Color
          </label>
          <ColorPickerGroup
            :colors="uiEffect.color || param.default"
            :multiple="false"
            @update:colors="updateColors"
          />
        </div>

        <!-- Multiple Colors Parameter -->
        <div v-else-if="param.key === 'colors'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i class="pi pi-palette mr-1"></i> Colors
          </label>
          <ColorPickerGroup
            :colors="uiEffect.colors || param.default"
            :multiple="true"
            @update:colors="updateColors"
          />
        </div>

        <!-- Slider Parameter -->
        <div v-else-if="param.type === 'slider'" class="mb-4">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            <i :class="param.key === 'duration' ? 'pi pi-clock' : 'pi pi-arrows-h'" class="mr-1"></i>
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

:deep(.p-dropdown) {
  width: 100%;
}

:deep(.p-dropdown-item) {
  display: flex;
  align-items: center;
}
</style>
