#include "Commands.hpp"
#include "Includes.hpp"

bool DEBUG_MODE = false;

void loadData();

map<string, function<void()>> cmds = {
	{ "help", help },
	{"echo",echo},
	{"godot",godot},
	{"shortcut",shortcut},
	{"list",list},
	{"config",config},
	{"config-reload",loadData},
	{".",application},
};

void commandParse(string unparsed_command) {
	arguments = {};
	string command = "";
	string word = "";
	int word_count = 0;

	stringstream ss(unparsed_command);

	while (ss >> word) {
		word_count++;
		if (word_count == 1) {
			command = word;
			continue;
		}

		arguments.push_back(word);
	}

	if (cmds[command]) {
		try
		{
			cmds[command]();
		}
		catch (const std::exception& error)
		{
			cout << "\t" << error.what();
		}
		cout << endl;
		cout << endl;
	}
	else {
		printError("Unknown Command!");
	}

}

void loadData() {
	vector<string> getData = readFile(CONFIG_PATH);
	if (getData.size() <= 0) return;
	
	GODOT_PATH = "";
	GODOT_GAMES_PATHS = {};
	STEAM_GAMES_FOLDER_PATHS = {};
	APP_PATHS = {};

	vector<string> config_line_data = {};
	string config_line;

	for (string current_line : getData) {
		if (current_line[0] == '#') continue;
		config_line_data = {};
		config_line = "";

		stringstream ss(current_line);
		while (ss >> config_line) {
			config_line_data.push_back(config_line);
		}

		if (config_line_data.size() <= 1) continue;

		string first_key = config_line_data[0];

		//switch cases dont work on strings
		if (first_key == "GD") GODOT_PATH = config_line_data[1];
		if (first_key == "SHORTCUT") STEAM_GAMES_FOLDER_PATHS.push_back(config_line_data[1]);
		if (first_key == "GODG") {
			if (config_line_data.size() < 3) {
				printError("Missing Value for GODOT Game Project. " + config_line_data[1]);
				continue;
			}

			GODOT_GAMES_PATHS[config_line_data[1]] = config_line_data[2];
		}

		if (first_key == "APP") {
			if (config_line_data.size() < 3) {
				printError("Missing Application Path.");
				continue;
			}

			APP_PATHS[config_line_data[1]] = "";
			
			//cause stringstream divides up the path by spaces, 
			//so this is a dumb fix till i can be bothered to come up with something better.
			for (int idx = 0; idx < config_line_data.size(); idx++) {
				if (idx < 2) continue; //because 0 is the key and 1 is the name of the program
				APP_PATHS[config_line_data[1]] += config_line_data[idx] + " ";
			}
			
		}

	}


}

int main()
{
	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, my_documents);


	//pointing to documents as my fruitcup.txt is automatically copied over there
	//because for some reason quick launching this app doesnt load the config
	if (result != S_OK) {
		std::cerr << "Couldn't find your Documents Folder: " << result << "\n";
	}
	else {
		CONFIG_PATH = (char*)my_documents;
		CONFIG_PATH += "/fruitcup.txt";
	}

	loadData();
	
	while (true)
	{
		cout << "\x1b[0;0;0m";
		string answer;
		cout << "> ";
		getline(cin, answer);
		commandParse(answer);
	}
}
