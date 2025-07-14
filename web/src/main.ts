import './assets/main.css'

import { createApp } from 'vue'
import { createPinia } from 'pinia'
import App from './App.vue'
import PrimeVue from 'primevue/config';
import Lara from "@primeuix/themes/lara";

const app = createApp(App)
app.use(createPinia())

app.use(PrimeVue, { ripple: true, theme: {
  preset: Lara,
} });

app.mount('#app')
