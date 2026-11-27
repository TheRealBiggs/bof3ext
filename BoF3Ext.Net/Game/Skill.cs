using System;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;

static partial class Skill {
	public struct SkillData {
		public Array16<byte> Name;
		public byte TargetFlags;
		public byte CategoryAndExamineChance;
		public byte Cost;
		public byte Power;
		public byte Element;
		readonly byte gap0;
		public ushort DescriptionId;
	};


	[GameVariable(0x65C4C8, arrayLength: 228)]
	public static partial Span<SkillData> SkillDatas { get; set; }
}