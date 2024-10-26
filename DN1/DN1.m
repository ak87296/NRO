% 1. NALOGA

% Prikažem vse decimalke 
format long;

% Odprem datoteko, za branje
fid = fopen('naloga1_1.txt', 'r');

% Preberem prvo vrstico
ime = fgetl(fid);

% Preberem drugo vrstico
dimenzije = fscanf(fid, 'stevilo preostalih vrstic: %d; stevilo podatkov v vrstici: %d', [1 2]);
stVrstic = dimenzije(1);
stPodatkov = dimenzije(2);

% Preberem preostale podatke (100 vrstic po 1 podatek)
podatki = fscanf(fid, '%f', [stPodatkov, stVrstic]);
podatki = podatki'; % Matriko preberem kot vektor vrstic

% Zaprem datoteko
fclose(fid);

% Pretvorim matriko podatki v vektor t
t = podatki(:);


% 2. NALOGA

% Odprem datoteko
fid = fopen('naloga1_2.txt', 'r');

% Preberem prvo vrstico.
stVrednosti = fscanf(fid, 'stevilo_podatkov_P: %d', 1);

% Ustvarim prazen vektor P.
P = zeros(stVrednosti, 1);

% Preberem vsako vrednost posebej z uporabo for zanke.

for i = 1:stVrednosti
    P(i) = fscanf(fid, '%f', 1);  % Preberem naslednjo vrednost kot float
end

% Zaprem datoteko
fclose(fid);

% Graf P(t)
plot(t, P, 'b-', 'LineWidth', 2); % Izrišem graf (z modro črto)
xlabel('t [s]'); % Oznaka x-osi
ylabel('P [W]'); % Oznaka y-osi
title('Graf P(t)'); % Naslov grafa
grid on; % Prikaži mrežo


% 3. NALOGA

tmin = min(t);
tmax = max(t);

% Število intervalov
n = length(t) - 1;

% Izračunam širino intervala
dx = (tmax - tmin) / n;

integral_vrednost = 0; % začetna vrednost integrala

% Sestavim trapez in dodam v integral
for i = 1:n
    integral_vrednost = integral_vrednost + (dx / 2) * (P(i) + P(i + 1));
end

disp(['Ročno izračunan integral:',num2str(integral_vrednost, '%.10f')]);

trapz_integral = trapz(t, P);
disp(['Izračunan integral s funkcijo trapz: ', num2str(trapz_integral, '%.10f')]);

% Ugotovimo, da dobimo isti rezultat.