export default function Dokumentacja() {
  return (
    <div className="flex-1 overflow-y-auto p-8 max-w-4xl mx-auto w-full">
      <h1 className="text-4xl font-bold mb-6 text-[#cccccc]">Dokumentacja Języka Benzin</h1>
      
      <div className="space-y-12 text-[#d4d4d4]">
        
        {/* 1. Podstawy */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">1. Podstawy</h2>
          
          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Komentarze</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#6a9955]"># Komentarz jednoliniowy (styl Python/Bash)</span><br/>
            <span className="text-[#6a9955]">// Komentarz jednoliniowy (styl C)</span><br/>
            <span className="text-[#6a9955]">/* <br/>&nbsp;&nbsp;&nbsp;Komentarz<br/>&nbsp;&nbsp;&nbsp;wieloliniowy <br/>*/</span>
          </div>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Wyjście i Wejście</h3>
          <p className="mb-2">Do wypisywania danych służy instrukcja <code className="text-[#dcdcaa]">pisz</code>, a do pobierania danych <code className="text-[#dcdcaa]">podaj</code>.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Witaj świecie!"</span>;<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Wartość: "</span> + <span className="text-[#b5cea8]">10</span>;<br/><br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">imie</span>;<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Podaj swoje imię:"</span>;<br/>
            <span className="text-[#dcdcaa]">podaj</span> <span className="text-[#9cdcfe]">imie</span>;<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Cześć, "</span> + <span className="text-[#9cdcfe]">imie</span>;
          </div>
        </section>

        {/* 2. Zmienne i Typy Danych */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">2. Zmienne i Typy Danych</h2>
          <p className="mb-4">Zmienne deklarujemy słowem kluczowym <code className="text-[#569cd6]">zmienna</code>. Typy są dynamiczne.</p>
          
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">liczba</span> = <span className="text-[#b5cea8]">42</span>;<br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">zmiennoprzecinkowa</span> = <span className="text-[#b5cea8]">3.14</span>;<br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">tekst</span> = <span className="text-[#ce9178]">"Napis"</span>;<br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">prawda_falsz</span> = <span className="text-[#569cd6]">prawda</span>; <span className="text-[#6a9955]"># lub falsz</span><br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">pustka</span> = <span className="text-[#569cd6]">nic</span>; <span className="text-[#6a9955]"># null</span>
          </div>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Interpolacja ciągów znaków</h3>
          <p className="mb-2">Możesz wstawiać wartości zmiennych bezpośrednio do tekstu używając <code className="text-[#ce9178]">${'{...}'}</code>.</p>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">x</span> = <span className="text-[#b5cea8]">10</span>;<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"X wynosi ${'{x}'}"</span>;
          </div>
        </section>

        {/* 3. Operatory */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">3. Operatory</h2>
          
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
            <div>
              <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Arytmetyczne</h3>
              <ul className="list-disc list-inside space-y-1 text-sm">
                <li><code className="text-[#d4d4d4]">+</code> Dodawanie</li>
                <li><code className="text-[#d4d4d4]">-</code> Odejmowanie</li>
                <li><code className="text-[#d4d4d4]">*</code> Mnożenie</li>
                <li><code className="text-[#d4d4d4]">/</code> Dzielenie</li>
                <li><code className="text-[#d4d4d4]">%</code> Reszta z dzielenia</li>
                <li><code className="text-[#d4d4d4]">++</code> Inkrementacja</li>
                <li><code className="text-[#d4d4d4]">--</code> Dekrementacja</li>
              </ul>
            </div>
            <div>
              <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Logiczne</h3>
              <ul className="list-disc list-inside space-y-1 text-sm">
                <li><code className="text-[#d4d4d4]">==</code> lub <code className="text-[#569cd6]">rowne</code></li>
                <li><code className="text-[#d4d4d4]">!=</code> lub <code className="text-[#569cd6]">nierowne</code></li>
                <li><code className="text-[#d4d4d4]">&lt;</code>, <code className="text-[#d4d4d4]">&gt;</code>, <code className="text-[#d4d4d4]">&lt;=</code>, <code className="text-[#d4d4d4]">&gt;=</code></li>
                <li><code className="text-[#d4d4d4]">&&</code> lub <code className="text-[#569cd6]">oraz</code></li>
                <li><code className="text-[#d4d4d4]">||</code> lub <code className="text-[#569cd6]">albo</code></li>
                <li><code className="text-[#d4d4d4]">!</code> Negacja</li>
              </ul>
            </div>
            <div>
              <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Bitowe</h3>
              <ul className="list-disc list-inside space-y-1 text-sm">
                <li><code className="text-[#d4d4d4]">&</code> AND</li>
                <li><code className="text-[#d4d4d4]">|</code> OR</li>
                <li><code className="text-[#d4d4d4]">^</code> XOR</li>
                <li><code className="text-[#d4d4d4]">~</code> NOT</li>
                <li><code className="text-[#d4d4d4]">&lt;&lt;</code> Przesunięcie w lewo</li>
                <li><code className="text-[#d4d4d4]">&gt;&gt;</code> Przesunięcie w prawo</li>
              </ul>
            </div>
            <div>
              <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Przypisania</h3>
              <ul className="list-disc list-inside space-y-1 text-sm">
                <li><code className="text-[#d4d4d4]">+=</code>, <code className="text-[#d4d4d4]">-=</code></li>
                <li><code className="text-[#d4d4d4]">*=</code>, <code className="text-[#d4d4d4]">/=</code></li>
                <li><code className="text-[#d4d4d4]">%=</code></li>
              </ul>
            </div>
          </div>
        </section>

        {/* 4. Instrukcje Sterujące */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">4. Instrukcje Sterujące</h2>
          
          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Instrukcja Warunkowa (Jeżeli)</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">x</span> &gt; <span className="text-[#b5cea8]">0</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Dodatnia"</span>;<br/>
            {'}'} <span className="text-[#c586c0]">inaczej</span> {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Niedodatnia"</span>;<br/>
            {'}'}
          </div>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Instrukcja Wyboru (Switch)</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#c586c0]">wybor</span> (<span className="text-[#9cdcfe]">x</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">przypadek</span> <span className="text-[#b5cea8]">1</span>:<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Jeden"</span>;<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#c586c0]">zlam</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">domyslnie</span>:<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Inna"</span>;<br/>
            {'}'}
          </div>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Pętle</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#6a9955]"># Pętla While</span><br/>
            <span className="text-[#c586c0]">wkółko</span> (<span className="text-[#9cdcfe]">i</span> &lt; <span className="text-[#b5cea8]">5</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">i</span>;<br/>
            &nbsp;&nbsp;<span className="text-[#9cdcfe]">i</span>++;<br/>
            {'}'}<br/><br/>
            <span className="text-[#6a9955]"># Pętla For</span><br/>
            <span className="text-[#c586c0]">dla</span> (<span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">i</span>=<span className="text-[#b5cea8]">0</span>; <span className="text-[#9cdcfe]">i</span>&lt;<span className="text-[#b5cea8]">10</span>; <span className="text-[#9cdcfe]">i</span>++) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">i</span> == <span className="text-[#b5cea8]">5</span>) {'{'} <span className="text-[#c586c0]">pomiń</span>; {'}'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">i</span>;<br/>
            {'}'}
          </div>
        </section>

        {/* 5. Funkcje */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">5. Funkcje</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">funkcja</span> <span className="text-[#dcdcaa]">powitanie</span>(<span className="text-[#9cdcfe]">imie</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">zwróć</span> <span className="text-[#ce9178]">"Cześć "</span> + <span className="text-[#9cdcfe]">imie</span>;<br/>
            {'}'}<br/><br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#dcdcaa]">powitanie</span>(<span className="text-[#ce9178]">"Marek"</span>);
          </div>
        </section>

        {/* 6. Struktury Danych */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">6. Struktury Danych</h2>
          
          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Tablice</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">arr</span> = [<span className="text-[#b5cea8]">1</span>, <span className="text-[#b5cea8]">2</span>, <span className="text-[#b5cea8]">3</span>];<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">arr</span>[<span className="text-[#b5cea8]">0</span>];<br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">arr</span>.<span className="text-[#9cdcfe]">dlugosc</span>;<br/>
            <span className="text-[#9cdcfe]">arr</span>.<span className="text-[#dcdcaa]">dodaj</span>(<span className="text-[#b5cea8]">4</span>);<br/>
            <span className="text-[#9cdcfe]">arr</span>.<span className="text-[#dcdcaa]">usun</span>(<span className="text-[#b5cea8]">0</span>);
          </div>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Słowniki (Obiekty)</h3>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">osoba</span> = {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#ce9178]">"imie"</span>: <span className="text-[#ce9178]">"Jan"</span>,<br/>
            &nbsp;&nbsp;<span className="text-[#9cdcfe]">wiek</span>: <span className="text-[#b5cea8]">30</span><br/>
            {'}'};<br/><br/>
            <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">osoba</span>.<span className="text-[#9cdcfe]">imie</span>;
          </div>
        </section>

        {/* 7. Programowanie Obiektowe */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">7. Programowanie Obiektowe</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#569cd6]">klasa</span> <span className="text-[#4ec9b0]">Pojazd</span> {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#569cd6]">konstruktor</span>(<span className="text-[#9cdcfe]">marka</span>) {'{'}<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#569cd6]">to</span>.<span className="text-[#9cdcfe]">marka</span> = <span className="text-[#9cdcfe]">marka</span>;<br/>
            &nbsp;&nbsp;{'}'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">jedz</span>() {'{'}<br/>
            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"${'{to.marka}'} jedzie..."</span>;<br/>
            &nbsp;&nbsp;{'}'}<br/>
            {'}'}<br/><br/>
            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">auto</span> = <span className="text-[#569cd6]">nowy</span> <span className="text-[#4ec9b0]">Pojazd</span>(<span className="text-[#ce9178]">"Fiat"</span>);<br/>
            <span className="text-[#9cdcfe]">auto</span>.<span className="text-[#dcdcaa]">jedz</span>();
          </div>
        </section>

        {/* 8. Obsługa Błędów */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">8. Obsługa Błędów</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#c586c0]">sprobuj</span> {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#c586c0]">rzuc</span> <span className="text-[#ce9178]">"Błąd!"</span>;<br/>
            {'}'} <span className="text-[#c586c0]">zlap</span> (<span className="text-[#9cdcfe]">e</span>) {'{'}<br/>
            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">e</span>;<br/>
            {'}'}
          </div>
        </section>

        {/* 9. Moduły */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">9. Moduły</h2>
          <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
            <span className="text-[#c586c0]">import</span> <span className="text-[#ce9178]">"biblioteka.bzn"</span>;
          </div>
        </section>

        {/* 10. Biblioteka Standardowa */}
        <section>
          <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">10. Biblioteka Standardowa</h2>
          
          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Napisy</h3>
          <ul className="list-disc list-inside space-y-1 text-sm mb-4">
            <li><code className="text-[#9cdcfe]">.dlugosc</code></li>
            <li><code className="text-[#9cdcfe]">.wGore()</code></li>
            <li><code className="text-[#9cdcfe]">.wDol()</code></li>
            <li><code className="text-[#9cdcfe]">.wytnij(start, dlugosc)</code></li>
            <li><code className="text-[#9cdcfe]">.podziel(separator)</code></li>
          </ul>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Matematyka (Matma)</h3>
          <ul className="list-disc list-inside space-y-1 text-sm mb-4">
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#9cdcfe]">PI</code></li>
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#dcdcaa]">sin</code>(<span className="text-[#9cdcfe]">x</span>), <code className="text-[#dcdcaa]">cos</code>(<span className="text-[#9cdcfe]">x</span>)</li>
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#dcdcaa]">pierwiastek</code>(<span className="text-[#9cdcfe]">x</span>)</li>
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#dcdcaa]">potega</code>(<span className="text-[#9cdcfe]">a</span>, <span className="text-[#9cdcfe]">b</span>)</li>
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#dcdcaa]">losuj</code>()</li>
            <li><code className="text-[#4ec9b0]">Matma</code>.<code className="text-[#dcdcaa]">czas</code>()</li>
          </ul>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Pliki (Plik)</h3>
          <ul className="list-disc list-inside space-y-1 text-sm mb-4">
            <li><code className="text-[#4ec9b0]">Plik</code>.<code className="text-[#dcdcaa]">czytaj</code>(<span className="text-[#ce9178]">"sciezka"</span>)</li>
            <li><code className="text-[#4ec9b0]">Plik</code>.<code className="text-[#dcdcaa]">zapisz</code>(<span className="text-[#ce9178]">"sciezka"</span>, <span className="text-[#ce9178]">"tresc"</span>)</li>
            <li><code className="text-[#4ec9b0]">Plik</code>.<code className="text-[#dcdcaa]">dopisz</code>(<span className="text-[#ce9178]">"sciezka"</span>, <span className="text-[#ce9178]">"tresc"</span>)</li>
          </ul>

          <h3 className="text-xl font-medium mb-2 text-[#4ec9b0]">Konwersja</h3>
          <ul className="list-disc list-inside space-y-1 text-sm">
            <li><code className="text-[#dcdcaa]">tekst</code>(<span className="text-[#9cdcfe]">wartosc</span>)</li>
            <li><code className="text-[#dcdcaa]">liczba</code>(<span className="text-[#9cdcfe]">napis</span>)</li>
          </ul>
        </section>

      </div>
    </div>
  );
}
