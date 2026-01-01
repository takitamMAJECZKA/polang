'use client';

export default function Przyklady() {
  return (
    <div className="flex-1 overflow-y-auto p-8 max-w-4xl mx-auto w-full">
      <h1 className="text-4xl font-bold mb-6 text-[#cccccc]">Przykłady Benzin</h1>
      <p className="mb-8 text-[#d4d4d4]">
        Zbiór przykładowych programów napisanych w języku Benzin. Możesz je skopiować i uruchomić w edytorze.
      </p>
      
      <div className="space-y-12 text-[#d4d4d4]">
        
        {/* Witaj Świecie */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">1. Witaj Świecie</h2>
          <p className="mb-2 text-sm text-[#9cdcfe]">Podstawowy program wypisujący tekst na ekran.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Witaj Świecie!"</span>;<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"To jest mój pierwszy program w Benzin."</span>;
          </div>
        </section>

        {/* Obliczenia */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">2. Proste Obliczenia</h2>
          <p className="mb-2 text-sm text-[#9cdcfe]">Dodawanie zmiennych i łączenie ich z tekstem.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">a</span> = <span className="text-[#b5cea8]">10</span>;<br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">b</span> = <span className="text-[#b5cea8]">20</span>;<br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">wynik</span> = <span className="text-[#9cdcfe]">a</span> + <span className="text-[#9cdcfe]">b</span>;<br/><br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Wynik dodawania: "</span> + <span className="text-[#9cdcfe]">wynik</span>;
          </div>
        </section>

        {/* Pętla i Warunek */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">3. Pętla i Warunek</h2>
          <p className="mb-2 text-sm text-[#9cdcfe]">Sprawdzanie parzystości liczb w pętli.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#c586c0]">dla</span> (<span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">i</span> = <span className="text-[#b5cea8]">1</span>; <span className="text-[#9cdcfe]">i</span> &lt;= <span className="text-[#b5cea8]">10</span>; <span className="text-[#9cdcfe]">i</span>++) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">i</span> % <span className="text-[#b5cea8]">2</span> == <span className="text-[#b5cea8]">0</span>) {'{'}<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">i</span> + <span className="text-[#ce9178]">" jest parzyste"</span>;<br/>
            &nbsp;&nbsp;{'}'} <span className="text-[#c586c0]">wprzeciwnymrazie</span> {'{'}<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">i</span> + <span className="text-[#ce9178]">" jest nieparzyste"</span>;<br/>
            &nbsp;&nbsp;{'}'}<br/>
            {'}'}
          </div>
        </section>

        {/* Ciąg Fibonacciego */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">4. Ciąg Fibonacciego (Rekurencja)</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">funkcja</span> <span className="text-[#dcdcaa]">fib</span>(<span className="text-[#9cdcfe]">n</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">n</span> &lt;= <span className="text-[#b5cea8]">1</span>) <span className="text-[#c586c0]">zwróć</span> <span className="text-[#9cdcfe]">n</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">zwróć</span> <span className="text-[#dcdcaa]">fib</span>(<span className="text-[#9cdcfe]">n</span>-<span className="text-[#b5cea8]">1</span>) + <span className="text-[#dcdcaa]">fib</span>(<span className="text-[#9cdcfe]">n</span>-<span className="text-[#b5cea8]">2</span>);<br/>
            {'}'}<br/><br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"10-ty wyraz ciągu Fibonacciego: "</span> + <span className="text-[#dcdcaa]">fib</span>(<span className="text-[#b5cea8]">10</span>);
          </div>
        </section>

        {/* Silnia */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">5. Silnia</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">funkcja</span> <span className="text-[#dcdcaa]">silnia</span>(<span className="text-[#9cdcfe]">n</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">n</span> == <span className="text-[#b5cea8]">0</span>) <span className="text-[#c586c0]">zwróć</span> <span className="text-[#b5cea8]">1</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">zwróć</span> <span className="text-[#9cdcfe]">n</span> * <span className="text-[#dcdcaa]">silnia</span>(<span className="text-[#9cdcfe]">n</span>-<span className="text-[#b5cea8]">1</span>);<br/>
            {'}'}<br/><br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Silnia z 5 to: "</span> + <span className="text-[#dcdcaa]">silnia</span>(<span className="text-[#b5cea8]">5</span>);
          </div>
        </section>

        {/* FizzBuzz */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">6. FizzBuzz</h2>
          <p className="mb-2 text-sm text-[#9cdcfe]">Klasyczne zadanie rekrutacyjne.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#c586c0]">dla</span> (<span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">i</span>=<span className="text-[#b5cea8]">1</span>; <span className="text-[#9cdcfe]">i</span>&lt;=<span className="text-[#b5cea8]">20</span>; <span className="text-[#9cdcfe]">i</span>++) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">i</span> % <span className="text-[#b5cea8]">15</span> == <span className="text-[#b5cea8]">0</span>) <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"FizzBuzz"</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">inaczej jeżeli</span> (<span className="text-[#9cdcfe]">i</span> % <span className="text-[#b5cea8]">3</span> == <span className="text-[#b5cea8]">0</span>) <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Fizz"</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">inaczej jeżeli</span> (<span className="text-[#9cdcfe]">i</span> % <span className="text-[#b5cea8]">5</span> == <span className="text-[#b5cea8]">0</span>) <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Buzz"</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">inaczej</span> <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">i</span>;<br/>
            {'}'}
          </div>
        </section>

      </div>
    </div>
  );
}
