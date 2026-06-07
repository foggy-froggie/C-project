# Packup: Optymalizator rozkładu wagi na wyjazdy górskie

### Problem: Optymalny rozkład obciążenia
Podczas wielodniowych wypraw górskich, wyzwaniem jest sprawiedliwe rozdzielenie sprzętu wspólnego i jedzenia między uczestników o różnej wydolności. Jest to czasochłonne i zrobione nieprawidłowo prowadzi do nieprzyjemnych chodź słusznych komentarzy. 
Problem posiada dwie kluczowe cechy:
Różne łądunki:
 - Ładunki ciągłe: Ekwipunek niezbędny przez cały czas trwania wyprawy (np. piła, garnek).
 - Ładunki interwałowe: Zapasy zużywalne (np. jedzenie na konkretny dzień), które obciążają plecak tylko do momentu ich konsumpcji.
Pakowanie następuje raz – przed wyjazdem. Nie możliwości redystrybucji sprzętu w trakcie wyprawy.

### Model matematyczny
Problem sprowadzamy do minimalizacji maksymalnego obciążenia.
- Niech $P = \{p_1, p_2, ..., p_n\}$ będzie zbiorem pakunków.
- Niech $T = \{t_1, t_2, ..., t_n\}$ będzie czasem trwania (dni) każdego pakunku (np. śniadanie na dzień 2 będzie miało wartość 2) 
- Niech $S = \{s_1, s_2, ..., s_m\}$ będzie zbiorem wytrzymałości uczestników.Celem jest takie przypisanie $p_i \rightarrow s_j$, aby zminimalizować wartość $\max(load_j)$ dla każdego uczestnika $j$, przy uwzględnieniu czasu, przez jaki dany pakunek obciąża plecak.

### Rozwiązanie Algorytm zachłanny z kopcem (Min-Heap)
Aby rozwiązać ten problem zaimplementowałam algorytm zachłanny:
Przedmioty są sortowane malejąco według ich czasu trwania i wag (w tej kolejności). Używając kopca w każdym kroku identyfikuję osobę, która ma w danej chwili najmniejsze obciążenie skumulowane (liczone jako waga x czas trwania / wytrzymałośc).Przedmiot jest przypisywany do tej osoby, a wartość jej obciążenia w kopcu jest aktualizowana.