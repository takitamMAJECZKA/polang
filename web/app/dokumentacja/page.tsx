"use client";

import { useState } from 'react';

export default function Dokumentacja() {
  const [mode, setMode] = useState<'noob' | 'dev'>('noob');

  const scrollToSection = (id: string) => {
    const element = document.getElementById(id);
    if (element) {
      element.scrollIntoView({ behavior: 'smooth' });
    }
  };

  return (
    <div className="flex h-full overflow-hidden flex-col md:flex-row">
      {/* Sidebar - Spis Treści */}
      <div className="w-full md:w-64 bg-[#1e1e1e] border-r border-[#2d2d2d] overflow-y-auto p-6 flex-shrink-0">
        <div className="mb-6">
            <label className="text-[#cccccc] text-sm font-bold mb-2 block">Poziom Dokumentacji:</label>
            <div className="flex bg-[#2d2d2d] rounded p-1">
                <button 
                    onClick={() => setMode('noob')}
                    className={`flex-1 py-1 text-xs rounded ${mode === 'noob' ? 'bg-[#007acc] text-white' : 'text-[#cccccc] hover:bg-[#3e3e42]'}`}
                >
                     Początkujący
                </button>
                <button 
                    onClick={() => setMode('dev')}
                    className={`flex-1 py-1 text-xs rounded ${mode === 'dev' ? 'bg-[#007acc] text-white' : 'text-[#cccccc] hover:bg-[#3e3e42]'}`}
                >
                     Zaawansowany
                </button>
            </div>
        </div>

        <h2 className="text-xl font-bold mb-4 text-[#cccccc]">Spis Treści</h2>
        <nav className="space-y-2 text-sm">
          {mode === 'noob' ? (
            <>
                <button onClick={() => scrollToSection('wstep')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">1. Wstęp</button>
                <button onClick={() => scrollToSection('zmienne')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">2. Zmienne (Pudełka)</button>
                <button onClick={() => scrollToSection('decyzje')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">3. Decyzje (Jeżeli)</button>
                <button onClick={() => scrollToSection('petle')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">4. Pętle (Powtarzanie)</button>
                <button onClick={() => scrollToSection('funkcje')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">5. Funkcje (Przepisy)</button>
                <button onClick={() => scrollToSection('tablice')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">6. Tablice (Półki)</button>
                <button onClick={() => scrollToSection('obiekty')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">7. Obiekty (Plecaki)</button>
                <button onClick={() => scrollToSection('moduly')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">8. Moduły</button>
            </>
          ) : (
            <>
                <button onClick={() => scrollToSection('skladnia')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">1. Składnia i Typy</button>
                <button onClick={() => scrollToSection('zmienne-dev')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">2. Zmienne i Stałe</button>
                <button onClick={() => scrollToSection('operatory')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">3. Operatory</button>
                <button onClick={() => scrollToSection('control-flow')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">4. Sterowanie Przepływem</button>
                <button onClick={() => scrollToSection('funkcje-dev')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">5. Funkcje</button>
                <button onClick={() => scrollToSection('oop')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">6. Programowanie Obiektowe</button>
                <button onClick={() => scrollToSection('errors')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">7. Obsługa Błędów</button>
                <button onClick={() => scrollToSection('io')} className="block text-[#9cdcfe] hover:text-white hover:underline text-left w-full">8. Biblioteka Standardowa i I/O</button>
            </>
          )}
        </nav>
      </div>

      {/* Main Content */}
      <div className="flex-1 overflow-y-auto p-8 max-w-4xl mx-auto w-full">
        {mode === 'noob' ? (
            <>
                <h1 className="text-4xl font-bold mb-6 text-[#cccccc]">Witaj w Świecie Benzin! 🚀</h1>
                <p className="text-lg text-[#d4d4d4] mb-8">
                Programowanie to nic innego jak wydawanie poleceń komputerowi. Wyobraź sobie, że komputer to bardzo szybki, ale niezbyt domyślny robot. Musisz mu powiedzieć <strong>dokładnie</strong>, co ma zrobić. Język Benzin służy właśnie do tego!
                </p>
                
                <div className="space-y-12 text-[#d4d4d4]">
                
                {/* 1. Wstęp */}
                <section id="wstep">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">1. Jak rozmawiać z komputerem?</h2>
                    <p className="mb-4">
                    Komputer rozumie proste komendy. Najważniejszą z nich jest <code className="text-[#dcdcaa]">pisz</code>. To tak, jakbyś kazał robotowi coś powiedzieć.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                    <span className="text-[#6a9955]"># To jest komentarz - komputer go ignoruje, to notatka dla Ciebie.</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Cześć! Jestem Twoim programem."</span>;
                    </div>
                    <p className="text-sm text-[#9cdcfe]">
                    Zauważ, że tekst zawsze piszemy w cudzysłowie <code className="text-[#ce9178]">" "</code>, a na końcu linii stawiamy średnik <code className="text-[#d4d4d4]">;</code>. To jak kropka na końcu zdania.
                    </p>
                </section>

                {/* 2. Zmienne */}
                <section id="zmienne">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">2. Zmienne to Pudełka 📦</h2>
                    <p className="mb-4">
                    Wyobraź sobie, że masz pudełko, na którym piszesz markerem nazwę, np. <code className="text-[#9cdcfe]">wiek</code>. Do środka wkładasz liczbę. W programowaniu nazywamy to <strong>zmienną</strong>.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">wiek</span> = <span className="text-[#b5cea8]">10</span>;<br/>
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">imie</span> = <span className="text-[#ce9178]">"Kacper"</span>;<br/><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Mam na imię "</span> + <span className="text-[#9cdcfe]">imie</span>;<br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Mam lat: "</span> + <span className="text-[#9cdcfe]">wiek</span>;
                    </div>
                    <p className="mb-2">Możesz zmieniać zawartość pudełka:</p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#9cdcfe]">wiek</span> = <span className="text-[#9cdcfe]">wiek</span> + <span className="text-[#b5cea8]">1</span>; <span className="text-[#6a9955]"># Teraz w pudełku jest 11!</span>
                    </div>
                </section>

                {/* 3. Decyzje */}
                <section id="decyzje">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">3. Podejmowanie Decyzji (Jeżeli) 🤔</h2>
                    <p className="mb-4">
                    Czasami chcesz, żeby program zrobił coś tylko pod pewnym warunkiem. Np. "Jeżeli pada deszcz, weź parasol".
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">pada_deszcz</span> = <span className="text-[#569cd6]">prawda</span>;<br/><br/>
                    <span className="text-[#c586c0]">jeżeli</span> (<span className="text-[#9cdcfe]">pada_deszcz</span>) {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Biorę parasol!"</span>;<br/>
                    {'}'} <span className="text-[#c586c0]">inaczej</span> {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Idę w samej koszulce."</span>;<br/>
                    {'}'}
                    </div>
                </section>

                {/* 4. Pętle */}
                <section id="petle">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">4. Pętle, czyli Powtarzanie 🔄</h2>
                    <p className="mb-4">
                    Programiści są leniwi i nie lubią pisać tego samego wiele razy. Jeśli chcesz wypisać liczby od 1 do 5, użyj pętli!
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#c586c0]">dla</span> (<span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">i</span> = <span className="text-[#b5cea8]">1</span>; <span className="text-[#9cdcfe]">i</span> &lt;= <span className="text-[#b5cea8]">5</span>; <span className="text-[#9cdcfe]">i</span>++) {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"To jest numer: "</span> + <span className="text-[#9cdcfe]">i</span>;<br/>
                    {'}'}
                    </div>
                    <p className="mt-2 text-sm text-[#9cdcfe]">
                    <code className="text-[#9cdcfe]">i++</code> oznacza "zwiększ <code className="text-[#9cdcfe]">i</code> o 1".
                    </p>

                    <h3 className="text-lg font-semibold text-[#9cdcfe] mt-4 mb-2">Pętla "Dla Każdego" (Foreach)</h3>
                    <p className="mb-4">
                    Jeśli masz tablicę (półkę) i chcesz wziąć każdą rzecz po kolei, użyj pętli <code className="text-[#c586c0]">dla ... w ...</code>.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">koszyk</span> = [<span className="text-[#ce9178]">"Jabłko"</span>, <span className="text-[#ce9178]">"Gruszka"</span>, <span className="text-[#ce9178]">"Śliwka"</span>];<br/><br/>
                    <span className="text-[#c586c0]">dla</span> (<span className="text-[#9cdcfe]">owoc</span> <span className="text-[#c586c0]">w</span> <span className="text-[#9cdcfe]">koszyk</span>) {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Mam: "</span> + <span className="text-[#9cdcfe]">owoc</span>;<br/>
                    {'}'}
                    </div>
                    <p className="mb-4">
                    Możesz też przeglądać obiekty (plecaki). Wtedy dostaniesz nazwy kieszonek (klucze).
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">auto</span> = {'{'} <span className="text-[#ce9178]">"marka"</span>: <span className="text-[#ce9178]">"Fiat"</span>, <span className="text-[#ce9178]">"kolor"</span>: <span className="text-[#ce9178]">"Czerwony"</span> {'}'};<br/><br/>
                    <span className="text-[#c586c0]">dla</span> (<span className="text-[#9cdcfe]">cecha</span> <span className="text-[#c586c0]">w</span> <span className="text-[#9cdcfe]">auto</span>) {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Cecha: "</span> + <span className="text-[#9cdcfe]">cecha</span>;<br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Wartość: "</span> + <span className="text-[#9cdcfe]">auto</span>[<span className="text-[#9cdcfe]">cecha</span>];<br/>
                    {'}'}
                    </div>
                </section>

                {/* 5. Funkcje */}
                <section id="funkcje">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">5. Funkcje to Przepisy 📜</h2>
                    <p className="mb-4">
                    Funkcja to taki mały program w programie. To jak przepis na ciasto. Raz go zapisujesz, a potem możesz piec ciasto wiele razy.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#6a9955]"># Tworzymy przepis na powitanie</span><br/>
                    <span className="text-[#569cd6]">funkcja</span> <span className="text-[#dcdcaa]">przywitaj</span>(<span className="text-[#9cdcfe]">kto</span>) {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Cześć, "</span> + <span className="text-[#9cdcfe]">kto</span> + <span className="text-[#ce9178]">"!"</span>;<br/>
                    {'}'}<br/><br/>
                    <span className="text-[#6a9955]"># Używamy przepisu</span><br/>
                    <span className="text-[#dcdcaa]">przywitaj</span>(<span className="text-[#ce9178]">"Ala"</span>);<br/>
                    <span className="text-[#dcdcaa]">przywitaj</span>(<span className="text-[#ce9178]">"Tomek"</span>);
                    </div>
                </section>

                {/* 6. Tablice */}
                <section id="tablice">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">6. Tablice to Półki 📚</h2>
                    <p className="mb-4">
                    Czasami masz dużo rzeczy tego samego typu, np. listę zakupów. Zamiast tworzyć zmienną <code className="text-[#9cdcfe]">rzecz1</code>, <code className="text-[#9cdcfe]">rzecz2</code>, użyj <strong>tablicy</strong>. To jak długa półka z numerowanymi przegródkami.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">plecak</span> = [<span className="text-[#ce9178]">"Kanapka"</span>, <span className="text-[#ce9178]">"Woda"</span>, <span className="text-[#ce9178]">"Mapa"</span>];<br/><br/>
                    <span className="text-[#6a9955]"># Komputery liczą od zera!</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">plecak</span>[<span className="text-[#b5cea8]">0</span>]; <span className="text-[#6a9955]"># Wypisze "Kanapka"</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">plecak</span>[<span className="text-[#b5cea8]">1</span>]; <span className="text-[#6a9955]"># Wypisze "Woda"</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">plecak</span>.<span className="text-[#9cdcfe]">dlugosc</span>; <span className="text-[#6a9955]"># Wypisze 3</span>
                    </div>
                </section>

                {/* 7. Obiekty */}
                <section id="obiekty">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">7. Obiekty to Zestawy 🎒</h2>
                    <p className="mb-4">
                    Tablica ma numerki (0, 1, 2...), ale czasem wolisz nazwać rzeczy. Obiekt to jak plecak z podpisanymi kieszonkami.
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">gracz</span> = {'{'} <br/>
                    &nbsp;&nbsp;<span className="text-[#ce9178]">"nick"</span>: <span className="text-[#ce9178]">"SuperBohater"</span>,<br/>
                    &nbsp;&nbsp;<span className="text-[#ce9178]">"poziom"</span>: <span className="text-[#b5cea8]">5</span>,<br/>
                    &nbsp;&nbsp;<span className="text-[#ce9178]">"zycie"</span>: <span className="text-[#b5cea8]">100</span><br/>
                    {'}'};<br/><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Gracz: "</span> + <span className="text-[#9cdcfe]">gracz</span>.<span className="text-[#9cdcfe]">nick</span>;<br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Poziom: "</span> + <span className="text-[#9cdcfe]">gracz</span>.<span className="text-[#9cdcfe]">poziom</span>;
                    </div>
                </section>

                {/* 8. Moduły */}
                <section id="moduly">
                    <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">8. Moduły (Pożyczanie Narzędzi) 🛠️</h2>
                    <p className="mb-4">
                    Nie musisz pisać wszystkiego sam. Benzin ma gotowe zestawy narzędzi, np. do matematyki. Żeby ich użyć, musisz je "zaimportować" (pożyczyć).
                    </p>
                    <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                    <span className="text-[#c586c0]">import</span> <span className="text-[#ce9178]">"Matma"</span>;<br/><br/>
                    <span className="text-[#6a9955]"># Teraz możemy używać narzędzi z pudełka "Matma"</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">pierwiastek</span>(<span className="text-[#b5cea8]">16</span>); <span className="text-[#6a9955]"># Wypisze 4</span><br/>
                    <span className="text-[#dcdcaa]">pisz</span> <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">losuj</span>(); <span className="text-[#6a9955]"># Wylosuje liczbę</span>
                    </div>
                </section>
                </div>
            </>
        ) : (
            <>
                <h1 className="text-4xl font-bold mb-6 text-[#cccccc]">Dokumentacja Techniczna Benzin 🛠️</h1>
                <p className="text-lg text-[#d4d4d4] mb-8">
                    Pełna specyfikacja języka Benzin dla programistów. Język jest dynamicznie typowany, interpretowany i wspiera paradygmaty imperatywne oraz obiektowe.
                </p>

                <div className="space-y-12 text-[#d4d4d4]">
                    
                    {/* 1. Składnia */}
                    <section id="skladnia">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">1. Składnia i Typy Danych</h2>
                        <p className="mb-4">Benzin obsługuje następujące typy proste i złożone:</p>
                        <ul className="list-disc list-inside mb-4 space-y-1 text-[#9cdcfe]">
                            <li><span className="text-[#ce9178]">Liczby</span> (double precision float): <code className="text-[#b5cea8]">10</code>, <code className="text-[#b5cea8]">3.14</code>, <code className="text-[#b5cea8]">-5</code></li>
                            <li><span className="text-[#ce9178]">Napisy</span> (string): <code className="text-[#ce9178]">"Tekst"</code> (właściwość <code className="text-[#9cdcfe]">.dlugosc</code>)</li>
                            <li><span className="text-[#ce9178]">Logiczne</span> (boolean): <code className="text-[#569cd6]">prawda</code>, <code className="text-[#569cd6]">falsz</code></li>
                            <li><span className="text-[#ce9178]">Null</span>: <code className="text-[#569cd6]">nic</code></li>
                            <li><span className="text-[#ce9178]">Tablice</span>: <code className="text-[#d4d4d4]">[1, 2, 3]</code> (właściwość <code className="text-[#9cdcfe]">.dlugosc</code>)</li>
                            <li><span className="text-[#ce9178]">Słowniki</span>: <code className="text-[#d4d4d4]">{'{ "klucz": "wartość" }'}</code></li>
                        </ul>
                    </section>

                    {/* 2. Zmienne */}
                    <section id="zmienne-dev">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">2. Zmienne</h2>
                        <p className="mb-4">Deklaracja zmiennych odbywa się za pomocą słowa kluczowego <code className="text-[#569cd6]">zmienna</code>.</p>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">x</span> = <span className="text-[#b5cea8]">10</span>;<br/>
                            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">y</span> = <span className="text-[#ce9178]">"Tekst"</span>;<br/>
                            <span className="text-[#9cdcfe]">x</span> = <span className="text-[#b5cea8]">20</span>; <span className="text-[#6a9955]"># Przypisanie</span>
                        </div>
                    </section>

                    {/* 3. Operatory */}
                    <section id="operatory">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">3. Operatory</h2>
                        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                            <div>
                                <h3 className="font-bold text-[#9cdcfe] mb-2">Arytmetyczne</h3>
                                <ul className="list-disc list-inside text-sm">
                                    <li><code className="text-[#d4d4d4]">+</code> : Dodawanie / Konkatenacja</li>
                                    <li><code className="text-[#d4d4d4]">-</code> : Odejmowanie</li>
                                    <li><code className="text-[#d4d4d4]">*</code> : Mnożenie</li>
                                    <li><code className="text-[#d4d4d4]">/</code> : Dzielenie</li>
                                    <li><code className="text-[#d4d4d4]">%</code> : Modulo</li>
                                    <li><code className="text-[#d4d4d4]">++</code>, <code className="text-[#d4d4d4]">--</code> : Inkrementacja/Dekrementacja</li>
                                </ul>
                            </div>
                            <div>
                                <h3 className="font-bold text-[#9cdcfe] mb-2">Logiczne i Porównania</h3>
                                <ul className="list-disc list-inside text-sm">
                                    <li><code className="text-[#d4d4d4]">==</code> (<code className="text-[#569cd6]">rowne</code>) : Równość</li>
                                    <li><code className="text-[#d4d4d4]">!=</code> (<code className="text-[#569cd6]">nierowne</code>) : Nierówność</li>
                                    <li><code className="text-[#d4d4d4]">&lt;</code>, <code className="text-[#d4d4d4]">&gt;</code>, <code className="text-[#d4d4d4]">&lt;=</code>, <code className="text-[#d4d4d4]">&gt;=</code></li>
                                    <li><code className="text-[#d4d4d4]">&&</code> (<code className="text-[#569cd6]">oraz</code>) : Koniunkcja</li>
                                    <li><code className="text-[#d4d4d4]">||</code> (<code className="text-[#569cd6]">albo</code>) : Alternatywa</li>
                                    <li><code className="text-[#d4d4d4]">!</code> : Negacja</li>
                                </ul>
                            </div>
                            <div>
                                <h3 className="font-bold text-[#9cdcfe] mb-2">Bitowe</h3>
                                <ul className="list-disc list-inside text-sm">
                                    <li><code className="text-[#d4d4d4]">&</code> : Bitowe AND</li>
                                    <li><code className="text-[#d4d4d4]">|</code> : Bitowe OR</li>
                                    <li><code className="text-[#d4d4d4]">^</code> : Bitowe XOR</li>
                                    <li><code className="text-[#d4d4d4]">~</code> : Bitowe NOT</li>
                                    <li><code className="text-[#d4d4d4]">&lt;&lt;</code>, <code className="text-[#d4d4d4]">&gt;&gt;</code> : Przesunięcia bitowe</li>
                                </ul>
                            </div>
                        </div>
                    </section>

                    {/* 4. Sterowanie Przepływem */}
                    <section id="control-flow">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">4. Sterowanie Przepływem</h2>
                        
                        <h3 className="text-lg font-semibold text-[#9cdcfe] mt-4 mb-2">Instrukcja Warunkowa</h3>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                            <span className="text-[#c586c0]">jeżeli</span> (warunek) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#6a9955]"># kod</span><br/>
                            {'}'} <span className="text-[#c586c0]">inaczej jeżeli</span> (inny_warunek) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#6a9955]"># kod</span><br/>
                            {'}'} <span className="text-[#c586c0]">inaczej</span> {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#6a9955]"># kod</span><br/>
                            {'}'}
                        </div>

                        <h3 className="text-lg font-semibold text-[#9cdcfe] mt-4 mb-2">Pętle</h3>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                            <span className="text-[#6a9955]"># Pętla While</span><br/>
                            <span className="text-[#c586c0]">wkolko</span> (warunek) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">zlam</span>; <span className="text-[#6a9955]"># break</span><br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">pomin</span>; <span className="text-[#6a9955]"># continue</span><br/>
                            {'}'}<br/><br/>
                            <span className="text-[#6a9955]"># Pętla For</span><br/>
                            <span className="text-[#c586c0]">dla</span> (<span className="text-[#569cd6]">zmienna</span> i = <span className="text-[#b5cea8]">0</span>; i &lt; <span className="text-[#b5cea8]">10</span>; i++) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> i;<br/>
                            {'}'}<br/><br/>
                            <span className="text-[#6a9955]"># Pętla Foreach</span><br/>
                            <span className="text-[#c586c0]">dla</span> (<span className="text-[#9cdcfe]">element</span> <span className="text-[#c586c0]">w</span> <span className="text-[#9cdcfe]">kolekcja</span>) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#9cdcfe]">element</span>;<br/>
                            {'}'}
                        </div>
                        <p className="text-sm text-[#9cdcfe] mb-1">
                            Pętla <code className="text-[#c586c0]">dla..w</code> iteruje po:
                        </p>
                        <ul className="list-disc list-inside ml-4 mt-1 text-sm text-[#9cdcfe] mb-4">
                            <li><strong>Tablicach</strong>: zwraca wartości elementów.</li>
                            <li><strong>Słownikach</strong>: zwraca klucze (string).</li>
                            <li><strong>Napisach</strong>: zwraca pojedyncze znaki.</li>
                        </ul>

                        <h3 className="text-lg font-semibold text-[#9cdcfe] mt-4 mb-2">Switch / Case</h3>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#c586c0]">wybor</span> (zmienna) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">przypadek</span> <span className="text-[#b5cea8]">1</span>:<br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Jeden"</span>;<br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">przypadek</span> <span className="text-[#b5cea8]">2</span>:<br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Dwa"</span>;<br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">domyslnie</span>:<br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Inna liczba"</span>;<br/>
                            {'}'}
                        </div>
                    </section>

                    {/* 5. Funkcje */}
                    <section id="funkcje-dev">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">5. Funkcje</h2>
                        <p className="mb-4">Funkcje są obiektami pierwszej klasy (first-class citizens).</p>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#569cd6]">funkcja</span> <span className="text-[#dcdcaa]">dodaj</span>(a, b) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">zwroc</span> a + b;<br/>
                            {'}'}<br/><br/>
                            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">wynik</span> = <span className="text-[#dcdcaa]">dodaj</span>(<span className="text-[#b5cea8]">5</span>, <span className="text-[#b5cea8]">3</span>);
                        </div>
                    </section>

                    {/* 6. OOP */}
                    <section id="oop">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">6. Programowanie Obiektowe</h2>
                        <p className="mb-4">Benzin wspiera klasy, dziedziczenie, metody statyczne oraz modyfikatory dostępu.</p>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#569cd6]">klasa</span> <span className="text-[#4ec9b0]">Pojazd</span> {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#569cd6]">publiczna</span> <span className="text-[#dcdcaa]">konstruktor</span>(marka) {'{'} <br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#569cd6]">to</span>.<span className="text-[#9cdcfe]">marka</span> = marka;<br/>
                            &nbsp;&nbsp;{'}'}<br/><br/>
                            &nbsp;&nbsp;<span className="text-[#569cd6]">publiczna</span> <span className="text-[#dcdcaa]">jedz</span>() {'{'} <br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Brum brum"</span>;<br/>
                            &nbsp;&nbsp;{'}'}<br/>
                            {'}'}<br/><br/>
                            <span className="text-[#569cd6]">klasa</span> <span className="text-[#4ec9b0]">Auto</span> <span className="text-[#569cd6]">dziedziczy</span> <span className="text-[#4ec9b0]">Pojazd</span> {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#569cd6]">statyczna</span> <span className="text-[#dcdcaa]">info</span>() {'{'} <br/>
                            &nbsp;&nbsp;&nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"To jest klasa Auto"</span>;<br/>
                            &nbsp;&nbsp;{'}'}<br/>
                            {'}'}<br/><br/>
                            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">mojeAuto</span> = <span className="text-[#569cd6]">nowy</span> <span className="text-[#4ec9b0]">Auto</span>(<span className="text-[#ce9178]">"Fiat"</span>);
                        </div>
                    </section>

                    {/* 7. Obsługa Błędów */}
                    <section id="errors">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">7. Obsługa Błędów</h2>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#c586c0]">sprobuj</span> {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#c586c0]">rzuc</span> <span className="text-[#ce9178]">"Coś poszło nie tak"</span>;<br/>
                            {'}'} <span className="text-[#c586c0]">zlap</span> (blad) {'{'} <br/>
                            &nbsp;&nbsp;<span className="text-[#dcdcaa]">pisz</span> <span className="text-[#ce9178]">"Złapano błąd: "</span> + blad;<br/>
                            {'}'}
                        </div>
                    </section>

                    {/* 8. IO */}
                    <section id="io">
                        <h2 className="text-2xl font-semibold mb-4 text-[#569cd6] border-b border-[#3e3e42] pb-2">8. Biblioteka Standardowa i I/O</h2>
                        
                        <h3 className="font-bold text-[#9cdcfe] mb-2">Wejście / Wyjście</h3>
                        <ul className="list-disc list-inside mb-4 space-y-1 text-[#9cdcfe]">
                            <li><code className="text-[#dcdcaa]">pisz</code> <span className="text-[#ce9178]">"tekst"</span> - Wypisuje tekst na standardowe wyjście.</li>
                            <li><code className="text-[#c586c0]">podaj</code> <span className="text-[#9cdcfe]">zmienna</span> - Pobiera dane od użytkownika.</li>
                        </ul>

                        <h3 className="font-bold text-[#9cdcfe] mb-2">Matematyka (Moduł Matma)</h3>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm mb-4">
                            <span className="text-[#c586c0]">import</span> <span className="text-[#ce9178]">"Matma"</span>;<br/><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">sin</span>(x); <span className="text-[#6a9955]"># Sinus</span><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">cos</span>(x); <span className="text-[#6a9955]"># Cosinus</span><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">pierwiastek</span>(x); <span className="text-[#6a9955]"># Pierwiastek kwadratowy</span><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">potega</span>(podstawa, wykladnik); <span className="text-[#6a9955]"># Potęgowanie</span><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">losuj</span>(); <span className="text-[#6a9955]"># Losowa liczba 0.0 - 1.0</span><br/>
                            <span className="text-[#4ec9b0]">Matma</span>.<span className="text-[#dcdcaa]">czas</span>(); <span className="text-[#6a9955]"># Aktualny czas (timestamp)</span>
                        </div>

                        <h3 className="font-bold text-[#9cdcfe] mb-2">Operacje na plikach (Moduł Plik)</h3>
                        <div className="bg-[#1e1e1e] border border-[#2d2d2d] p-4 rounded-md font-mono text-sm">
                            <span className="text-[#c586c0]">import</span> <span className="text-[#ce9178]">"Plik"</span>;<br/><br/>
                            <span className="text-[#4ec9b0]">Plik</span>.<span className="text-[#dcdcaa]">zapisz</span>(<span className="text-[#ce9178]">"dane.txt"</span>, <span className="text-[#ce9178]">"Treść"</span>);<br/>
                            <span className="text-[#569cd6]">zmienna</span> <span className="text-[#9cdcfe]">tresc</span> = <span className="text-[#4ec9b0]">Plik</span>.<span className="text-[#dcdcaa]">czytaj</span>(<span className="text-[#ce9178]">"dane.txt"</span>);<br/>
                            <span className="text-[#4ec9b0]">Plik</span>.<span className="text-[#dcdcaa]">dopisz</span>(<span className="text-[#ce9178]">"dane.txt"</span>, <span className="text-[#ce9178]">"\nNowa linia"</span>);
                        </div>
                    </section>

                </div>
            </>
        )}
      </div>
    </div>
  );
}
