'use client';

import { useRouter } from 'next/navigation';

export default function Przyklady() {
  const router = useRouter();

  const examples = [
    {
      title: "Witaj Świecie",
      description: "Klasyczny przykład na początek.",
      code: `pisz "Witaj Świecie!";
pisz "To jest mój pierwszy program w Benzin.";`
    },
    {
      title: "Obliczenia",
      description: "Proste operacje matematyczne na zmiennych.",
      code: `zmienna a = 10;
zmienna b = 20;
zmienna wynik = a + b;

pisz "Wynik dodawania: " + wynik;`
    },
    {
      title: "Pętla i Warunek",
      description: "Użycie pętli 'dla' oraz instrukcji warunkowej 'jeżeli'.",
      code: `dla (zmienna i = 1; i <= 10; i++) {
    jeżeli (i % 2 == 0) {
        pisz i + " jest parzyste";
    } wprzeciwnymrazie {
        pisz i + " jest nieparzyste";
    }
}`
    },
    {
      title: "Funkcja Silnia",
      description: "Przykład rekurencji w funkcji.",
      code: `funkcja silnia(n) {
    jeżeli (n <= 1) {
        zwróć 1;
    }
    zwróć n * silnia(n - 1);
}

pisz "Silnia z 5 to: " + silnia(5);`
    }
  ];

  const copyToClipboard = (text: string) => {
    navigator.clipboard.writeText(text);
    alert("Skopiowano kod do schowka!");
  };

  return (
    <div className="flex-1 overflow-y-auto p-8 max-w-5xl mx-auto w-full">
      <h1 className="text-4xl font-bold mb-2 text-[#cccccc]">Przykłady</h1>
      <p className="text-[#969696] mb-8 text-lg">Zobacz jak prosty i czytelny jest Benzin. Skopiuj kod i uruchom go w edytorze!</p>
      
      <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
        {examples.map((example, index) => (
          <div key={index} className="bg-[#252526] border border-[#3e3e42] rounded-lg overflow-hidden flex flex-col">
            <div className="p-4 border-b border-[#3e3e42] bg-[#2d2d2d] flex justify-between items-center">
              <div>
                <h3 className="font-bold text-[#cccccc]">{example.title}</h3>
                <p className="text-xs text-[#969696]">{example.description}</p>
              </div>
            </div>
            <div className="p-4 bg-[#1e1e1e] font-mono text-sm text-[#d4d4d4] overflow-x-auto flex-1 whitespace-pre">
              {example.code}
            </div>
            <div className="p-3 bg-[#252526] border-t border-[#3e3e42] flex justify-end gap-2">
              <button 
                onClick={() => copyToClipboard(example.code)}
                className="px-3 py-1.5 text-xs font-medium text-[#cccccc] hover:bg-[#3e3e42] rounded transition-colors"
              >
                Kopiuj
              </button>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
