using System;
using System.IO;

namespace BoF3Ext.Generators;


static class EmbeddedResources {
	public static string ReadAllText(string name) {
		using var stream = typeof(EmbeddedResources).Assembly.GetManifestResourceStream(name)
			?? throw new InvalidOperationException($"Embedded resource '{name}' was not found.");

		using var reader = new StreamReader(stream);

		return reader.ReadToEnd();
	}
}