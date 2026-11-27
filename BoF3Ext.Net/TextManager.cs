using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace BoF3Ext;


class TextManager : Singleton<TextManager> {
	public record class TextEntry(GameString GameString, int Length, int ByteCount);


	readonly Dictionary<uint, TextEntry[]> generalTexts = [];
	readonly Dictionary<uint, TextEntry[]> dialogue = [];
	readonly Dictionary<uint, TextEntry[]> enemyNames = [];
	readonly Dictionary<string, TextEntry> enemyNameMap = [];
	readonly Dictionary<uint, TextEntry> locations = [];

	TextEntry[] itemNames = [];
	TextEntry[] weaponNames = [];
	TextEntry[] armourNames = [];
	TextEntry[] accessoryNames = [];
	TextEntry[] skillNames = [];
	TextEntry[] menuTabTexts = [];
	TextEntry[] categoryTexts = [];
	TextEntry[] configTexts = [];
	TextEntry[] battleCommands = [];
	TextEntry[] battleActions = [];
	TextEntry[] statNames = [];
	TextEntry[] skillNoteTexts = [];
	TextEntry[] formationNames = [];
	TextEntry[] statusTexts = [];
	TextEntry? masterText;


	public void Initialise() {
		if (File.Exists("NewData/Text/text_enemynames_map.txt")) {
			var lines = File.ReadAllLines("NewData/Text/text_enemynames_map.txt");

			foreach (var line in lines) {
				var oldNameEnd = line.IndexOf('=');
				var oldName = line[..oldNameEnd];
				var newName = line[(oldNameEnd + 1)..];

				enemyNameMap[oldName] = EncodeText(newName);
			}
		}

		if (File.Exists("NewData/Text/text_locationnames.txt")) {
			var lines = File.ReadAllLines("NewData/Text/text_locationnames.txt");

			foreach (var line in lines) {
				var idEnd = line.IndexOf('=');
				var id = line[..idEnd];
				var name = line[(idEnd + 1)..];

				locations[UInt32.Parse(id)] = EncodeText(name);
			}
		}

		if (File.Exists("NewData/Text/text_itemnames.txt"))
			itemNames = [.. File.ReadAllLines("NewData/Text/text_itemnames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_weaponnames.txt"))
			weaponNames = [.. File.ReadAllLines("NewData/Text/text_weaponnames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_armornames.txt"))
			armourNames = [.. File.ReadAllLines("NewData/Text/text_armornames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_accessorynames.txt"))
			accessoryNames = [.. File.ReadAllLines("NewData/Text/text_accessorynames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_skillnames.txt"))
			skillNames = [.. File.ReadAllLines("NewData/Text/text_skillnames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_menutabs.txt"))
			menuTabTexts = [.. File.ReadAllLines("NewData/Text/text_menutabs.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_categories.txt"))
			categoryTexts = [.. File.ReadAllLines("NewData/Text/text_categories.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_config.txt"))
			configTexts = [.. File.ReadAllLines("NewData/Text/text_config.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_battlecommands.txt"))
			battleCommands = [.. File.ReadAllLines("NewData/Text/text_battlecommands.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_battleactions.txt"))
			battleActions = [.. File.ReadAllLines("NewData/Text/text_battleactions.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_stats.txt"))
			statNames = [.. File.ReadAllLines("NewData/Text/text_stats.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_skillnotes.txt"))
			skillNoteTexts = [.. File.ReadAllLines("NewData/Text/text_skillnotes.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_formationnames.txt"))
			formationNames = [.. File.ReadAllLines("NewData/Text/text_formationnames.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_status.txt"))
			statusTexts = [.. File.ReadAllLines("NewData/Text/text_status.txt").Select(EncodeText)];

		if (File.Exists("NewData/Text/text_master.txt"))
			masterText = EncodeText(File.ReadAllLines("NewData/Text/text_master.txt")[0]);
	}


	public bool TryGetGeneralText(uint datId, uint index, out TextEntry text)
		=> TryGetReplacementText(
			generalTexts, datId, index, GeneralTextKeyFunc, GeneralTextIndexFunc, GeneralTextFilepathFunc, out text
		);

	public bool TryGetDialogue(uint area, uint index, out TextEntry text)
		=> TryGetReplacementText(dialogue, area, index, SimpleKeyFunc, SimpleIndexFunc, DialogueFilepathFunc, out text);

	public bool TryGetEnemyName(uint area, uint index, out TextEntry name)
		=> TryGetReplacementText(
			enemyNames, area, index, SimpleKeyFunc, SimpleIndexFunc, EnemyNameFilepathFunc, out name, enemyNameMap
		);

	public bool TryGetLocationName(uint id, [NotNullWhen(true)] out TextEntry? name)
		=> locations.TryGetValue(id, out name);

	public bool TryGetItemName(uint index, out TextEntry name)
		=> TryGetSimpleReplacementText(itemNames, index, out name);

	public bool TryGetWeaponName(uint index, out TextEntry name)
		=> TryGetSimpleReplacementText(weaponNames, index, out name);

	public bool TryGetArmourName(uint index, out TextEntry name)
		=> TryGetSimpleReplacementText(armourNames, index, out name);

	public bool TryGetAccessoryName(uint index, out TextEntry name)
		=> TryGetSimpleReplacementText(accessoryNames, index, out name);

	public bool TryGetSkillName(uint index, out TextEntry name)
		=> TryGetSimpleReplacementText(skillNames, index, out name);

	public bool TryGetMenuTabText(uint index, out TextEntry text)
		=> TryGetSimpleReplacementText(menuTabTexts, index, out text);

	public bool TryGetCategoryText(uint index, out TextEntry text)
		=> TryGetSimpleReplacementText(categoryTexts, index, out text);

	public bool TryGetConfigText(uint index, out TextEntry text)
		=> TryGetSimpleReplacementText(configTexts, index, out text);

	public bool TryGetBattleCommandText(uint index, out TextEntry text)
		=> TryGetSimpleReplacementText(battleCommands, index, out text);

	public bool TryGetBattleActionText(uint index, out TextEntry text)
		=> TryGetSimpleReplacementText(battleActions, index, out text);

	public bool TryGetStatName(Game.World.StatId id, out TextEntry text)
		=> TryGetSimpleReplacementText(statNames, (uint)id, out text);

	public bool TryGetSkillNoteText(uint id, out TextEntry text)
		=> TryGetSimpleReplacementText(skillNoteTexts, id, out text);

	public bool TryGetFormationName(uint id, out TextEntry text)
		=> TryGetSimpleReplacementText(formationNames, id, out text);

	public bool TryGetStatusText(uint id, out TextEntry text)
		=> TryGetSimpleReplacementText(statusTexts, id, out text);

	public bool TryGetMasterText(out TextEntry text) {
		if (masterText != null) {
			text = masterText;

			return true;
		}

		text = new(default, 0, 0);

		return false;
	}

	public static ushort EncodeCharacter(char c)
		=> (ushort)(((c | 0x8000) >> 8) | ((c & 0xFF) << 8));


	static bool TryGetReplacementText(
		Dictionary<uint, TextEntry[]> map,
		uint arg1,
		uint arg2,
		Func<uint, uint, uint> keyFunc,
		Func<uint, uint, uint> indexFunc,
		Func<uint, uint, string> filepathFunc,
		out TextEntry text,
		Dictionary<string, TextEntry>? replacementMap = null
	) {
		var key = keyFunc(arg1, arg2);
		var index = indexFunc(arg1, arg2);

		if (!map.TryGetValue(key, out var texts)) {
			var filepath = filepathFunc(arg1, arg2);

			if (!File.Exists(filepath)) {
				text = new(default, 0, 0);

				return false;
			}

			var lines = File.ReadAllLines(filepath);
			texts = new TextEntry[lines.Length];

			for (int i = 0; i < lines.Length; i++) {
				var line = lines[i];

				texts[i] = replacementMap != null && replacementMap.TryGetValue(line, out var replacement)
					? replacement
					: EncodeText(line);
			}

			map[key] = texts;
		}

		if (index >= texts.Length) {
			text = new(default, 0, 0);

			return false;
		}

		text = texts[index];

		return true;
	}

	static unsafe TextEntry EncodeText(string text) {
		var textSpan = text.AsSpan();

		MemoryStream stream = new();

		for (int i = 0; i < text.Length;) {
			var c = textSpan[i++];

			if (c == '{') {
				c = textSpan[i++];

				if (c == '{') {
					var tag = textSpan.Slice(i, 5);
					i += 5;

					switch (tag) {
						case "NLINE":
							stream.WriteByte(0x01);
							break;

						case "NPAGE":
							stream.WriteByte(0x02);
							break;

						case "CHRTR":
							stream.WriteByte(0x03);
							break;

						case "COLRS":
							stream.WriteByte(0x05);
							i++;    // ','
							stream.WriteByte((byte)(textSpan[i++] - '0'));
							break;

						case "COLRE":
							stream.WriteByte(0x06);
							break;

						case "PRMTR": {
							stream.WriteByte(0x07);
							i++;    // ','

							var val = (byte)GetTagIntValue(textSpan[i..], out var length);
							i += length;

							stream.WriteByte(val);
							break;
						}

						case "SOUND": {
							stream.WriteByte(0x0A);
							i++;    // ','

							var val = Byte.Parse(textSpan.Slice(i, 2), System.Globalization.NumberStyles.HexNumber);
							i += 2;

							stream.WriteByte(val);
							break;
						}

						case "PAUSE":
							stream.WriteByte(0x0B);
							break;

						case "TXTBX": {
							stream.WriteByte(0x0C);
							i++;    // ','

							var val = Byte.Parse(textSpan.Slice(i, 2), System.Globalization.NumberStyles.HexNumber);
							i += 2;

							stream.WriteByte(val);
							break;
						}

						case "EFCTS":
							stream.WriteByte(0x0D);
							break;

						case "EFCTE": {
							stream.WriteByte(0x0E);
							stream.WriteByte(0x0F);
							i++;    // ','

							var val = Byte.Parse(textSpan.Slice(i, 2), System.Globalization.NumberStyles.HexNumber);
							i += 2;

							stream.WriteByte(val);
							break;
						}

						case "UNK0F":
							stream.WriteByte(0x0F);
							break;

						case "INSTS":
							stream.WriteByte(0x10);
							break;

						case "INSTE":
							stream.WriteByte(0x11);
							break;

						case "CHOIC": {
							stream.WriteByte(0x14);
							i++;    // ','

							// Id
							var val = (byte)GetTagIntValue(textSpan[i..], out var length);
							i += length;
							i += 1;	// ','

							stream.WriteByte(val);

							// Type | Count start
							stream.WriteByte(0x0C);

							val = (byte)GetTagIntValue(textSpan[i..], out length);
							i += length;
							i += 1;	// ','

							// Read choices
							List<string> choices = [];

							while (textSpan[i] != '}') {
								var choice = GetTagStringValue(textSpan[i..]);
								i += choice.Length;

								if (textSpan[i] == ',')
									i++;

								choices.Add(new(choice));
							}

							// Type | Count write
							stream.WriteByte((byte)((val << 4) | choices.Count));

							// Choices
							for (int j = 0; j < choices.Count; j++) {
								var choice = choices[j];

								foreach (var _c in choice) {
									if (_c > 0x7F)
										WriteUnicodeCharacter(stream, _c);
									else
										stream.WriteByte((byte)_c);
								}

								if (j < choices.Count - 1)
									stream.WriteByte(0x00);
							}

							break;
						}

						case "DURTN": {
							stream.WriteByte(0x16);
							i++;    // ','

							var val = (byte)GetTagIntValue(textSpan[i..], out var length);
							i += length;

							stream.WriteByte(val);
							break;
						}

						case "UNK5F":
							stream.WriteByte(0x5F);
							break;

						default:
							if (tag.StartsWith("SYM")) {
								var val = Byte.Parse(tag[3..], System.Globalization.NumberStyles.HexNumber);

								stream.WriteByte(val);
							} else
								throw new InvalidDataException($"Unknown tag '{tag}'");

							break;
					}

					i += 2;	// "}}"
				} else {
					if (c > 0x7F)
						WriteUnicodeCharacter(stream, c);
					else
						stream.WriteByte((byte)c);
				}
			} else {
				if (c > 0x7F)
					WriteUnicodeCharacter(stream, c);
				else
					stream.WriteByte((byte)c);
			}
		}

		var data = stream.ToArray();

		var pointer = (byte*)NativeMemory.Alloc((nuint)data.Length + 1);
		data.CopyTo(new Span<byte>(pointer, data.Length));
		pointer[data.Length] = 0;

		return new(new(pointer), text.Length, data.Length);
	}

	static void WriteUnicodeCharacter(MemoryStream stream, char c) {
		stream.WriteByte((byte)((c >> 8) | 0x80));
		stream.WriteByte((byte)(c & 0xFF));
	}

	static int GetTagIntValue(ReadOnlySpan<char> text, out int length) {
		length = 0;

		while (text[length] != '}' && text[length] != ',')
			length++;

		return Int32.Parse(text[..length]);
	}

	static ReadOnlySpan<char> GetTagStringValue(ReadOnlySpan<char> text) {
		var length = 0;

		while (text[length] != '}' && text[length] != ',')
			length++;

		return text[..length];
	}

	static uint SimpleKeyFunc(uint arg1, uint _)
		=> arg1;

	static uint SimpleIndexFunc(uint _, uint arg2)
		=> arg2;

	static uint GeneralTextKeyFunc(uint arg1, uint arg2)
		=> (arg1 << 16) | (arg2 >> 14);

	static uint GeneralTextIndexFunc(uint _, uint arg2)
		=> arg2 & 0x3FFF;

	static string GeneralTextFilepathFunc(uint arg1, uint arg2) {
		var datName = Game.Dat.FileNames[(int)arg1].ToString();
		datName = datName[..^4];

		return $"NewData/Text/text_general_{datName}_{arg2 >> 14}.txt";
	}

	static string DialogueFilepathFunc(uint arg1, uint _)
		=> $"NewData/Text/text_dialogue_area{arg1:D3}.txt";

	static string EnemyNameFilepathFunc(uint arg1, uint _)
		=> $"NewData/Text/text_enemynames_area{arg1:D3}.txt";

	static bool TryGetSimpleReplacementText(TextEntry[] array, uint index, out TextEntry text) {
		if (index < array.Length) {
			text = array[index];

			return true;
		}

		text = new(default, 0, 0);

		return false;
	}
}