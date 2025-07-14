<script setup lang="ts">
import { ref, watch } from 'vue';
import ColorPicker from 'primevue/colorpicker';
import Button from 'primevue/button';

const props = defineProps<{
  colors: string[] | string;
  multiple: boolean;
  maxColors?: number;
}>();

const emit = defineEmits(['update:colors']);

// Internal representation - always an array for consistent handling
const internalColors = ref<string[]>(
  Array.isArray(props.colors) ? [...props.colors] : [props.colors]
);

// Watch for external changes to colors
watch(() => props.colors, (newColors) => {
  internalColors.value = Array.isArray(newColors) ? [...newColors] : [newColors];
}, { deep: true });

// Add a new color
const addColor = () => {
  if (props.multiple && (!props.maxColors || internalColors.value.length < props.maxColors)) {
    const newColors = [...internalColors.value, '#FF0000'];
    emit('update:colors', newColors);
  }
};

// Remove a color at specific index
const removeColor = (index: number) => {
  if (props.multiple && internalColors.value.length > 1) {
    const newColors = [...internalColors.value];
    newColors.splice(index, 1);
    emit('update:colors', newColors);
  }
};

// Update a specific color
const updateColor = (index: number, color: string) => {
  if (props.multiple) {
    const newColors = [...internalColors.value];
    newColors[index] = color;
    emit('update:colors', newColors);
  } else {
    // For single color mode, just emit the color directly
    emit('update:colors', color);
  }
};
</script>

<template>
  <div>
    <!-- Single color mode -->
    <div v-if="!multiple" class="mb-2">
      <ColorPicker
        v-model="internalColors[0]"
        @change="updateColor(0, internalColors[0])"
        class="w-full"
      />
    </div>

    <!-- Multiple colors mode -->
    <div v-else class="flex flex-wrap gap-0.5 mb-2">
      <div
        v-for="(color, index) in internalColors"
        :key="index"
        class="relative group"
      >
        <ColorPicker
          v-model="internalColors[index]"
          @change="updateColor(index, internalColors[index])"
        />

        <!-- Delete button (appears on hover) -->
        <Button
          icon="pi pi-trash"
          class="p-button-danger p-button-sm absolute -bottom-4 transform -translate-x-1/2 opacity-0 group-hover:opacity-100 transition-opacity"
          @click="removeColor(index)"
          :disabled="internalColors.length <= 1"
          style="width: 1.5rem; height: 1.5rem; padding: 0.25rem"
        />
      </div>

      <!-- Add color button -->
      <Button
        v-if="!maxColors || internalColors.length < maxColors"
        icon="pi pi-plus"
        class="p-button-sm"
        @click="addColor"
        style="width: 2.5rem; height: 2.5rem"
      />
    </div>
  </div>
</template>

<style scoped>
.group {
  margin-bottom: 0.75rem;
}
</style>
