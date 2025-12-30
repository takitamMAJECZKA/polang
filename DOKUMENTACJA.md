# Dokumentacja Języka Benzin (MyLang)

Witaj w dokumentacji języka **Benzin** (wcześniej znanego jako MyLang). Jest to interpretowany język programowania o składni zbliżonej do C/JavaScript, ale z polskimi słowami kluczowymi.

## Spis Treści
1. [Podstawy](#1-podstawy)
2. [Zmienne i Typy Danych](#2-zmienne-i-typy-danych)
3. [Operatory](#3-operatory)
4. [Instrukcje Sterujące](#4-instrukcje-sterujące)
5. [Funkcje](#5-funkcje)
6. [Struktury Danych](#6-struktury-danych)
7. [Programowanie Obiektowe](#7-programowanie-obiektowe)
8. [Biblioteka Standardowa](#8-biblioteka-standardowa)

---

## 1. Podstawy

### Komentarze
Język wspiera trzy rodzaje komentarzy:
```mylang
# Komentarz jednoliniowy (styl Python/Bash)
// Komentarz jednoliniowy (styl C)
/* 
   Komentarz
   wieloliniowy 
*/
```

### Wyjście i Wejście
Do wypisywania danych służy instrukcja `pisz`, a do pobierania danych `podaj`.

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
zmienna liczba_calkowita = 42;
zmienna liczba_zmiennoprzecinkowa = 3.14;
zmienna tekst = "To jest napis";
zmienna prawda_falsz = prawda;  # lub falsz
zmienna pustka = nic;
```

### Zmiana wartości zmiennej
Gdy zmienna jest już zadeklarowana (użyto słowa `zmienna`), możesz zmieniać jej wartość bez ponownego używania tego słowa.

```mylang
zmienna x = 5;
pisz x; # 5

x = 10; # Zmiana wartości
pisz x; # 10
```

### Interpolacja ciągów znaków
Możesz wstawiać wartości zmiennych bezpośrednio do tekstu używając `${...}`.

```mylang
zmienna x = 10;
pisz "X wynosi ${x}, a x+1 to ${x+1}";
```

---

## 3. Operatory

### Arytmetyczne
| Operator | Opis | Przykład |
|----------|------|----------|
| `+` | Dodawanie | `2 + 2` |
| `-` | Odejmowanie | `5 - 3` |
| `*` | Mnożenie | `4 * 2` |
| `/` | Dzielenie | `10 / 2` |
| `%` | Reszta z dzielenia | `10 % 3` |
| `++` | Inkrementacja | `i++` |
| `--` | Dekrementacja | `i--` |

### Logiczne i Porównania
| Operator | Opis |
|----------|------|
| `==` lub `rowne` | Równość |
| `!=` lub `nierowne` | Nierówność |
| `<` | Mniejsze |
| `>` | Większe |
| `<=` | Mniejsze lub równe |
| `>=` | Większe lub równe |
| `&&` lub `oraz` | Koniunkcja logiczna (AND) |
| `||` lub `albo` | Alternatywa logiczna (OR) |
| `!` | Negacja logiczna (NOT) |

### Negacja (`!`)
Znak `!` odwraca wartość logiczną. Prawda staje się fałszem, a fałsz prawdą.

```mylang
zmienna x = prawda;
pisz !x; # Wypisze: falsz

jezeli (!x) {
    pisz "To się nie wykona";
}
```

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

### Pętla `wkolko` (While)
```mylang
zmienna i = 0;
wkolko (i < 5) {
    pisz i;
    i++;
}
```

### Pętla `dla` (For)
```mylang
dla (zmienna i = 0; i < 10; i++) {
    jezeli (i == 5) { pomin; } # continue
    jezeli (i == 8) { zlam; }  # break
    pisz i;
}
```

### Pętla `dla` (Foreach)
Możesz łatwo iterować po elementach tablicy używając składni `dla ... w ...`.

```mylang
zmienna owoce = ["Jablko", "Banan", "Gruszka"];

dla (owoc w owoce) {
    pisz "Lubie: " + owoc;
}
```

---

## 5. Funkcje

Funkcje definiujemy słowem `funkcja`. Mogą zwracać wartości przy użyciu `zwroc`.

```mylang
funkcja powitanie(imie) {
    zwroc "Cześć " + imie;
}

pisz powitanie("Adam");
```

### Funkcje anonimowe i Domknięcia (Closures)
Funkcje mogą być przypisywane do zmiennych i zwracane z innych funkcji.

```mylang
funkcja generator(x) {
    funkcja dodaj(y) {
        zwroc x + y;
    }
    zwroc dodaj;
}

zmienna dodaj5 = generator(5);
pisz dodaj5(10); # Wypisze 15
```

---

## 6. Struktury Danych

### Tablice
Tablice są indeksowane od 0.

```mylang
zmienna liczby = [10, 20, 30];
pisz liczby[1]; # 20

# Metody tablic
liczby.dodaj(40);
liczby.usun(0); # Usuwa element pod indeksem 0
```

### Słowniki (Obiekty)
Pary klucz-wartość. Klucze muszą być napisami.

```mylang
zmienna osoba = {
    "imie": "Jan",
    wiek: 30        # Klucz może być bez cudzysłowia
};

pisz osoba.imie;      # Dostęp przez kropkę
pisz osoba["wiek"];   # Dostęp przez nawias
```

---

## 7. Programowanie Obiektowe

Język wspiera klasy, dziedziczenie i metody.

```mylang
klasa Pojazd {
    konstruktor(marka) {
        to.marka = marka; # 'to' oznacza 'this'
    }
    
    jedz() {
        pisz "${to.marka} jedzie...";
    }
}

klasa Samochod dziedziczy Pojazd {
    konstruktor(marka, model) {
        to.marka = marka;
        to.model = model;
    }
    
    trab() {
        pisz "Bip bip!";
    }
}

zmienna auto = nowy Samochod("Fiat", "126p");
auto.jedz();
auto.trab();
```

---

## 8. Biblioteka Standardowa

### Konwersja Typów
*   `tekst(wartosc)` - zamienia liczbę na napis.
*   `liczba(napis)` - zamienia napis na liczbę.

### Obiekt `Matma`
*   `Matma.PI` - stała PI.
*   `Matma.sin(x)` - sinus.
*   `Matma.cos(x)` - cosinus.
*   `Matma.pierwiastek(x)` - pierwiastek kwadratowy.
*   `Matma.potega(podstawa, wykladnik)` - potęgowanie.
*   `Matma.losuj()` - losowa liczba 0.0 - 1.0.
*   `Matma.czas()` - aktualny czas (timestamp).

### Obiekt `Plik`
*   `Plik.czytaj("sciezka")` - zwraca zawartość pliku jako tekst.
*   `Plik.zapisz("sciezka", "tresc")` - nadpisuje plik.
*   `Plik.dopisz("sciezka", "tresc")` - dopisuje do pliku.

### Metody dla Napisów (String)
Dostępne jako metody na zmiennych tekstowych:
*   `.wGore()` - zamienia na wielkie litery.
*   `.wDol()` - zamienia na małe litery.
*   `.wytnij(start, dlugosc)` - zwraca fragment tekstu.
*   `.podziel("separator")` - zwraca tablicę fragmentów.

```mylang
zmienna s = "ala ma kota";
pisz s.wGore(); # ALA MA KOTA
pisz s.podziel(" "); # ["ala", "ma", "kota"]
```
