<script setup lang="ts">
import { ref } from 'vue';

const props = defineProps<{
  mask: { type: string; [key: string]: any };
  title: string;
}>();

const emit = defineEmits(['update:mask']);

// Handle updates to the mask
const updateMask = (updates: any) => {
  emit('update:mask', { ...props.mask, ...updates });
};

// Available mask types
const maskTypes = [
  { value: 'none', label: 'None' },
  { value: 'blink', label: 'Blink' },
  { value: 'invert', label: 'Invert' },
  { value: 'pulse', label: 'Pulse' },
  { value: 'sawtooth', label: 'Sawtooth' },
  { value: 'stars', label: 'Stars' },
  { value: 'wave', label: 'Wave' },
  { value: 'sectionsWave', label: 'Sections Wave' },
  { value: 'pulseSawtooth', label: 'Pulse Sawtooth' },
];
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">{{ title }} Type</label>
      <select
        class="w-full border border-gray-300 rounded-md p-2"
        v-model="mask.type"
        @change="updateMask({ type: $event.target.value })"
      >
        <option v-for="type in maskTypes" :key="type.value" :value="type.value">
          {{ type.label }}
        </option>
      </select>
    </div>

    <!-- Dynamic parameters based on mask type -->
    <div v-if="mask.type === 'blink'" class="mb-4">
      <!-- Blink parameters -->
      <label class="block text-sm font-medium text-gray-700 mb-1">Duration (ms)</label>
      <input type="number" class="w-full border border-gray-300 rounded-md p-2" min="1" />
    </div>

    <div v-else-if="mask.type === 'wave'" class="mb-4">
      <!-- Wave parameters -->
      <label class="block text-sm font-medium text-gray-700 mb-1">Duration (ms)</label>
      <input type="number" class="w-full border border-gray-300 rounded-md p-2" min="1" />

      <label class="block text-sm font-medium text-gray-700 mt-2 mb-1">Length</label>
      <input type="number" class="w-full border border-gray-300 rounded-md p-2" min="1" />

      <label class="block text-sm font-medium text-gray-700 mt-2 mb-1">Gap</label>
      <input type="number" class="w-full border border-gray-300 rounded-md p-2" min="0" />
    </div>

    <!-- Similar blocks for other mask types -->

    <button
      v-if="mask.type !== 'none'"
      class="mt-4 bg-blue-500 text-white py-2 px-4 rounded hover:bg-blue-600"
    >
      Apply Changes
    </button>
  </div>
</template>
