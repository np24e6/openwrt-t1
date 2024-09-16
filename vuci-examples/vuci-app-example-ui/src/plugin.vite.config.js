// vite.config.js
import { defineConfig, loadEnv } from "vite"
import vue2 from '@vitejs/plugin-vue2';
import dynamicImport from 'vite-plugin-dynamic-import'
import viteCompression from 'vite-plugin-compression';
const path = require("path")
export default defineConfig(({mode}) => {
  const env = loadEnv(mode, process.cwd(), '')
  return {
    plugins: [dynamicImport(), vue2({ isProduction: true }), viteCompression()],
    build: {
      cssCodeSplit: false,
      lib: {
        entry: env.VITE_PLUGIN_ENTRY,
        name: env.VITE_PLUGIN_NAME,
        fileName: env.VITE_PLUGIN_FILENAME,
      },
      rollupOptions: {
        external: ['vue'],
        output: {
          globals: {
            vue: 'Vue'
          }
        }
      }
    },
    resolve: {
      extensions: ['.mjs', '.js', '.ts', '.jsx', '.tsx', 'json', '.vue'],
      alias: {
        "@": path.resolve(__dirname, "./src")
      }
    },
  }
})
