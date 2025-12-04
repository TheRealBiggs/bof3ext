module;

export module bof3ext.textManager;

import bof3ext.helpers;
import bof3ext.singleton;
import bof3.dat;

import std;


void ReadTextFileIntoVector(const char* filename, std::vector<std::string>& vector) {
	if (!std::filesystem::exists(filename))
		return;

	std::ifstream file(filename);

	std::string line;

	while (std::getline(file, line))
		vector.push_back(line);
}


void TranslateEncodedText(std::string& text) {
	size_t off = 0;

	while ((off = text.find("{{", off)) != std::string::npos) {
		auto len = text.find("}}", off);
		len -= off;
		len -= 2;	// strlen("{{")

		auto tag = std::string_view(text.c_str() + off + 2, len);
		std::string_view arg;

		if (tag.length() > 5) {
			arg = tag.substr(6, tag.length() - 6);
			tag = tag.substr(0, 5);
		}

		if (tag == "NLINE")
			text = text.replace(off, len + 2 + 2, 1, '\x01');
		else if (tag == "NPAGE")
			text = text.replace(off, len + 2 + 2, 1, '\x02');
		else if (tag == "CHRTR")
			text = text.replace(off, len + 2 + 2, 1, '\x03');
		else if (tag == "COLRS") {
			char code[2]{ 0 };
			code[0] = '\x05';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[1]);
			text = text.replace(off, len + 2 + 2, code, 2);
		} else if (tag == "COLRE")
			text = text.replace(off, len + 2 + 2, 1, '\x06');
		else if (tag == "PRMTR") {
			char code[2]{ 0 };
			code[0] = '\x07';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[1]);
			text = text.replace(off, len + 2 + 2, code, 2);
		} else if (tag == "SOUND") {
			char code[2]{ 0 };
			code[0] = '\x0A';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[1], 16);
			text = text.replace(off, len + 2 + 2, code, 2);
		} else if (tag == "PAUSE")
			text = text.replace(off, len + 2 + 2, 1, '\x0B');
		else if (tag == "TXTBX") {
			char code[2]{ 0 };
			code[0] = '\x0C';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[1], 16);
			text = text.replace(off, len + 2 + 2, code, 2);
		} else if (tag == "EFCTS")
			text = text.replace(off, len + 2 + 2, 1, '\x0D');
		else if (tag == "EFCTE") {
			char code[3]{ 0 };
			code[0] = '\x0E';
			code[1] = '\x0F';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[2], 16);
			text = text.replace(off, len + 2 + 2, code, 3);
		} else if (tag == "UNK0F")
			text = text.replace(off, len + 2 + 2, 1, '\x0F');
		else if (tag == "INSTS")
			text = text.replace(off, len + 2 + 2, 1, '\x10');
		else if (tag == "INSTE")
			text = text.replace(off, len + 2 + 2, 1, '\x11');
		else if (tag == "CHOIC") {
			auto commaPos = arg.find(',');
			char id;
			std::from_chars(arg.data(), arg.data() + commaPos, id);

			auto startPos = commaPos + 1;
			commaPos = arg.find(',', startPos);
			char type;
			std::from_chars(arg.data() + startPos, arg.data() + commaPos, type);

			auto choicesLength = arg.length() - (commaPos + 1);

			auto code = new char[4 + choicesLength + 1] { 0 };
			code[0] = '\x14';
			code[1] = id;
			code[2] = '\x0C';
			code[3] = (char)((type << 4) | (std::count(arg.begin().operator+=(commaPos + 1), arg.end(), ',') + 1));

			std::memcpy(&code[4], arg.data() + commaPos + 1, choicesLength);

			for (auto i = 4U; i < 4 + choicesLength; ++i) {
				if (code[i] == ',')
					code[i] = '\x00';
			}

			text = text.replace(off, len + 2 + 2, code, 4 + choicesLength);

			delete[] code;
		} else if (tag == "DURTN") {
			char code[2]{ 0 };
			code[0] = '\x16';
			std::from_chars(arg.data(), arg.data() + arg.size(), code[1]);
			text = text.replace(off, len + 2 + 2, code, 2);
		} else if (tag == "UNK5F")
			text = text.replace(off, len + 2 + 2, 1, '\x5F');
		else if (tag.starts_with("SYM")) {
			char code[1]{ 0 };
			std::from_chars(tag.data() + 3, tag.data() + tag.size(), code[0], 16);
			text = text.replace(off, len + 2 + 2, code, 1);
		}
	}
}


export class TextManager : public Singleton<TextManager> {
public:
	void Initialise() {
		ReadTextFileIntoVector("NewData\\Text\\text_itemnames.txt", itemNames);
		ReadTextFileIntoVector("NewData\\Text\\text_weaponnames.txt", weaponNames);
		ReadTextFileIntoVector("NewData\\Text\\text_armornames.txt", armorNames);
		ReadTextFileIntoVector("NewData\\Text\\text_accessorynames.txt", accessoryNames);
		ReadTextFileIntoVector("NewData\\Text\\text_skillnames.txt", skillNames);
		ReadTextFileIntoVector("NewData\\Text\\text_menutabs.txt", menuTabTexts);
		ReadTextFileIntoVector("NewData\\Text\\text_categories.txt", categoryTexts);

		if (std::filesystem::exists("NewData\\Text\\text_enemynames_map.txt")) {
			std::ifstream file("NewData\\Text\\text_enemynames_map.txt");

			std::string line;

			while (std::getline(file, line)) {
				auto idEnd = line.find('=');
				auto oldName = line.substr(0, idEnd);
				auto newName = line.substr(idEnd + 1);

				enemyNameMap[oldName] = newName;
			}
		}
	}


	bool TextFileIsLoaded(unsigned int fileIndex, unsigned int fileNum) const {
		auto key = (fileIndex << 16) | fileNum;

		return text.count(key) > 0;
	}

	void LoadTextFile(unsigned int fileIndex, unsigned int fileNum) {
		auto datName = g_DatFileNames[fileIndex];
		std::filesystem::path datPath(datName);
		auto stem = datPath.stem().generic_string();

		auto filename = std::format("NewData\\Text\\text_general_{}_{}.txt", stem.c_str(), fileNum);

		if (!std::filesystem::exists(filename))
			return;

		std::ifstream file(filename);

		std::vector<std::string> lines;
		std::string line;

		while (std::getline(file, line)) {
			TranslateEncodedText(line);

			lines.push_back(line);
		}

		auto key = (fileIndex << 16) | fileNum;
		text[key] = std::move(lines);

		file.close();
	}


	bool HasText(unsigned int fileIndex, unsigned int fileNum, unsigned int index) const {
		auto key = (fileIndex << 16) | fileNum;

		return TextFileIsLoaded(fileIndex, fileNum) && index < text.at(key).size() && !text.at(key)[index].empty();
	}

	const std::string& GetText(unsigned int fileIndex, unsigned int fileNum, unsigned int index) const {
		auto key = (fileIndex << 16) | fileNum;

		return text.at(key)[index];
	}


	bool HasItemName(unsigned int index) const {
		return !itemNames.empty() && itemNames.size() >= index;
	}

	const std::string& GetItemName(unsigned int index) const {
		return itemNames[index];
	}


	bool HasWeaponName(unsigned int index) const {
		return !weaponNames.empty() && weaponNames.size() >= index;
	}

	const std::string& GetWeaponName(unsigned int index) const {
		return weaponNames[index];
	}


	bool HasArmorName(unsigned int index) const {
		return !armorNames.empty() && armorNames.size() >= index;
	}

	const std::string& GetArmorName(unsigned int index) const {
		return armorNames[index];
	}


	bool HasAccessoryName(unsigned int index) const {
		return !accessoryNames.empty() && accessoryNames.size() >= index;
	}

	const std::string& GetAccessoryName(unsigned int index) const {
		return accessoryNames[index];
	}


	bool HasSkillName(unsigned int index) const {
		return !skillNames.empty() && skillNames.size() >= index;
	}

	const std::string& GetSkillName(unsigned int index) const {
		return skillNames[index];
	}


	bool HasMenuTabText(unsigned int index) const {
		return !menuTabTexts.empty() && menuTabTexts.size() >= index;
	}

	const std::string& GetMenuTabText(unsigned int index) const {
		return menuTabTexts[index];
	}


	bool HasCategoryText(unsigned int index) const {
		return !categoryTexts.empty() && categoryTexts.size() >= index;
	}

	const std::string& GetCategoryText(unsigned int index) const {
		return categoryTexts[index];
	}


	bool EnemyNamesFileIsLoaded(unsigned int area) const {
		return enemyNames.count(area) > 0;
	}

	void LoadEnemyNames(unsigned int area) {
		auto filename = std::format("NewData\\Text\\text_enemynames_area{:03d}.txt", area);

		if (!std::filesystem::exists(filename))
			return;

		std::ifstream file(filename);

		std::vector<std::string> lines;

		ReadTextFileIntoVector(filename.c_str(), lines);

		enemyNames[area] = std::move(lines);
	}


	bool HasEnemyName(unsigned int area, unsigned int index) const {
		return EnemyNamesFileIsLoaded(area) && enemyNames.at(area).size() >= index;
	}

	const std::string& GetEnemyName(unsigned int area, unsigned int index) const {
		const auto& name = enemyNames.at(area)[index];

		if (enemyNameMap.count(name) > 0)
			return enemyNameMap.at(name);

		return name;
	}


	bool DialogueFileIsLoaded(unsigned int area) const {
		return dialogue.count(area) > 0;
	}

	void LoadDialogue(unsigned int area) {
		auto filename = std::format("NewData\\Text\\text_dialogue_area{:03d}.txt", area);

		if (!std::filesystem::exists(filename))
			return;

		std::ifstream file(filename);

		std::vector<std::string> lines;
		std::string line;

		while (std::getline(file, line)) {
			TranslateEncodedText(line);

			lines.push_back(line);
		}

		dialogue[area] = std::move(lines);
	}


	bool HasDialogue(unsigned int area, unsigned int index) const {
		return DialogueFileIsLoaded(area) && dialogue.at(area).size() > index;
	}

	const std::string& GetDialogue(unsigned int area, unsigned int index) const {
		return dialogue.at(area)[index];
	}


private:
	std::map<unsigned int, std::vector<std::string>> text;
	std::map<std::string, std::string> enemyNameMap;
	std::map<unsigned int, std::vector<std::string>> enemyNames;
	std::map<unsigned int, std::vector<std::string>> dialogue;

	std::vector<std::string> itemNames;
	std::vector<std::string> weaponNames;
	std::vector<std::string> armorNames;
	std::vector<std::string> accessoryNames;
	std::vector<std::string> skillNames;
	std::vector<std::string> menuTabTexts;
	std::vector<std::string> categoryTexts;
};