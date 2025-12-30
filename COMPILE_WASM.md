# Jak skompilować Benzin do WebAssembly (WASM)

Aby uruchomić Benzin w przeglądarce, musimy skompilować `main.c` do plików `.js` i `.wasm`.

## Wymagania
1. Zainstalowane narzędzie **Emscripten** (emsdk).
   - Instrukcja: https://emscripten.org/docs/getting_started/downloads.html

## Kompilacja

1. Otwórz terminal (z aktywnym środowiskiem Emscripten).
2. Przejdź do folderu projektu.
3. Uruchom komendę:

```bash
emcc main.c -o web/public/benzin.js \
  -s EXPORTED_FUNCTIONS="['_run_code', '_main']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
  -s NO_EXIT_RUNTIME=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -O3
```

To stworzy dwa pliki w folderze `web/public`:
- `benzin.js`
- `benzin.wasm`

## Uruchomienie strony

1. Przejdź do folderu `web`:
   ```bash
   cd web
   ```
2. Zainstaluj zależności:
   ```bash
   npm install
   ```
3. Uruchom serwer deweloperski:
   ```bash
   npm run dev
   ```
4. Otwórz przeglądarkę na `http://localhost:3000`.

Powinieneś zobaczyć edytor kodu i działający interpreter Benzin!
