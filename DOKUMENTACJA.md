# Dokumentacja Języka Benzin (MyLang)

Witaj w dokumentacji języka **Benzin**. Jest to interpretowany język programowania o składni zbliżonej do C/JavaScript, ale z polskimi słowami kluczowymi.

## Spis Treści
1. [Podstawy](#1-podstawy)
2. [Zmienne i Typy Danych](#2-zmienne-i-typy-danych)
3. [Operatory](#3-operatory)
4. [Instrukcje Sterujące](#4-instrukcje-sterujące)
5. [Funkcje](#5-funkcje)
6. [Struktury Danych](#6-struktury-danych)
7. [Programowanie Obiektowe](#7-programowanie-obiektowe)
8. [Obsługa Błędów](#8-obsługa-błędów)
9. [Moduły](#9-moduły)
10. [Biblioteka Standardowa](#10-biblioteka-standardowa)

---

## 1. Podstawy

### Komentarze
```mylang
# Komentarz jednoliniowy (styl Python/Bash)
// Komentarz jednoliniowy (styl C)
/* 
   Komentarz
   wieloliniowy 
*/
```

### Wyjście i Wejście
```mylang
pisz "Witaj świecie!";
pisz "Wartość: " + 10;

zmienna imie;
pisz "Podaj swoje imię:";
podaj imie;
pisz "Cześć, ${imie}!";
```

---

## 2. Zmienne i Typy Danych

Zmienne deklarujemy słowem kluczowym `zmienna`. Typy są dynamiczne.

```mylang
zmienna liczba = 42;
zmienna zmiennoprzecinkowa = 3.14;
zmienna tekst = "Napis";
zmienna prawda_falsz = prawda;  # lub falsz
zmienna pustka = nic;           # null
```

### Interpolacja
```mylang
zmienna x = 10;
pisz "X wynosi ${x}";
```

---

## 3. Operatory

### Arytmetyczne
| Operator | Opis |
|----------|------|
| `+` | Dodawanie |
| `-` | Odejmowanie |
| `*` | Mnożenie |
| `/` | Dzielenie |
| `%` | Reszta z dzielenia |
| `++` | Inkrementacja |
| `--` | Dekrementacja |

### Przypisania Złożonego
| Operator | Opis |
|----------|------|
| `+=` | Dodaj i przypisz |
| `-=` | Odejmij i przypisz |
| `*=` | Pomnóż i przypisz |
| `/=` | Podziel i przypisz |
| `%=` | Reszta i przypisz |

### Logiczne i Porównania
| Operator | Opis |
|----------|------|
| `==` lub `rowne` | Równość |
| `!=` lub `nierowne` | Nierówność |
| `<` | Mniejsze |
| `>` | Większe |
| `<=` | Mniejsze lub równe |
| `>=` | Większe lub równe |
| `&&` lub `oraz` | Koniunkcja (AND) |
| `||` lub `albo` | Alternatywa (OR) |
| `!` | Negacja (NOT) |

### Bitowe
| Operator | Opis |
|----------|------|
| `&` | Bitowe AND |
| `|` | Bitowe OR |
| `^` | Bitowe XOR |
| `~` | Bitowe NOT |
| `<<` | Przesunięcie w lewo |
| `>>` | Przesunięcie w prawo |

---

## 4. Instrukcje Sterujące

### Instrukcja Warunkowa `jezeli`
```mylang
jezeli (x > 0) {
    pisz "Dodatnia";
} inaczej {
    pisz "Niedodatnia";
}
```

### Instrukcja Wyboru `wybor` (Switch)
```mylang
wybor (x) {
    przypadek 1:
        pisz "Jeden";
        zlam;
    przypadek 2:
        pisz "Dwa";
        zlam;
    domyslnie:
        pisz "Inna liczba";
}
```

### Pętle
**While (`wkolko`):**
```mylang
wkolko (i < 5) {
    pisz i;
    i++;
}
```

**For (`dla`):**
```mylang
dla (zmienna i = 0; i < 10; i++) {
    jezeli (i == 5) { pomin; } # continue
    jezeli (i == 8) { zlam; }  # break
    pisz i;
}
```

**Foreach (`dla ... w ...`):**
```mylang
dla (element w kolekcja) {
    pisz element;
}
```

---

## 5. Funkcje

```mylang
funkcja dodaj(a, b) {
    zwroc a + b;
}

pisz dodaj(2, 3);
```

---

## 6. Struktury Danych

### Tablice
```mylang
zmienna arr = [1, 2, 3];
pisz arr[0]; # 1
pisz arr.dlugosc; # 3

arr.dodaj(4);    # Dodaje na koniec
arr.usun(0);     # Usuwa element pod indeksem 0
```

### Słowniki
```mylang
zmienna obj = { "imie": "Jan", "wiek": 30 };
pisz obj.imie;
pisz obj["wiek"];
```

---

## 7. Programowanie Obiektowe

```mylang
klasa Zwierze {
    konstruktor(nazwa) {
        to.nazwa = nazwa;
    }
    dajGlos() {
        pisz "Muuu";
    }
}

klasa Pies dziedziczy Zwierze {
    dajGlos() {
        pisz "Hau hau";
    }
}

zmienna p = nowy Pies("Burek");
p.dajGlos();
```

---

## 8. Obsługa Błędów

```mylang
sprobuj {
    rzuc "Wystąpił błąd!";
} zlap (blad) {
    pisz "Złapano: " + blad;
}
```

---

## 9. Moduły

Możesz importować kod z innych plików.
```mylang
import "biblioteka.bzn";
```

---

## 10. Biblioteka Standardowa

### Napisy (String)
Metody dostępne na każdym napisie:
*   `.dlugosc` - długość napisu (właściwość).
*   `.wGore()` - zamienia na wielkie litery.
*   `.wDol()` - zamienia na małe litery.
*   `.wytnij(start, dlugosc)` - zwraca fragment.
*   `.podziel(separator)` - dzieli napis na tablicę.

### Matematyka (`Matma`)
> **Uwaga:** Wymaga zaimportowania modułu: `import "Matma";`

*   `Matma.PI`
*   `Matma.sin(x)`, `Matma.cos(x)`
*   `Matma.pierwiastek(x)`
*   `Matma.potega(podstawa, wykladnik)`
*   `Matma.losuj()` - losowa 0.0-1.0
*   `Matma.czas()` - timestamp

### Pliki (`Plik`)
> **Uwaga:** Wymaga zaimportowania modułu: `import "Plik";`

*   `Plik.czytaj("sciezka")`
*   `Plik.zapisz("sciezka", "tresc")`
*   `Plik.dopisz("sciezka", "tresc")`
*   `Plik.istnieje("sciezka")`
*   `Plik.usun("sciezka")`

### Funkcje Globalne
Dostępne bez importowania żadnych modułów:

*   `wejscie()` - pobiera linię tekstu od użytkownika.
*   `losuj(min, max)` - zwraca losową liczbę z zakresu [min, max). Bez argumentów zwraca 0.0-1.0.
*   `typ(x)` - zwraca typ zmiennej jako napis ("liczba", "napis", "tablica", "slownik", "logiczna", "funkcja", "obiekt", "nic").
*   `napis(x)` lub `tekst(x)` - konwertuje wartość na napis.
*   `liczba(x)` - konwertuje napis na liczbę.

### Metody Tablic
*   `.dodaj(element)` - dodaje element na koniec tablicy.
*   `.usun(index)` - usuwa element pod danym indeksem.
*   `.dlugosc` - zwraca liczbę elementów.

### Metody Napisów
*   `.zawiera(tekst)` - sprawdza czy napis zawiera podany tekst (zwraca prawda/falsz).
*   `.dlugosc` - zwraca długość napisu.

