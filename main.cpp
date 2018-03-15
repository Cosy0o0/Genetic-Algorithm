#include <iostream>
#include <queue>
#include <fstream>
#include <vector>
#include <ctime>

int N = 0; // liczba miast
int P = 20; //rozmiar populacji
using namespace std;

struct usuwane
{
	int x;
	int y;
};

struct matrix // struktura zawierająca macierz kosztów w postaci vectora oraz rozmiar macierzy
{
	vector <vector<int>> tab;
	int size = N;

};

class Stopwatch { // klasa umożliwiająca liczenie czasu algorytmu
private:
	clock_t tstart;
	clock_t tstop;
public:
	double time;
	Stopwatch::Stopwatch() {
		//ctor
	}
	void Stopwatch::start() {
		tstart = clock();
		time = 0;
	}
	void Stopwatch::stop() {
		tstop = clock();
		time = ((double)(tstop - tstart)) / CLOCKS_PER_SEC;
	}
	double Stopwatch::read() {
		return time;
	}
};

matrix generowanie(matrix start) // funkcja generująca losową macierz
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			int bufor;
			bufor = (rand() % 50) + 1;
			if (i == j) start.tab[i].push_back(INT_MAX);
			else start.tab[i].push_back(bufor);
		}
	}
	return start;
}

matrix wczytaj(matrix start) // funkcja wczytująca macierz z pliku symetrycznego
{
	ifstream plik;
	plik.open("gr17.tsp");
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			start.tab[i].push_back(INT_MAX);
		}
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			int bufor;
			plik >> bufor;
			start.tab[i][j] = bufor;
			start.tab[j][i] = start.tab[i][j];
			if (i == j)
			{
				start.tab[i][j] = INT_MAX;
			}
		}
	}
	plik.close();
	return start;
}

matrix wczytaj2(matrix start) // funkcja wczytująca macierz z pliku asymetrycznego
{
	ifstream plik;
	plik.open("60tabu.txt");
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			int bufor;
			plik >> bufor;
			if (i == j) start.tab[i].push_back(INT_MAX);
			else start.tab[i].push_back(bufor);
		}
	}
	plik.close();
	return start;
}

int wyswietl(matrix start) // funkcja wyświetlająca macierz
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (start.tab[i][j] == INT_MAX) cout << "NN ";
			else cout << start.tab[i][j] << " ";
		}
		cout << endl;
	}
	return 0;
}

int tourCost(int * tour, matrix actual) // funkcja licząca koszt podróży
{
	int bestTourCost = 0;
	for (int i = 0; i < N - 1; i++)
	{
		bestTourCost += actual.tab[tour[i]][tour[i + 1]];
	}
	bestTourCost += actual.tab[tour[N - 1]][tour[0]];
	return bestTourCost;
}

int * beginRandom(int * startingTour) //funkcja podająca losową trasę przez wszystkie miasta
{
	int * startingRand = new int[N];
	for (int i = 0; i < N; i++)
	{
		startingRand[i] = -1;
	}
	int counter = 0, los = 0;
	while (counter < N)
	{
		do
		{
			los = rand() % N;
		} while (startingRand[los] != -1);
		startingRand[los] = -2;
		startingTour[counter] = los;
		counter++;
	}
	delete[] startingRand;
	return startingTour;
}

void showTab(int * tab,int size)
{
	for (int i = 0; i < size; i++)
	{
		cout << tab[i] << " ";
	}
	cout << endl;
}

void genetic(matrix actual)
{
	Stopwatch clock;
	clock.start(); // rozpoczęcie liczenia czasu
	int ** population = new int *[P]; //stworzenie populacji początkowej
	for (int i = 0; i < P; i++)
	{
		population[i] = new int[N];
		population[i] = beginRandom(population[i]);
	}

	for (int k = 0; k < 1000; k++)
	{
		int ** newPopulation = new int *[P]; // stworzenie następnej populacji
		int * cost = new int[P]; // tablica kosztów doróg
		int * parent = new int[P]; // tablica rodziców
		int bestCost = 0;
		for (int i = 0; i < P; i++) // znalezienie drogi o najmniejszym koszcie
		{
			cost[i] = tourCost(population[i], actual);
			if (cost[i] < cost[bestCost])
			{
				bestCost = i;
			}
			newPopulation[i] = new int[N];
		}
		newPopulation[0] = population[bestCost]; // przypisanie najlepszej drogi do nowej populacji
		if (k == 999) // instrukcja kończąca algorytm
		{
			cout << "Najlepsza droga: " << cost[bestCost] << endl;
			clock.stop(); // zatrzymanie liczenia czasu
			cout << "Czas[s]: " << clock.read() << endl;
		}

		int parentCounter = 0;
		for (int i = 0; i < P; i++) // walka
		{
			int a = rand() % P; // wybranie losowych dróg
			int b = a;
			while (a == b)
			{
				b = rand() % P;
			}
			bool ap = 0; // sprawdzenie czy już są rodzicami
			bool bp = 0;
			for (int j = 0; j < parentCounter; j++)
			{
				if (a == parent[j]) ap = 1;
				if (b == parent[j]) bp = 1;
			}
			if (cost[a] <= cost[b] && !ap) // przypisanie nowego rodzica
			{
				parent[parentCounter] = a;
				parentCounter++;
			}
			else if (cost[a] > cost[b] && !bp)
			{
				parent[parentCounter] = b;
				parentCounter++;
			}
		}
		for (int i = 0; i < P - 1; i++) //rozmnażanie
		{
			int mother = parent[rand() % parentCounter]; // wybranie losowych rodziców
			int father = mother;
			while (mother == father)
			{
				father = parent[rand() % parentCounter];
			}
			int wallOne = rand() % (N - 1); // wybranie losowego przedziału, czyli części
			int wallTwo = wallOne;			// matki w dziecku
			while (wallOne == wallTwo)
			{
				wallTwo = rand() % (N - 1);
			}
			int firstWall, secondWall;
			if (wallOne < wallTwo)
			{
				firstWall = wallOne+1;
				secondWall = wallTwo+1;
			}
			else
			{
				firstWall = wallTwo+1;
				secondWall = wallOne+1;
			}

			for (int j = firstWall; j != secondWall;j++) //przypisanie części matki do dziecka
			{
				newPopulation[i+1][j] = population[mother][j];
			}

			int childPosition = secondWall;
			for (int j = secondWall; firstWall != childPosition;) // przypisanie części ojca do dziecka
			{
				bool isUsed = 0;
				for (int m = 0; m < N; m++)
				{
					if (newPopulation[i + 1][m] == population[father][j]) isUsed = 1;
				}
				if (!isUsed)
				{
					newPopulation[i + 1][childPosition] = population[father][j];
					childPosition++;
				}
				j++;
				childPosition = childPosition % N;
				j = j % N;
			}
			if (rand() % 1000 == 5) //mutacja
			{
				int first = rand() % N;
				int second = first;
				while (first == second)
				{
					second = rand() % N;
				}
				int buf = newPopulation[i][first];
				newPopulation[i][first] = newPopulation[i][second];
				newPopulation[i][second] = buf;
			}
		}

		population = newPopulation; // nowa populacja dzieci staje się aktualną

		delete[] parent;
		delete[] cost;
	}
	for (int i = 0; i < P; i++)
	{
		delete[] population[i];
	}
	delete[] population;
}

int main()
{
	srand(time(NULL));
	matrix start;

	for (char option = '0'; option != '5';)
	{
		cout << "Program obliczania problemu komiwojazera przy użyciu algorytmu genetycznego" << endl;
		cout << "1: Wygeneruj losowa tablice" << endl;
		cout << "2: Wczytaj z pliku" << endl;
		cout << "3: Wyswietl" << endl;
		cout << "4: Uruchom algorytm" << endl;
		cout << "5: Opusc program" << endl;
		cin >> option;

		switch (option)
		{
		case '1':
			cout << "Podaj liczbe wierzcholkow: ";
			cin >> N;
			start = generowanie(start);
			break;
		case '2':
			cout << "Podaj liczbe wierzcholkow: ";
			cin >> N;
			start = wczytaj(start);
			break;
		case '3':
			wyswietl(start);
			break;
		case '4':
			genetic(start);
			break;
		case '5':
			break;
		default:
			cout << "Nie ma takiej opcji!" << endl;
			break;
		}
	}

	return 0;
}
