using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BoF3Ext.Hooks;


static class Battle {
	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void StartUseSkillHook() {
		Game.Battle.OriginalStartUseSkill();

		if (Game.Battle.CurrentActionId != 151) {
			TextManager.Instance.TryGetSkillName(Game.Battle.CurrentActionId, out var name);

			Span<byte> buffer = Game.Text.TextParameterBuffers[14];
			Span<byte> nameSpan = new(name.GameString.Pointer, name.ByteCount + 1);

			nameSpan.CopyTo(buffer);
		}
	}

	[UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
	static unsafe void LoadEnemyBattleDataHook(byte slot, byte index) {
		ref var enemy = ref Game.Battle.EnemyBattleDatas[slot];
		var areaId = Game.World.CurrentAreaId;

		Game.Battle.OriginalLoadEnemyBattleData(slot, index);

		if (TextManager.Instance.TryGetEnemyName(areaId, index, out var name)) {
			Unsafe.WriteUnaligned(ref enemy.Name[0], 0x600DBEEFu);
			Unsafe.WriteUnaligned(ref enemy.Name[4], (nuint)name.GameString.Pointer);
		}
	}


	public static unsafe void Enable() {
		Game.Battle.HookStartUseSkill(&StartUseSkillHook);
		Game.Battle.HookLoadEnemyBattleData(&LoadEnemyBattleDataHook);
	}
}