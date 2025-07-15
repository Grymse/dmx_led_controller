<script setup lang="ts">
import { ref, defineProps, defineEmits, watch, computed } from 'vue';

const props = defineProps<{
  id: number;
  name: string;
  isActive: boolean;
  duration: number;
  colorEffect?: { type: string; [key: string]: any };
}>();

const emit = defineEmits(['click', 'remove', 'update:duration']);

const localDuration = ref(props.duration);

// Update local duration when prop changes
watch(() => props.duration, (newDuration) => {
  localDuration.value = newDuration;
});

const handleClick = () => {
  emit('click', props.id);
};

const handleRemove = (e: Event) => {
  e.stopPropagation();
  emit('remove', props.id);
};

const updateDuration = (e: Event) => {
  e.stopPropagation(); // Prevent module selection when interacting with duration
  const value = parseInt((e.target as HTMLSelectElement).value);
  emit('update:duration', props.id, value);
};

// Helper function to ensure color is in valid format
const formatColorIfNeeded = (color: string) => {
  if (typeof color !== 'string') return '#FF0000';

  if (color.startsWith('#')) {
    return color;
  }

  // If it's a hex without #, add it
  if (/^[0-9A-Fa-f]{6}$/.test(color)) {
    return `#${color}`;
  }

  return `#${color}`;
};

// Use computed property to get colors to display to ensure reactivity
const colorsToDisplay = computed(() => {
  if (!props.colorEffect) return [];

  // For single color effect
  if (props.colorEffect.type === 'single' && props.colorEffect.color) {
    return [formatColorIfNeeded(props.colorEffect.color)];
  }

  // For rainbow effect, create a simulated rainbow gradient (5 colors)
  if (props.colorEffect.type === 'rainbow') {
    return ['#FF0000', '#FFFF00', '#00FF00', '#00FFFF', '#FF00FF'];
  }

  // For multiple color effects
  if (['fade', 'sections', 'sectionsWave', 'switch'].includes(props.colorEffect.type) &&
      Array.isArray(props.colorEffect.colors)) {
    return props.colorEffect.colors.map(color => formatColorIfNeeded(color));
  }

  return [];
});
</script>

<template>
  <div
    class="relative px-4 py-3 rounded-md cursor-pointer transition-all duration-200 flex items-center gap-3 border"
    :class="isActive
      ? 'border-indigo-400 bg-indigo-50 text-indigo-900 shadow-sm'
      : 'border-gray-300 bg-gray-100 hover:bg-gray-200'"
    @click="handleClick"
  >
    <!-- Module name -->
    <span>{{ name }}</span>

    <!-- Color indicators -->
    <div class="flex -space-x-1" v-if="colorEffect">
      <div
        v-for="(color, index) in colorsToDisplay"
        :key="index"
        class="w-4 h-4 rounded-full border border-white"
        :style="{ backgroundColor: color }"
        :title="color"
      ></div>
    </div>

    <!-- Duration dropdown -->
    <div class="flex items-center ml-auto" @click.stop>
      <select
        :value="duration"
        @change="updateDuration"
        class="text-sm border rounded px-2 py-1"
        :class="isActive
          ? 'bg-indigo-100 text-indigo-900 border-indigo-300'
          : 'bg-white text-gray-800 border-gray-300'"
      >
        <option value="500">0.5s</option>
        <option value="1000">1s</option>
        <option value="2000">2s</option>
        <option value="3000">3s</option>
        <option value="5000">5s</option>
        <option value="10000">10s</option>
        <option value="20000">20s</option>
        <option value="30000">30s</option>
      </select>
    </div>

    <!-- Remove button -->
    <button
      class="w-5 h-5 rounded-full flex items-center justify-center text-xs hover:bg-red-600 hover:text-white transition-colors"
      :class="isActive
        ? 'bg-indigo-200 text-indigo-800'
        : 'bg-gray-200 text-gray-600'"
      @click="handleRemove"
    >
      ×
    </button>
  </div>
</template>
