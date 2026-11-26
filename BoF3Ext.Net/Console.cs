using BoF3Ext.Wrappers;

namespace BoF3Ext;


static partial class Console {
	const int STDOUT = 1;
	const int STDERR = 2;
	const uint UTF8 = 65001;

	static bool ownsConsole;


	public static bool AllocConsole() {
		if (ownsConsole)
			return true;

		if (!Win32.AllocConsole())
			return false;

		if (Win32.freopen_s(out var stdout, "CONOUT$", "w", Win32.GetStream(STDOUT)) != 0) {
			Win32.FreeConsole();

			return false;
		}

		if (Win32.freopen_s(out _, "CONOUT$", "w", Win32.GetStream(STDERR)) != 0) {
			Win32.fclose(stdout);
			Win32.FreeConsole();

			return false;
		}

		Win32.SetConsoleOutputCP(UTF8);
		ownsConsole = true;

		return true;
	}

	public static bool FreeConsole() {
		if (ownsConsole) {
			Win32.fclose(Win32.GetStream(STDOUT));
			Win32.fclose(Win32.GetStream(STDERR));
			ownsConsole = false;
		}

		return Win32.FreeConsole();
	}

	public static void Write(string? text)
		=> WriteTo(STDOUT, text, false);

	public static void WriteLine(string? text)
		=> WriteTo(STDOUT, text, true);

	public static void WriteError(string? text)
		=> WriteTo(STDERR, text, false);

	public static void WriteErrorLine(string? text)
		=> WriteTo(STDERR, text, true);


	static void WriteTo(uint index, string? text, bool newLine) {
		var stream = Win32.GetStream(index);

		if (text != null)
			Win32.fputs(text, stream);

		if (newLine)
			Win32.fputc('\n', stream);

		Win32.fflush(stream);
	}
}
