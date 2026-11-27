using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class World {
	public enum MasterId {
		Bunyan,
		Mygas,
		Yggdrasil,
		Dlonzo,
		Fahl,
		Durandal,
		Giotto,
		Hondara,
		Emitai,
		Deis,
		Hachio,
		Bais,
		Lang,
		Lee,
		Wynn,
		Ladon,
		Meryleep
	}

	public enum CharacterId {
		YoungRyu,
		YoungNina,
		Garr,
		Teepo,
		Rei,
		Momo,
		Peco,
		AdultRyu,
		AdultNina,
		YoungRyuPajamas,
		Whelp
	}

	public enum StatId {
		Power,
		Defence,
		Intellect,
		Agility
	}


	public unsafe struct Actor {
		public byte Flags;
		public byte State;
		public byte SubState;
		public byte Step;
		byte byte_4;
		public byte Id;
		byte byte_6;
		public byte Byte_7;
		public byte Direction;
		public byte Counter;
		byte randByte_A;
		byte byte_B;
		uint dword_C;
		uint dword_10;
		uint dword_14;
		public uint Dword_18;
		uint dword_1C;
		int int_20;
		public byte GraphicsFlags;
		public byte TexturePage;
		public byte TextureVOffset;
		public byte PaletteIndex;
		public byte MirrorX;
		public byte DrawIndex;
		public byte PaletteFlags;
		public byte SortBiasIndex;
		public ushort PaletteSet;
		public short X;
		public short Y;
		public ushort SortKey;
		public int WorldX;
		public int WorldY;
		public int WorldZ;
		public int ScaleX;
		public int ScaleY;
		public byte ScaleMode;
		public byte FrameCount;
		public byte FrameTimer;
		public byte AnimationId;
		public byte* SpriteData;
		public byte* AnimationData;
		public ushort* FrameTable;
		public ushort FrameIndex;
		public ushort FrameValue;
		public byte BlendMode;
		public Vec3b Colour;
		public int CameraDepth;
		public uint ResourceId;
		public uint SavedTextureVOffset;
		public byte gap_6C_0, gap_6C_1, gap_6C_2, gap_6C_3;
		public uint RenderFlags;
		public float fX;
		public float fY;
		public float fZ;
	};

	public struct Stats {
		public ushort Hp;
		public ushort Ap;
		public ushort Power;
		public ushort Defence;
		public ushort Agility;
		public ushort Intellect;
		byte byte_C;
		byte byte_D;
		byte byte_E;
		public byte FireResistance;
		public byte IceResistance;
		public byte ElectricResistance;
		public byte EarthResistance;
		public byte WindResistance;
		public byte HolyResistance;
		public byte PsionicResistance;
		public byte StatusResistance;
		public byte DeathResistance;
		byte byte_18;
		byte byte_19;
		byte byte_1A;
		byte byte_1B;
		byte byte_1C;
		byte byte_1D;
		byte byte_1E;
		byte byte_1F;
	};

	public struct MasterStatGrowth {
		public sbyte Hp;
		public sbyte Ap;
		public sbyte Power;
		public sbyte Defense;
		public sbyte Agility;
		public sbyte Intelligence;
	};

	public struct Character {
		public Array9<byte> Name;
		public byte Id;
		public byte Level;
		public byte CharacterFlags;
		public uint Xp;
		public ushort StatusFlags;
		public byte WeaponId;
		public byte ArmorId1;
		public byte ArmorId2;
		public byte ArmorId3;
		public byte OptionId1;
		public byte OptionId2;
		public ushort CurHp;
		public ushort CurAp;
		public byte CurWillpower;
		public byte ResistanceBoostFlags;
		public byte Fatigue;
		public byte MasterId;
		public Stats BattleStats;
		public Stats Stats;
		public Array4<Array10<byte>> Skills;
		public byte MasterStartLevel;
		public MasterStatGrowth MasterStatGrowth;
		unsafe fixed byte reserved[21];
	};

	public unsafe struct PersistentState {
		uint dword_0;
		uint dword_4;
		uint dword_8;
		uint dword_C;
		uint dword_10;
		byte eventStage;
		byte eventParam;
		ushort eventVal;
		uint dword_18;
		uint dword_1C;
		uint dword_20;
		ushort areaId;
		byte byte_26;
		byte facingDirection;
		uint worldX;
		uint worldY;
		fixed byte gap_30[9];
		byte byte_3A;
		fixed byte gap_3B[54];
		ushort checksum;
		fixed byte gap_72[6];
		byte messageSpeed;
		byte soundMode;
		public byte WindowColour;
		byte menuBg;
		byte byte_7C;
		byte hideBattleCommandText;
		byte autoRun;
		fixed byte gap_7F[17];
		public Array8<Character> Characters;
		fixed byte scriptLayerFlags[160];
		fixed byte eventFlags[32];
		fixed byte gap_670[8];
		uint zenny;
		ushort word_67C;
		byte byte_67E;
		byte byte_67F;
		byte formation;
		byte byte_681;
		fixed byte partyMembers[3];
		fixed byte reserveMembers[3];
		fixed byte flags_688[32];
		fixed byte flags_6A8[32];
		fixed byte flags_6C8[32];
		uint playTime;
		fixed byte openedChestFlags[32];
		fixed byte bytes_70C[32];
		fixed byte collectedFieldItemFlags[32];
		byte curPLIdx;
		byte byte_74D;
		byte byte_74E;
		byte byte_74F;
		byte byte_750;
		byte fieldMusicId;
		fixed byte gap_752[2];
		uint battleCount;
		uint totalZennyEarned;
		uint fieldItemsCollected;
		uint chestsOpened;
		uint dword_764;
		fixed byte gap_768[12];
		fixed byte itemIds[128];
		fixed byte weaponIds[128];
		fixed byte armorIds[128];
		fixed byte accessoryIds[128];
		fixed byte itemQuantities[128];
		fixed byte weaponQuantities[128];
		fixed byte armorQuantities[128];
		fixed byte accessoryQuantities[128];
		fixed byte keyItems[32];
		fixed byte skillNotes[128];
		fixed byte gap_C14[104];
		fixed uint countdownTimers[2];
		fixed byte gap_C84[28];
		fixed sbyte charName[5];
		fixed byte previewPartyMembers[3];
		byte previewLevel;
		byte previewWindowColour;
		byte previewMenuBg;
		fixed byte gap_CAB[1];
		uint previewPlayTime;
		uint previewExperience;
		bool previewEventFlag92;
		fixed byte gap_CB5[1];
		fixed sbyte previewNameSuffix[4];
		fixed byte gap_CBA[2];
		Array9<Input.Buttons> buttonMappings;
		fixed byte gap[994];
	};


	// TODO: Figure out a better way to handle this
	public const nint GAMEMODE_FIRST = 0x496B60;


	[GameFunction(0x574A60)]
	public static partial int GetXpForLevel(byte characterId, byte level);

	[GameFunction(0x5A98A0)]
	public static partial void UpdateScheduler();

	[GameFunction(0x5A9907)]
	public static partial void InitialiseModeStack();

	[GameFunction(0x5A9914)]
	public static partial void SetGameMode(int index, nint func);


	[GameVariable(0x64B390, arrayLength: 7)]
	public static partial Span<Character> DefaultCharacterDatas { get; set; }

	[GameVariable(0x7E0918)]
	public static partial byte FieldRenderFlags { get; set; }

	[GameVariable(0x9039E0)]
	public static partial ref PersistentState PersistentStateData { get; }

	[GameVariable(0x904EFC)]
	public static partial ushort CurrentAreaId { get; set; }

	[GameVariable(0x937F88, isPointer: true)]
	public static partial ref Actor CurrentActor { get; }

	[GameVariable(0x937F94)]
	public static partial uint FrameCount { get; set; }
}