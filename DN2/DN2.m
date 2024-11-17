clc
clear

% METODA “scatteredInterpolant”

tic
% Odprem datoteko 'vozlisca temperature dn2.txt' za branje
fileID = fopen('vozlisca_temperature_dn2.txt', 'r');

% Preskočim prve štiri vrstice z opisom in informacijami o mreži
for i = 1:4
    fgetl(fileID);
end

% Uporabim textscan za branje podatkov (x, y, temperatura), z vejico kot ločilom
data = textscan(fileID, '%f %f %f', 'Delimiter', ',', 'CollectOutput', true);
fclose(fileID);

% Od tu naprej bi lahko podatke bral na dva načina. Lahko bi definiral
% prazne matrike in vsako vrstico dodal v matriko. Driga možnost je, da
% celice pretvorim v matriko in preberem matriko. Preizkusil sem oba
% načina in ugotovil, da je drugi precej hitrejši, zato sem obdržal drugo
% možnost.

% Pretvorim podatke v matriko za lažji dostop
data = data{1}; 

% Razdelim podatkov na x, y in temperaturne vrednosti T
x = data(:, 1);  % Prvi stolpec: x koordinate
y = data(:, 2);  % Drugi stolpec: y koordinate
T = data(:, 3);  % Tretji stolpec: temperature

% Kreiram interpolacijsko funkcijo
F = scatteredInterpolant(x, y, T, 'linear', 'none');

% Definiram točke za izračun temperature
x_tocka1 = 0.403;
y_tocka1 = 0.503;

% Izračunam temperaturo v podani točki
T_tocka1 = F(x_tocka1, y_tocka1);

% Prikažem rezultat
fprintf('Temperatura v točki (%.3f, %.3f) je približno %.3f.\n', x_tocka1, y_tocka1, T_tocka1);
toc


% METODA “griddedInterpolant”

tic
% Rekonstrukcija mreže
x_mreza = unique(x);  % Mreža v x-smeri
y_mreza = unique(y);  % Mreža v y-smeri

% Pretvorba v matriko temperature
T_mreza = reshape(T, numel(x_mreza), numel(y_mreza)); 

% Kreiranje interpolantne funkcije
F = griddedInterpolant({x_mreza, y_mreza}, T_mreza, 'linear', 'none');

% Definiram točke za izračun temperature
x_tocka2 = 0.403;
y_tocka2 = 0.503;

% Izračunam temperaturo
T_tocka2 = F(x_tocka2, y_tocka2);

% Prikaz rezultata
fprintf('Temperatura v točki (%.3f, %.3f) je približno %.3f.\n', x_tocka2, y_tocka2, T_tocka2);
toc



% LASTNA INTERPOLACIJSKA METODA (Metoda najbližjega soseda)

tic
% Koordinate točke, kjer želimo izračunati temperaturo
x_tocka3 = 0.403;
y_tocka3 = 0.503;

% Izračun razdalje med točko in vsemi vozlišči
razdalja = sqrt((x - x_tocka3).^2 + (y - y_tocka3).^2);

% Poiščemo najbližje vozlišče
[~, idx_min] = min(razdalja);  % min vrača najmanjšo vrednost in njen indeks

% Temperatura najbližjega vozlišča
T_najblizja = T(idx_min);

% Prikaz rezultata
fprintf('Temperatura v točki (%.3f, %.3f) z metodo najbližjega soseda je %.3f.\n', x_tocka3, y_tocka3, T_najblizja);
toc

% Glede na uporabljene metode in izpisane čase ugotovim, da je metoda
% najbližjih sosesov najhitrejša, a tudi najmanj natančna. Težava se pojavi
% predvsem, če bi imeli velike temperaturne gradiente, kar bi še povečalo
% napako.
% Če pa primerjam prvi dve metodi, pa ugotovim, da z metodo
% “griddedInterpolant” pridem do istega rezultata kot s prvo metodo, le da
% tokrat kar precej hitreje.


% Vozlišče z največjo temperaturo

[max_temp, idx_max] = max(T);

% Koordinate vozlišča z največjo temperaturo
x_max = x(idx_max);
y_max = y(idx_max);

% Izpis rezultata
fprintf('Največja temperatura je %.3f °C in se pojavi pri koordinatah (x = %.3f, y = %.3f).\n', max_temp, x_max, y_max);