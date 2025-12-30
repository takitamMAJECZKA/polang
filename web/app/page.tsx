'use client';

import { useState, useEffect, useRef } from 'react';
import Editor from '@monaco-editor/react';

// Define types for the WASM module
interface BenzinModule {
  ccall: (ident: string, returnType: string, argTypes: string[], args: any[]) => any;
  print: (text: string) => void;
  printErr: (text: string) => void;
}

interface WindowWithBenzin extends Window {
  createBenzinModule?: (config: any) => Promise<BenzinModule>;
  BenzinInstance?: BenzinModule;
}

export default function Home() {
  const [output, setOutput] = useState<string>('');
  const [code, setCode] = useState<string>(`zmienna x = 10
pisz "Witaj w Benzin!"
pisz "x = " + x
dla (zmienna i=0; i<3; i++) {
    pisz "Petla " + i
}`);
  const [isReady, setIsReady] = useState(false);
  const wasmModule = useRef<BenzinModule | null>(null);

  useEffect(() => {
    const loadWasm = async () => {
      try {
        const win = window as WindowWithBenzin;
        
        if (win.BenzinInstance) {
          setIsReady(true);
          wasmModule.current = win.BenzinInstance;
          return;
        }

        const script = document.createElement('script');
        script.src = '/benzin.js';

        // FIX: Temporarily hide 'define' to prevent Emscripten from using AMD loader
        // This avoids "Can only have one anonymous define call per script file" error
        // caused by conflict with Monaco Editor's loader.
        const globalWin = window as any;
        const savedDefine = globalWin.define;
        globalWin.define = undefined;

        script.onload = async () => {
          // Restore 'define'
          globalWin.define = savedDefine;

          if (win.createBenzinModule) {
            const moduleConfig = {
              print: (text: string) => {
                setOutput((prev) => prev + text + '\\n');
              },
              printErr: (text: string) => {
                setOutput((prev) => prev + '[BŁĄD] ' + text + '\\n');
              }
            };

            try {
              const instance = await win.createBenzinModule(moduleConfig);
              wasmModule.current = instance;
              win.BenzinInstance = instance;
              setIsReady(true);
            } catch (e) {
              console.error("WASM init error:", e);
              setOutput(prev => prev + "Critical Error: Failed to initialize WASM\\n" + String(e));
            }
          }
        };
        
        script.onerror = () => {
          globalWin.define = savedDefine;
          console.error("Failed to load benzin.js");
        };

        document.body.appendChild(script);
      } catch (err) {
        console.error(err);
      }
    };
    loadWasm();
  }, []);

  const runCode = () => {
    setOutput('');
    if (wasmModule.current) {
      try {
        wasmModule.current.ccall('run_code', 'null', ['string'], [code]);
      } catch (e) {
        setOutput((prev) => prev + '\\n[CRITICAL ERROR] ' + String(e));
      }
    }
  };

  const handleEditorWillMount = (monaco: any) => {
    monaco.languages.register({ id: 'benzin' });

    monaco.languages.setMonarchTokensProvider('benzin', {
      tokenizer: {
        root: [
          [/\b(jeżeli|wprzeciwnymrazie|dopóki|dla|zwróć)\b/, 'keyword.control'],
          [/\b(zmienna|funkcja|klasa|nowy|to)\b/, 'keyword'],
          [/\b(prawda|fałsz|null)\b/, 'constant.language'],
          [/\b(pisz)\b/, 'support.function'],
          [/\b(liczba|tekst)\b/, 'type'],
          
          // Identifiers
          [/\b[A-Z]\w*\b/, 'type.identifier'], // Classes (Uppercase start)
          [/\b[a-zA-Z_]\w*(?=\s*\()/, 'function'], // Functions (followed by '(')
          [/\b[a-zA-Z_]\w*\b/, 'identifier'], // Variables

          [/"([^"\\]|\\.)*"/, 'string'],
          [/\/\/.*$/, 'comment'],
          [/[{}()\[\]]/, 'delimiter'],
          [/[0-9]+(\.[0-9]+)?/, 'number'],
          [/[+\-*\/=<>!&|^~]+/, 'operator'],
        ],
      },
    });

    monaco.editor.defineTheme('benzin-dark', {
      base: 'vs-dark',
      inherit: true,
      rules: [
        { token: 'keyword.control', foreground: 'c586c0' }, // Purple (if, else, for)
        { token: 'keyword', foreground: '569cd6' }, // Blue (var, function)
        { token: 'constant.language', foreground: '569cd6' }, // Blue (true, false)
        { token: 'support.function', foreground: 'dcdcaa' }, // Yellow (print)
        { token: 'type', foreground: '4ec9b0' }, // Teal (types)
        { token: 'type.identifier', foreground: '4ec9b0' }, // Teal (Classes)
        { token: 'function', foreground: 'dcdcaa' }, // Yellow (Functions)
        { token: 'identifier', foreground: '9cdcfe' }, // Light Blue (Variables)
        { token: 'string', foreground: 'ce9178' }, // Orange (strings)
        { token: 'comment', foreground: '6a9955' }, // Green (comments)
        { token: 'number', foreground: 'b5cea8' }, // Light Green (numbers)
        { token: 'delimiter', foreground: 'd4d4d4' },
        { token: 'operator', foreground: 'd4d4d4' },
      ],
      colors: {
        'editor.background': '#1e1e1e', // VS Code Default Dark BG
        'editor.foreground': '#d4d4d4',
      },
    });
  };

  return (
    <div className="flex flex-col h-full bg-[#1e1e1e] text-[#d4d4d4] font-sans overflow-hidden">
      <div className="h-[40px] bg-[#252526] flex items-center justify-between px-4 border-b border-[#1e1e1e]">
        <div className="text-xs font-bold text-[#cccccc] uppercase tracking-wider flex items-center gap-2">
          <span className="text-[#569cd6]">main.bzn</span>
        </div>
        <button
          onClick={runCode}
          disabled={!isReady}
          className={`
            flex items-center gap-2 px-3 py-1 rounded-sm font-semibold text-xs transition-colors
            ${isReady 
              ? 'bg-[#238636] text-white hover:bg-[#2ea043] cursor-pointer' 
              : 'bg-[#2d2d2d] text-[#6e7681] cursor-not-allowed'}
          `}
        >
          {isReady ? (
            <>
              <svg width="14" height="14" viewBox="0 0 16 16" fill="currentColor">
                <path d="M3 2l10 6-10 6z"></path>
              </svg>
              URUCHOM
            </>
          ) : (
            'Ładowanie...'
          )}
        </button>
      </div>

      <div className="flex flex-1 overflow-hidden">
        {/* Editor Pane */}
        <div className="flex-1 border-r border-[#2d2d2d] flex flex-col min-w-[300px]">
          <div className="flex-1">
            <Editor
              height="100%"
              defaultLanguage="benzin"
              theme="benzin-dark"
              beforeMount={handleEditorWillMount}
              value={code}
              onChange={(value) => setCode(value || '')}
              options={{
                minimap: { enabled: false },
                fontSize: 14,
                fontFamily: "'JetBrains Mono', 'Fira Code', Consolas, monospace",
                lineNumbers: 'on',
                scrollBeyondLastLine: false,
                automaticLayout: true,
                padding: { top: 16, bottom: 16 },
                renderLineHighlight: 'none',
              }}
            />
          </div>
        </div>

        {/* Output Pane */}
        <div className="flex-1 bg-[#1e1e1e] flex flex-col min-w-[300px]">
          <div className="bg-[#252526] px-4 py-2 text-xs font-semibold text-[#cccccc] uppercase tracking-wider border-b border-[#1e1e1e]">
            Terminal
          </div>
          <div className="flex-1 p-4 font-mono text-sm leading-relaxed overflow-y-auto text-[#cccccc]">
            {output ? (
              output.split('\\n').map((line, i) => (
                <div key={i} className={`mb-0.5 ${line.includes('[BŁĄD]') ? 'text-[#f14c4c]' : ''}`}>
                  {line}
                </div>
              ))
            ) : (
              <div className="text-[#6e7681] italic">Kliknij "URUCHOM" aby zobaczyć wynik...</div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
