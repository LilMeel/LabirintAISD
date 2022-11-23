#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <list>


#pragma region usings
namespace fs = std::filesystem;
using std::fstream;
using std::vector;
using std::string;
using std::list; // List тоже самое, что связанный список
using std::cout;
using std::endl;
#pragma endregion

class Graph {
	size_t num_of_vertices;
	int start_vert, end_vert;
	list<int>* adjLists;
	int* start_pos;
	int* end_pos;
	bool* used;
	vector<int> forks;
	vector<int> best_way;
	vector<int> current_way;

public:
	Graph(int _vertices, int* _start_pos, int* _end_pos) {
		num_of_vertices = _vertices;
		start_pos = _start_pos;
		end_pos = _end_pos;
		adjLists = new list<int>[num_of_vertices];
	}

	~Graph() {
		delete[] adjLists;
		cout << "Graph was deleted" << endl;
	}

	void print_graph() {
		for (int v = 0; v < num_of_vertices; v++) {
			auto temp = adjLists[v];
			cout << "\n Adjacency list of vertex " << endl;
			int i = 0;
			for (auto iter = temp.begin(); iter != temp.end(); iter++)
			{
				std::cout << *iter << "\t";
			}
		}
	}

	int add_edge(int src, int dest, int i, int j) {
		if (i == start_pos[0] && j == start_pos[1]) {
			start_vert = src;
		}
		if (i == end_pos[0] && j == end_pos[1]) {
			end_vert = src;
		}

		adjLists[src].push_front(dest);
		return 0;
	}


	void bypass_with_return(int v, int from) {
		used[v] = true;
		current_way.push_back(from);

		if (v == end_vert) {
			if (best_way.size() > current_way.size() || best_way.size() == 0) {
				best_way = current_way;
			}
			return;
		}

		if (adjLists[v].size() > 2) {
			forks.push_back(v);
		}

		for (int u : adjLists[v]) {
			if (!used[u]) {
				bypass_with_return(u, v);
				if (forks.size() > 0 && v == forks.back()) {
					vector<int>::iterator index = std::find(current_way.begin(), current_way.end(), from);
					current_way.erase(index, current_way.end());
					forks.pop_back();
				}

			}
		}
	}

	void print_way() {
		for (int val : best_way) {
			cout << val << ' ' << endl;
		}
	}

	size_t get_size() {
		return num_of_vertices;
	}

	void create_visitor_array() {
		used = new bool[num_of_vertices];
		memset(used, false, num_of_vertices);
	}
};

class Matrix {
	std::ifstream fs;
	size_t width, height;
	vector<std::vector<int>> matrix_of_labirint;
	int start_pos[2], end_pos[2];
	int num_of_verticles = 0;
	bool** visited;
	Graph* graph;

	void read_matrix() {
		string line;
		height = 0;
		while (getline(fs, line)) {
			if (!height)
				width = line.length() / 2 + line.length() % 2;
			int* iarr = new int[width];
			int j = 0;
			for (char symb : line) {
				if (symb >= 48 && symb <= 57) {
					iarr[j] = symb - 48;
					if (iarr[j] == 5) {
						start_pos[0] = height;
						start_pos[1] = j;
					}
					else if (iarr[j] == 9) {
						end_pos[0] = height;
						end_pos[1] = j;
					}
					else if (iarr[j] == 2) {
						num_of_verticles++;
					}

					j++;
				}
			}
			matrix_of_labirint.push_back(vector<int>(iarr, iarr + width));
			//delete[] iarr;
			height++;
		}
		fs.close();

		visited = new bool* [height];
		for (int i = 0; i < height; i++) {
			visited[i] = new bool[width];
			memset(visited[i], '\0', width);
		}
		graph = new Graph(num_of_verticles + 2, start_pos, end_pos); // +2, тк ещё добавялем ещё начало и конец

		generate_graph(start_pos[0], start_pos[1], start_pos[0], start_pos[1]);

		for (int i = 0; i < height; i++) {
			delete[] visited[i];
		}
		delete[] visited;
	}

	void generate_graph(int i, int j, int pred_i, int pred_j) {
		static int last_verticle = 0;

		int curr_verticle = last_verticle;
		if (visited[i][j]) {
			return;
		}
		visited[i][j] = true;

		for (int iter = -1; iter < 2; iter += 2) {
			for (int jiter = 0; jiter < 2; jiter++) {
				int temp_dx = 0;
				if (jiter) {
					temp_dx = ~iter | 1;
				}
				if (matrix_of_labirint[i + iter + temp_dx][j + temp_dx] != 4 && !visited[i + iter + temp_dx][j + temp_dx]) {
					graph->add_edge(curr_verticle, last_verticle + 1, i + iter + temp_dx , j+temp_dx);
					graph->add_edge(last_verticle+1, curr_verticle, i + iter + temp_dx, j + temp_dx);
					last_verticle++;
					generate_graph(i + iter + temp_dx, j + temp_dx, i, j);
				}
			}
		}
	}

public:
	Matrix(string matrix_path) {
		fs.open(matrix_path, fstream::in);
		if (fs.fail()) {
			cout << "Ошибка при открытие файла :(" << endl;
			return;
		}
		read_matrix();
		fs.close();
	}

	~Matrix() {
		matrix_of_labirint.clear();
		for (int i = 0; i < height; i++) {
			delete[] visited[i];
		}
		delete[] visited;
	}

	void print_matrix() {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				cout << matrix_of_labirint[i][j] << ' ';
			}
			cout << endl;
		}
	}


	Matrix() {

	}
#pragma region Getters
	int get_width() {
		return width;
	}

	int get_height() {
		return height;
	}

	int* get_start_pos() {
		return start_pos;
	}

	int* get_end_pos() {
		return end_pos;
	}

	vector<vector<int>> get_matrix() {
		return matrix_of_labirint;
	}

	Graph* get_graph() {
		return graph;
	}
#pragma endregion

};

class Labirint{
	Matrix* main_matrix;
	Graph* graph;
	int* start_pos;
	int* end_pos;

public:
	Labirint(string lab_path) {
		main_matrix = new Matrix(lab_path);
		graph = main_matrix->get_graph();
	}

	void dfs() {
		//bool* used = new bool[graph->get_size()];
		//memset(used, '\0', graph->get_size());
		graph->print_graph();
		graph->create_visitor_array();

		graph->bypass_with_return(1,0);
		graph->print_way();
	}

};

int main()
{
	setlocale(LC_ALL, "Russian");
	string filename;

	std::cout << "Введите название файла для открытия лабиринта: " << std::endl;

	std::cin >> filename;

	Labirint* main_labirint = new Labirint(fs::current_path().string() + "\\" + filename);
	main_labirint->dfs();
}

