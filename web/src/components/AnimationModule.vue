<script setup lang="ts">
import { ref, defineProps, defineEmits, watch } from 'vue';

const props = defineProps<{
  id: number;
  name: string;
  isActive: boolean;
  duration: number;
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
</script>

<template>
  <div
    class="relative px-4 py-3 rounded-md cursor-pointer transition-all duration-200 flex items-center gap-3"
    :class="isActive ? 'bg-blue-500 text-white' : 'bg-gray-100 hover:bg-gray-200'"
    @click="handleClick"
  >
    <span>{{ name }}</span>

    <!-- Duration dropdown -->
    <div class="flex items-center" @click.stop>
      <select
        :value="duration"
        @change="updateDuration"
        class="text-sm border rounded px-2 py-1"
        :class="isActive ? 'bg-blue-600 text-white border-blue-400' : 'bg-white text-gray-800 border-gray-300'"
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

    <button
      class="ml-auto w-5 h-5 rounded-full flex items-center justify-center text-xs hover:bg-red-600 hover:text-white transition-colors"
      :class="isActive ? 'bg-blue-600 text-white' : 'bg-gray-200 text-gray-600'"
      @click="handleRemove"
    >
      ×
    </button>
  </div>
</template>
