using System;
using System.Collections.Generic;
using System.Text;

using BoF3Ext.Generators;

namespace BoF3Ext.Game;


static partial class Overworld {
	[GameFunction(0x403E00, hookable: true)]
	public static partial void UpdateLocationPanel();
}