Ziemię odwiedzają kosmici, zachwyceni uroczą zaściankowością naszej planety. Po przybyciu zajmują jedno z miejsc w jednym z H hoteli, następnie wynajmują jednego z przewodników, ruszają na wyprawę, po powrocie zwalniają miejsce z hotelu. Problem jest tylko w tym, że są to okropnie drażliwi kosmici, należący do jednej z dwóch fakcji: Fioletowych oraz Błękitnych. Błękitni i Fioletowi nienawidzą się wzajemnie i dlatego nie może dojść do sytuacji, w której w hotelu równocześnie przebywaliby Błękitni i Fioletowi - należy zapewnić, że w danym hotelu zawsze jest tylko jedna fakcja - ale nie wolno przypisać hotelu na sztywno do jednej z fakcji. Co jakiś czas hotele muszą być sprzątane przez ludzkie ekipy sprzątające.
Modelujemy procesy Fiolotewe, Błękitne, oraz Sprzątaczy.

Obrażalscy kosmici (H - liczba hoteli, B - liczba błękitnych kosmitów, F - liczba fioletowych kosmitów, S - liczba sprzątaczy P - liczba przewodnikow, M – liczba miejsc w hotelu)

1. Wartość zegara logicznego procesu j-tego zainicjalizowana jako 0. Wartości wektora znaczników czasowych ostatnio otrzymanych wiadomości od każdego innego procesu zainicjalizowane jako 0. Priorytety ustalane są na podstawie znaczników czasowych. W przypadku, kiedy są one równe decyduje id procesu.
2. Jeśli po otrzymaniu wiadomości od innego procesu wartość znacznika czasowego
   wiadomości jest wyższa niż zegara procesu odbiorcy, zastępuje on wartość swojego zegara wartością przychodząca.
3. Aby otrzymać dostęp do hotelu lub przewodnika proces j-ty zwiększa wartość swojego zegara logicznego o 1 oraz wysyła REQ ze swoim znacznikiem czasowy, rodzajem procesu(błękitni, fioletowi, sprzątacze) oraz id hotelu do którego chce wejść lub informacją o tym, że żądanie dotyczy przewodnika do wszystkich innych procesów łącznie z samym sobą.
4. Proces j-ty otrzymuje dostęp do miejsca w hotelu jeżeli:
   (W1) Od wszystkich pozostałych procesów otrzymaliśmy wiadomości o starszej
   etykiecie czasowej.
   (W2) Dla:
   - Niebieskiego lub fioletowego, jeżeli:
     (W1) W kolejce nie znajduje się żadne żądanie z wyższym priorytetem ubiegające się o ten sam hotel, którego rodzaj to przeciwna fakcja lub sprzątacze.
     (W2) Żądanie znajduje się w M(liczba miejsc w hotelu) najstarszych żądań spośród ubiegających się o ten sam hotel.
   - Sprzątaczy, jeżeli w kolejce nie znajduje się żadne żądanie z wyższym priorytetem ubiegające się o ten sam hotel.
5. Po otrzymaniu dostępu do miejsca w hotelu proces j-ty (jeśli jest kosmitą) zwiększa o jeden wartość swojego zegara i wysyła REQ ubiegająć się o przewodnika.
6. Proces(kosmita) otrzymuje dostęp do przewodnika jeżeli:
   (W1) Od wszystkich pozostałych procesów otrzymaliśmy wiadomości o starszej
   etykiecie czasowej.
   (W2) Żądanie znajduje się w P(liczba przewodnikow) żądań z najwyższym priorytetem spośród żądań o przewodnika.
7. Proces j-ty wypisuje na standardowe wyjście informację o wyjściu z hotelu i wysyła RELEASE do wszystkich innych procesów
8. Proces i-ty po otrzymaniu REQ wstawia żądanie do lokalnej kolejki żądań (posortowanej po ich znacznikach czasowych/priorytetach) , zwiększa o jeden wartość swojego zegara, następnie wysyła ACK do procesu j-tego.
9. Proces i-ty po otrzymaniu RELEASE od procesu j-tego usuwa wszystkie żądania procesu j-tego z kolejki.
