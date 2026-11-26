using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Battle {
	public struct StatModifiers {
		public byte Power;
		public byte Defence;
		public byte Agility;
		public byte Intelligence;
		byte byte_4, byte_5, byte_6, byte_7;
	};

	public unsafe struct EnemyBattleData {
		public World.Actor Actor;
		public Array12<byte> Name;
		public byte EnemyId;
		public byte EnemyType;
		public byte ActionPattern;
		public bool IsHpKnown;
		public ushort AiFlags;
		public ushort StatusFlags;
		public ushort Zenny;
		public ushort Xp;
		public ushort Level;
		public ushort SpriteId;
		public Array8<byte> Skills;
		public ushort RemainingHp;
		public ushort RemainingAp;
		public Dat.EnemyLootInfo ItemSteal;
		public Dat.EnemyLootInfo ItemDrop;
		public World.Stats CurrentStats;
		public World.Stats BaseStats;
		public byte Index;
		public byte AiRuleFlags;
		public sbyte TargetOffsetX;
		public sbyte TargetOffsetY;
		public nint AiCallback;
		public ushort* AiScript;
		byte byte_FC;
		byte byte_FD;
		byte byte_FE;
		byte byte_FF;
		public byte AiHandlerId;
		public byte AttackSoundType;
		byte byte_102;
		byte byte_103;
		public byte TargetId;
		public byte ActionType;
		public ushort SkillId;
		byte byte_108;
		byte byte_109;
		byte byte_10A;
		byte byte_10B;
		public byte DamageFlags;
		public byte StatusTurns;
		byte byte_10E;
		byte byte_10F;
		public uint BattleFlags;
		public uint EffectFlags;
		public StatModifiers StatModifiers;
		byte byte_120;
		byte byte_121;
		public byte AttackBoostTurns;
		byte byte_123;
		public byte ChargeLevel;
		byte byte_125;
		ushort word_126;
	};

	public struct BattleCharacter {
		public Array8<byte> Name;
		byte byte_8;
		byte byte_9;
		public byte Level;
		unsafe fixed byte gap[321];
	};

	public struct Action {
		public byte TargetId;
		public byte Type;
		public ushort Id;
		unsafe fixed byte gap_4[4];
		public byte DamageFlags;
		public byte StatusEffectCounter;
		byte byte_A, byte_B;
		public uint BattleFlags;
		public uint EffectFlags;
		public StatModifiers StatModifiers;
		unsafe fixed byte gap_1C[2];
		public byte BerserkCounter;
		byte byte_1F;
		public byte ChargeLevel;
		byte byte_21;
		ushort word_22;
	}


	[GameFunction(0x42F680, hookable: true)]
	public static partial void StartUseSkill();

	[GameFunction(0x444EB0)]
	public static partial bool ShouldDrawEnemyName(byte index);

	[GameFunction(0x4946C0, hookable: true)]
	public static partial void LoadEnemyBattleData(byte slot, byte index);

	[GameFunction(0x598810)]
	public static partial uint GetXpToNextLevel(byte characterIndex);


	[GameVariable(0x802DC0, arrayLength: 3)]
	public static partial Span<BattleCharacter> BattleCharacters { get; set; }

	[GameVariable(0x904AB0)]
	public static partial byte PartyMemberCount { get; set; }

	[GameVariable(0x904B34)]
	public static partial byte CurrentAttackerIndex { get; set; }

	[GameVariable(0x904B80)]
	public static partial ushort CurrentActionId { get; set; }

	[GameVariable(0x93B960, arrayLength: 8)]
	public static partial Span<EnemyBattleData> EnemyBattleDatas { get; set; }
}
