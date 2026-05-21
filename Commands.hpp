#pragma once

#include "Includes.hpp"

//i am too lazy to come up with some smart way to call a function with arguments
//so they're now global, but at the beginning of commandParse, it clears it before calling
vector<string> arguments = {};

string GODOT_PATH = "";
map<string, string> GODOT_GAMES_PATHS = {};
vector<string> STEAM_GAMES_FOLDER_PATHS = {};
map<string, string> APP_PATHS = {};

string YTDLP_PATH = "";

string CONFIG_PATH = "";


//UTILS
void printError(string toPrint) {
	cout << "\x1b[0;33;31m> " << toPrint << endl;
	cout << "\x1b[0;0;0m" << endl;
}

void print(string toPrint) {
	cout << toPrint;
}

void openDirInExplorer(const char* loc) {
	ShellExecuteA(NULL, "open", loc, NULL, NULL, SW_SHOW);
}

void beginApplication(const char* app_path, char* argument) {
	// additional information
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	// start the program up
	CreateProcessA(app_path,   // the path
		argument,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		DETACHED_PROCESS & CREATE_NEW_CONSOLE,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

vector<string> readFile(string filePath) {
	ifstream file;
	file.open(filePath);

	if (!file.is_open()) {
		printError("Couldn't open file!");
		return {};
	}

	string currentLine;
	vector<string> data = {};

	while (getline(file,currentLine))
	{
		data.push_back(currentLine);
	}

	file.close();

	return data;
}

void toLowerCaseRef(string &wantedString) {
	for (int i = 0; i < wantedString.size(); i++) {
		wantedString[i] = tolower(wantedString[i]);
	}
}

string toLowerCase(string wantedString) {
	for (int i = 0; i < wantedString.size(); i++) {
		wantedString[i] = tolower(wantedString[i]);
	}

	return wantedString;
}

string removeCharacter(string wantedString, char badChar) {
	for (int i = 0; i < wantedString.size(); i++) {
		if (wantedString[i] != badChar) continue;

		wantedString.erase(wantedString.begin() + i);
	}

	return wantedString;
}

//

void help() {
	print("Commands:\n");
	print("help - presents this screen\n");
	print("echo [args] - prints outs arguments\n");
	print("godot [game] - opens the godot engine (optional: can open projects specified in the config)\n");
	print("shortcut [directory] - opens explorer to the wanted dir (more for shortcuts/places you're more likely to go to.)\n");
	print("list [directory] - lists files in directory index (no argument will display directory indexs)\n");
	print("config - opens notepad to config file\n");
	print("config-reload - reloads config file\n");
	print(". [app] - opens app listed in config\n");
}

void application() {
	string wanted_app = arguments.front();
	string app;
	string args = "";

	app = APP_PATHS[wanted_app];

	if (app == "") {
		printError("Couldn't find that app! App Inputted: " + wanted_app);
		return;
	}

	if (arguments.size() > 1) {
		args = arguments[1];
	}

	
	beginApplication(app.c_str(), (char*)args.c_str());
}

void list() {
	DIR* dr;
	struct dirent* en;
	string current_dir = "nil";
	
	//basically if the args is just nothing
	if (arguments.size() <= 0) {
		printError("Please enter a Index. Here are the paths currently inputted:");
		print("-------------------------\n");
		print("# | PATH                |\n");
		print("-------------------------\n");
		for (int i = 0; i < STEAM_GAMES_FOLDER_PATHS.size(); i++) {
			print(to_string(i) + " | " + STEAM_GAMES_FOLDER_PATHS[i] + "\n");
		}

		return;
	}

	string arg = arguments.front();
	int conv_arg = atoi(arg.c_str());
	for (int i = 0; i < STEAM_GAMES_FOLDER_PATHS.size(); i++) {
		if (i == conv_arg) {
			current_dir = STEAM_GAMES_FOLDER_PATHS[i];
			break;
		}
	}

	if (current_dir == "nil") {
		printError("Couldn't find directory.");
		return;
	}
	
	print("List files in " + current_dir + "\n");

	dr = opendir(current_dir.c_str());

	if (dr) {
		while ((en = readdir(dr)) != NULL) { //this probably doesn't scale well with bigger directories
			string game_name = en->d_name;
			bool cont = true;

			for (char i : game_name) {
				if (!cont) continue;

				if (i == '.') {
					cont = false;
				}
			}

			if (cont) {
				game_name = "\t" + game_name;
			}

			print(game_name + "\n");
		}
		closedir(dr);
	}
	else {
		printError("Directory doesn't exist? Path: " + current_dir + "\n");
	}


}

void shortcut() {
	if (arguments.size() <= 0) {
		printError("No Folder Inputted.");
		return;
	}

	string wanted_game = arguments.front();
	string game_location;
	bool found_game = false;
	
	//now it's case insensitive & doesnt require spaces
	//ex: GTA IV, gta iv and GTAIV are all valid, as long as the directory we actually want actually exists
	string conv_wg = toLowerCase(wanted_game);
	conv_wg = removeCharacter(conv_wg, ' ');

	DIR* dr;
	struct dirent* en;

	for (int i = 0; i < STEAM_GAMES_FOLDER_PATHS.size(); i++) {
		string current_dir = STEAM_GAMES_FOLDER_PATHS[i];
		print("Searching in " + current_dir + "\n");

		dr = opendir(current_dir.c_str());

		if (dr) {
			while ((en = readdir(dr)) != NULL) { //this probably doesn't scale well with bigger directories
				string game_name = en->d_name;
				string conv_gn = toLowerCase(game_name);
				conv_gn = removeCharacter(conv_gn, ' ');

				//this also wouldn't do well if there are duplicates
				//TODO: add duplicate handling
				if (conv_gn != conv_wg) continue;

				game_location = current_dir + "\\" + game_name;
				found_game = true;
				break;
			}
			closedir(dr);
		}
		else {
			printError("Directory doesn't exist? Path: " + current_dir + "\n");
		}
	}

	if (!found_game) {
		printError("Couldn't find that anywhere. Directory: " + wanted_game + "\n");
		return;
	}

	print("Found " + wanted_game + " at " + game_location + "\n");
	
	//apparently explorer is just a special prince that needs to be handled in a different way
	//damn you bill gates
	openDirInExplorer(game_location.c_str());
}

void config() {
	openDirInExplorer(CONFIG_PATH.c_str());
}

void ytdlp() {
	if (YTDLP_PATH == "") {
		printError("YTDLP Executable wasn't provided!");
		return;
	}

	if (arguments.size() < 2) {
		printError("You're missing a few arguments.");
		return;
	}

	string cmd = "C:/WINDOWS/system32/cmd.exe";
	string dir = arguments[0];
	string playlist_url = arguments[1];
	string args = "/c start "  + YTDLP_PATH + " -f 'bestaudio' --download-archive archive.txt -x --audio-format mp3 --embed-metadata -t sleep ";
	args += "-o " + dir;

	beginApplication((const char*)cmd.c_str(), (char*)args.c_str());

}

void godot() {
	if (GODOT_PATH == "") {
		printError("Godot Executable wasn't provided!");
		return;
	}

	if (arguments.size() > 0){
		if (GODOT_GAMES_PATHS.find(arguments.front()) != GODOT_GAMES_PATHS.end()) {
			string the_path = GODOT_GAMES_PATHS[arguments.front()];
			char last_char = the_path[the_path.length()];
			if (last_char == ' ') {
				the_path = the_path.substr(0, the_path.length() - 1);
			}
			if (last_char != '/') {
				the_path += "/";
			}
			arguments[0] = "-e " + the_path + "project.godot ";
		}
		else {
			printError("Couldn't find Project! Path: " + arguments.front());
			arguments[0] = "";
		}
	}


	//incase i need to do some really hard debugging on godot and my game
	if (arguments.size() > 1 && arguments[1] == "-em") {
		beginApplication(GODOT_PATH.c_str(), (char*)arguments[0].c_str());
	}
	else {
		//yes we have to specify cmd, i guess CreateProcessA just isn't global or has access to windows apps normally
		//also we're doing this because godot is aggressive as fuck with grabbing the cli as an output and nothing
		//else worked. another dumbass decision by godot, love them, but bro.
		string cmd = "C:/WINDOWS/system32/cmd.exe";
		string final_arg = "/c start " + GODOT_PATH + " " + (arguments.size() > 0 ? arguments[0] : "");

		beginApplication(cmd.c_str(), (char*)final_arg.c_str());
	}
	

}

void echo() {
	for (int i = 0; i < arguments.size(); i++) {
		cout << arguments[i];
		if (i < arguments.size()) cout << " ";
	}

}
