#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

enum Category {
	strategy, role_playing, action, puzzle
};


const int SIZE = 100;
class Game {
	static int next_id;
	int id;
	char name[SIZE];
	Category category;
	int popularity;
public: 
	Game() = default;
	Game(const char* name, const Category category, const int& popularity, const int& id = next_id++) :id{ id }, category { category }, popularity{ popularity } {
		strcpy_s(this->name, name);
	}
	~Game() {};

	void print() const {
		const string cat[4] = { "strategy", "role_playing", "action", "puzzle" };
		cout << "ID: " << id << "\tGame name: " << name << "\tCategory: " << cat[category] << "\tPopularity: " << popularity << endl;
	}
	char* getName() {
		return name;
	}
	Category getCategory() {
		return category;
	}
	int getId() {
		return id;
	}
	void changeID(int id) {
		this->id = id;
	}
};

int Game::next_id = 1;

class Database {
	const string file_name;
	fstream file;
	size_t size = 0;
	size_t object_size = sizeof(Game);
public:
	Database(const string& file_name) : file_name{ file_name } {
		ofstream start(file_name);
		start.close();
		file.open(file_name, ios_base::out | ios_base::in | ios_base::binary);
		if (!file.is_open()) {
			cout << "Error opening file!" << endl;
			return;
		}
	}
	~Database(){
		if (file.is_open()) {
			file.close();
		}
	}
	void add(const Game& game) {
		if (!file.is_open()) {
			file.open(file_name);
		}
		file.seekp(0, ios_base::end);
		file.write((char*)&game, object_size);
		++size;
	}
	void print() {
		if (!file.is_open()) {
			file.open(file_name);
		}
		file.seekg(0);
		Game res;
		while (file.read(reinterpret_cast<char*>(&res), object_size))
		{
			res.print();
		}
		cout << endl;
	}
	void searchByName(const char* name) {
		if (!file.is_open()) {
			file.open(file_name);
		}
		file.clear();
		file.seekg(0);
		Game res;
		while (file.read(reinterpret_cast<char*>(&res), object_size))
		{
			if (strcmp(res.getName(), name) == 0)
			{
				res.print();
			}
		}
		cout << endl;
	}
	void searchByCategory(const Category& category) {
		if (!file.is_open()) {
			file.open(file_name);
		}

		file.clear();
		file.seekg(0);
		Game res;
		while (file.read(reinterpret_cast<char*>(&res), object_size)) {
			if (res.getCategory() == category) {
				res.print();
			}
		}
	}

	void editGamebyID(int id, const Game& updatedGame) {
		if (!file.is_open()) {
			file.open(file_name);
		}
		file.clear();
		int pos = (id - 1) * object_size;
		file.seekg(pos);
		Game currentGame;
		file.read(reinterpret_cast<char*>(&currentGame), object_size);

		cout << "Current game:\t";
		currentGame.print();
		if (file.fail()) {
			cerr << "Failed to read game data." << endl;
			return;
		}

		file.seekp(pos);
		file.write(reinterpret_cast<const char*>(&updatedGame), object_size);
		
		if (file.fail()) {
			cerr << "Failed to write updated game data." << endl;
			return;
		}
		cout << "Updated"<<endl;
	}
	void delbyID(int id) {
		file.clear();
		file.seekg(0);
		string tmpfile = "tmp.dat";
		ofstream temp(tmpfile);
		Game res;
		bool found = false;
		while (file.read(reinterpret_cast<char*>(&res), object_size)) {
			if (res.getId() == id) {
				found = true;
			}
			if (res.getId() != id) {
				if (res.getId() > id) {
					res.changeID(res.getId() - 1);
				}
				temp.write((char*)&res, object_size);
			}
		}
		file.close();
		temp.close();
		if (found) {
			remove(file_name.c_str());
			rename(tmpfile.c_str(), file_name.c_str());
		}
		else {
			remove(tmpfile.c_str());
			cout << "Game with id: " << id << " was not found"<< endl;
		}
	
	}
	void clearDatabase() {
		file.close();
		if (remove(file_name.c_str()) != 0) {
			cerr << "Error during an attempt to clear the database" << endl;
		}
		else {
			cout << "Database cleared" << endl;
		}
	}
};



int main()
{
	vector<Game> games;

	games.emplace_back("The Legend of Heroes: Trails of Cold Steel", Category::role_playing, 80);
	games.emplace_back("Sid Meier's Civilization VI", Category::strategy, 90);
	games.emplace_back("The Witcher 3: Wild Hunt", Category::role_playing, 85);
	games.emplace_back("Portal 2", Category::puzzle, 75);
	games.emplace_back("Red Dead Redemption 2", Category::action, 88);

	Database gamedata("database.dat");
	for (auto& game : games) {
		gamedata.add(game);
	}
	gamedata.print();
	gamedata.searchByName("Portal 2");
	gamedata.searchByCategory(Category::role_playing);
	Game newgame("Updated Game Name", Category::action, 95, 1);
	gamedata.editGamebyID(1, newgame);
	gamedata.print();
	gamedata.delbyID(2);
	gamedata.add({ "The Legend of Heroes: Trails of Cold Steel", Category::role_playing, 80, 5 });
	gamedata.print();
	gamedata.clearDatabase();
	gamedata.print();

}
